/*
* The following file handles the startup and general part of the update
* procedure for devices with a bootloader.
*
* Normally the application will be verified for correctness while waiting for
* an explicit request in the meantime. During this period, the device did
* not start the ACL procedure yet (only listening). If the application is
* valid and the listen period has elapsed without receiving a request to stay
* in bootloader mode, the application shall be booted by the bootEvent
* callback.
*
* Remain in bootloader mode can be forced by sending the proprietary message,
* PGN0_VECAN_UPDATE_BEGIN. A seperate message is used instead of the VREGs
* since 6 bytes of data are needed to carry the NAME of the device which should
* remain in bootmode. All booting devices can be forced to stay in bootload by
* setting a special value.
*/

#include <velib/base/base.h>
#include <velib/velib_inc_J1939.h>

#include <velib/boot/boot.h>
#include <velib/boot/prog.h>
#include <velib/boot/verify.h>

#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939_device.h>
#include <velib/nmea2k/n2k.h>
#include <velib/vecan/vecan.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/reg_rx.h>
#include <velib/vecan/reg_prod_sf.h>

#define CFG_BOOT_TICK_MSEC			50

#define CFG_BOOT_LISTEN_MSEC		1000
#define CFG_BOOT_UPD_IDLE_MSEC		5000
#define CFG_BOOT_UPD_TIMEOUT_MSEC	10000


#define BOOT_LISTEN_TICKS 			(CFG_BOOT_LISTEN_MSEC / CFG_BOOT_TICK_MSEC)
#define BOOT_UPD_TIMEOUT_TICKS		(CFG_BOOT_UPD_TIMEOUT_MSEC / CFG_BOOT_TICK_MSEC)
#define BOOT_UPD_IDLE_TICKS			(CFG_BOOT_UPD_IDLE_MSEC / CFG_BOOT_TICK_MSEC)

#define BOOT_ENABLE_FORCE			0xAA

// State of the boot / update
BootVars veBoot;

void bootInit(void)
{
	verifyInit();
	veBoot.updReq = veFalse;
	veBoot.timer = BOOT_LISTEN_TICKS;
	veBoot.state = BOOT_APP_VALIDATING;
}


// Request for normal mode.
void bootVRegHandler(VeEvent ev)
{
	if ((ev == VE_EV_VREG_RX || ev == VE_EV_VREG_RX_CMD) && veRegRxId == VE_REG_UPDATE_END) {
		/* When the update is part of the app, accept a new vup connection */
		veBoot.state = BOOT_UPD_WAIT;
		veBoot.updReq = veFalse;

		/* For bootloaders this call is expected not to return */
		bootUpdEndEvent();
		veRegRxHandled = veTrue;
	}
}

void bootHandler(VeEvent ev)
{
	// Only allow one active updater at a time.
	if ((ev & VE_EV_MSG_RX) && veBoot.updReq && veBoot.updNad != n2kMsgIn->msgId.p.src)
	{
		// (Try to) send a nack to indicate that bootloader is busy.
		if (n2kMsgIn->msgId.p.ps != J1939_NAD_BROADCAST)
			veRegSendAck(n2kMsgIn->msgId.p.src, VE_REG_UPDATE_DATA, VACK_ERR_BUSY);
		return;
	}

	if (ev == VE_EV_TICK && veBoot.timer > 0)
		veBoot.timer--;

	// A normal start can normally be prevented by sending a PGN0_VECAN_UPDATE_BEGIN
	// message, with the correct NAME attached. In case of a broken application, it is
	// however impossible to obtain the correct NAME. For that reason, the bootmode is
	// also entered if the last byte is 0xAA. In that case any device restarting will
	// remain in update mode.
	if (ev == VE_EV_MSG_RX)
	{
		if (
				n2kMsgIn->mdata[7] == BOOT_ENABLE_FORCE ||
				(
					veInUn32Le() == j1939Device.name.low &&
					veInUn8() == j1939Device.name.high.fields.function &&
					((veInUn8() >> 1) == j1939Device.name.high.fields.devClass)
				)
			)
		{
			veBoot.updReq = veTrue;
			veBoot.updNad = n2kMsgIn->msgId.p.src;
			veBoot.panic = n2kMsgIn->mdata[7] == BOOT_ENABLE_FORCE;
		}
	}

	// Handle the data.
	switch (veBoot.state)
	{
		// Check the application, only boot when valid; enable update mode when invalid
		// or requested to do so.
		case BOOT_APP_VALIDATING:
			switch (ev)
			{
				case VE_EV_UPDATE:
					// busy, continue to validate application
					if (verifyState == VERIFY_BUSY) {
						verifyUpdate();

					// normal situation: valid and no update requested
					} else if (verifyState == VERIFY_PASSED && !veBoot.updReq) {

						// wait for timer to expire to allow an updater to connect
						if (veBoot.timer == 0) {
							bootEvent();
						}

					// update requested or invalid firmware
					} else {
						// enable bootloader and check for updater presence
						j1939AclEnable();
						veBoot.timer = 0;
						veBoot.state = BOOT_UPD_WAIT;
						veBoot.ackCode = VACK_ERR_UPD_INVALID;
						bootUpdEnterEvent();
					}
					break;

				default:
					break;
			}
			break;

		// Wait for the updater to "connect". Note: to shorten the normal boot time,
		// the enable update is already accepted during the validation. If validating
		// takes long there is thus no need to add an additional longer delay AFTER
		// the validation process.
		case BOOT_UPD_WAIT:
			if (veBoot.updReq)
			{
				progBegin();
				veBoot.ackCode = VACK_BEGIN;
				veBoot.state = BOOT_DATA_SEND_ACK;

			} else if (veBoot.ackCode != VACK_OK && veBoot.timer == 0) {
				// no (longer) an updater connected -> periodically broadcast state
				veBoot.updNad = J1939_NAD_BROADCAST;
				veBoot.state = BOOT_DATA_SEND_ACK;
			}
			break;

		// Waiting for data.
		case BOOT_DATA_WAIT:
			switch (ev)
			{
				case VE_EV_TICK:
					// Send an error when no data is received in time.
					if (veBoot.timer == 0)
					{
						veBoot.ackCode = VACK_ERR_TIMEOUT;
						veBoot.state = BOOT_DATA_SEND_ACK;
					}
					break;

				case VE_EV_VREG_RX:
				case VE_EV_VREG_RX_CMD:
					// Handle the incoming data
					if (veRegRxId == VE_REG_UPDATE_DATA)
					{
						veRegRxHandled = veTrue;
						(void)veInUn8();	// reserved byte
						veBoot.ackCode = progData();
						veBoot.state = (veBoot.ackCode == VACK_ERR_BUSY ? BOOT_DATA_HANDLE : BOOT_DATA_SEND_ACK);
					}
					break;

				default:
					break;
			}
			break;

		case BOOT_DATA_HANDLE:
			// erasing flash pages etc might take some time..
			veBoot.ackCode = progUpdate();
			if (veBoot.ackCode != VACK_ERR_BUSY) {
				veBoot.state = BOOT_DATA_SEND_ACK;
			}
			break;


		// Send the ACK to the connected updater.
		case BOOT_DATA_SEND_ACK:
			if (veRegSendAck(veBoot.updNad, VE_REG_UPDATE_DATA, veBoot.ackCode))
			{
				// If in an error state -> end "connection" and revert to idle
				if (veBoot.ackCode & VACK_ERR)
				{
					veBoot.updReq = veFalse;
					veBoot.timer = BOOT_UPD_IDLE_TICKS;
					veBoot.state = BOOT_UPD_WAIT;

				// ack is send, wait for more data.
				} else {
					veBoot.timer = BOOT_UPD_TIMEOUT_TICKS;
					veBoot.state = BOOT_DATA_WAIT;
				}
			}
			break;
	}
}
