#include <velib/velib_inc_J1939.h>

#include <velib/base/base.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939_device.h>
#include <velib/J1939/J1939_nmt.h>
#include <velib/J1939/J1939_request.h>
#include <velib/nmea2k/n2k.h>
#include <velib/vecan/upd.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/reg_rx.h>
#include <velib/vecan/vecan.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_todo.h>
#include <velib/utils/ve_logger.h>

#define MOD "UPD"

#include <stdio.h>

/**
 * @addtogroup VELIB_VECAN_UPD
 *
 * Implements a channel to the UDF for updating a single device (VUP).
 *
 * @details
 *	See design documents for more detail. Intended use is:
 *
 *  updInit();
 *
 *	updBegin(device, 0, veFalse)
 *	wait for state == UPD_UPDATE_READY or UPD_ERROR.
 *
 *	updSend(someDataFunction);
 *  wait for state == UPD_DONE or UPD_ERROR.
 *
 *  updSend(someOtherDataFunction);
 *  wait for state == UPD_DONE or UPD_ERROR.
 *
 *  updEnd()
 *  wait for state == UPD_ENDED
 */

UpdVars		updVars;						//< state

#define UPD_WAIT_TIMEOUT			15*20	// * 50ms
#define UPD_UPDATE_ENABLE_REPEAT	10		// * 50 ms

/**
 * Init / reset to idle...
 */
void updInit(void)
{
	updVars.state = UPD_IDLE;
}

/**
 * Start to update an device on the network.
 *
 * @retval veFalse when an update is already busy.
 */
veBool updBegin(J1939FuncTp* device, un8 instance, veBool panic)
{
	veAssert(device != NULL && device != NULL);

	// check if not busy...
	if (updVars.state != UPD_IDLE && updVars.state != UPD_ENDED)
		return veFalse;

	updVars.udf = device;
	updVars.vupInstance = instance;

	if (panic) {
		updVars.timeout = UPD_WAIT_TIMEOUT;
		updVars.state = UPD_UPDATE_BEGIN;
	}
	else {
		updVars.state = UPD_UPDATE_ENABLE;
		updVars.timeout = 0;
	}

	return veTrue;
}

/**
 * Register callback to send / receive data.
 *
 * @param dataFunc
 *	The function handling the data to / from the UDF. It can expect @ref 
 *	VE_EV_VUP_TX and @ref VE_EV_VUP_RX events for poll and deliver data 
 *	respectively. The callback can read / write directly from the message
 *	by @ref VE_IO_STREAM.
 *
 *	This is a seperate functions since several files can be send after 
 *	eachother. The function should be called after updBegin was called and 
 *	the state changed to @ref UPD_UPDATE_READY or a file was uploaded 
 *	succesfully, i.o.w. state is @ref UPD_DONE.
 */
veBool updSend(UpdDataFunc dataFunc)
{
	if (updVars.state != UPD_UPDATE_READY && updVars.state != UPD_DONE)
		return veFalse;

	updVars.dataFunc = dataFunc;
	updVars.state = UPD_SENDING;
	return veTrue;
}

/**
 * End the current update.
 */
void updEnd(void)
{
	updVars.state = UPD_SEND_END;
}

/** 
 * The actual work.
 * 
 * Expects:
 *	- VE_EV_TICK
 *	- VE_EV_UPDATE
 *	- VE_EV_VREG_RX_ACK / to be added to @ref veRegRxHndlrDef.
 */
void updHandler(VeEvent ev)
{
	if (updVars.state == UPD_IDLE)
		return;

	if (ev == VE_EV_TICK)
	{
		if (updVars.timer != 0)
			updVars.timer--;

		/*
		* Something is quite wrong if this happens. Most likely the device is no
		* longer on the bus otherwise it should have send a timeout to us, since
		* it should timeout before we do.
		*/
		if (updVars.timeout != 0)
		{
			if (--updVars.timeout == 0)
			{
				updVars.errorCode = VACK_ERR_LOCAL_TIMEOUT;
				updVars.state = UPD_ERROR;
				return;
			}
		}
	}

	/* Only handle the VREGs from the UDF to us. */
	if (ev == VE_EV_VREG_RX_ACK || ev == VE_EV_VREG_RX)
	{
		if (
				n2kMsgIn->msgId.p.src != updVars.udf->nmt.nad ||
				n2kMsgIn->msgId.p.ps != j1939Device.nad
			)
		{
			return;
		}

		if (VACK_IS_ERROR(veRegAckCode))
			logE(MOD, "RegId 0x%04X nacked with %X", veRegRxId, veRegAckCode);
	}

	/*
	* Any error targeted directly to this device means that an action of this
	* device caused the problem; this is fatal. Broadcasted errors are not fatal
	* but just repeat the last encountered error, e.g. no valid firmware.
	*
	* Timeouts during the setup of the "connection" are ignored since it might be
	* from a previous connection.
	*/
	if (
			ev == VE_EV_VREG_RX_ACK && veRegRxId == VE_REG_UPDATE_DATA 
			&& VACK_IS_ERROR(veRegAckCode) && 
			!(updVars.state == UPD_UPDATE_BEGIN && veRegAckCode == VACK_ERR_TIMEOUT)
		)
	{
		updVars.timeout = 0;
		updVars.errorCode = veRegAckCode;
		updVars.state = UPD_ERROR;
		return;
	}

	switch (updVars.state)
	{
	case UPD_UPDATE_ENABLE:

		// Send the message that udf:vupInstance needs to be updated.
		switch (ev)
		{
		case VE_EV_TICK:
			veRegPrepareSf(updVars.udf->nmt.nad, VE_REG_UPDATE_ENABLE);
			if (n2kMsgOut == NULL)
				return;

			veOutUn8(updVars.vupInstance);
			veOutSendSf();
			updVars.state = UPD_UPDATE_BEGIN;
			updVars.timer = 0;
			updVars.timeout = UPD_WAIT_TIMEOUT;
			break;

		default:
			break;
		}
		break;

	case UPD_UPDATE_BEGIN:
		// Periodically send enable update mode...
		switch (ev)
		{
		case VE_EV_TICK:
			if (updVars.timer == 0 && updSendUpdateBegin(&updVars.udf->nmt.name, updVars.panic))
				updVars.timer = UPD_UPDATE_ENABLE_REPEAT;
			break;

		// The UDF should send an ACK for DATA_BEGIN when ready.
		case VE_EV_VREG_RX_ACK:
			if (veRegRxId == VE_REG_UPDATE_DATA && veRegAckCode == VACK_BEGIN)
			{
				updVars.state = UPD_UPDATE_READY;
				updVars.timeout = 0;
			}
			break;
		
		default:
			break;
		}
		break;

	case UPD_SENDING:
	{
		switch (ev)
		{
		case VE_EV_TICK:
		case VE_EV_UPDATE:
			veRegPrepareFp(updVars.udf->nmt.nad, VE_REG_UPDATE_DATA);
			if (n2kMsgOut == NULL)
				return;

			// Send the next data fragment...
			veOutUn8(0xFF);							/* reserved */
			if (updVars.dataFunc(VE_EV_VUP_TX))		/* poll for data */
				updVars.state = UPD_DONE_WAIT;		/* last fragment */
			else
				updVars.state = UPD_ACK_WAIT;

			veOutFinalize();
			n2kFpSequence((N2kFp*) n2kMsgOut, &veRegFpSeq);
			j1939TxPush(n2kMsgOut);
			updVars.timeout = UPD_WAIT_TIMEOUT;
			break;
		
		default:
			break;
		}
		break;
	}

	// Waiting for the data to be handled... UDF should respond with an ACK / continue.
	case UPD_ACK_WAIT:
		switch (ev)
		{
		case VE_EV_VREG_RX_ACK:
			if (veRegRxId == VE_REG_UPDATE_DATA && veRegAckCode == VACK_CONT)
			{
				updVars.dataFunc(VE_EV_VUP_RX);		/* optional returned data */
				updVars.state = UPD_SENDING;
				updVars.timeout = 0;
				veTodo();
			}
			break;

		default:
			break;
		}
		break;

	// The last ACK should be 'OK'.
	case UPD_DONE_WAIT:
		switch (ev)
		{
		case VE_EV_VREG_RX_ACK:
			if (veRegRxId == VE_REG_UPDATE_DATA && veRegAckCode == VACK_OK)
			{
				updVars.state = UPD_DONE;
				updVars.timeout = 0;
			}
			break;

		default:
			break;
		}
		break;

	// End update -> resume normal operation.
	case UPD_SEND_END:
		switch (ev)
		{
		case VE_EV_TICK:
		case VE_EV_UPDATE:
			if (updSendUpdateEnd(updVars.udf->nmt.nad))
				updVars.state = UPD_ENDED;
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}
}

/**
 * Send unlock message to keep the device in update mode.
 *
 * @param name
 *		The NAME of the device which must unlocked.
 * @param panic
 *		Overrules the unlock and keeps every starting bootloader in boot mode.
 * @return
 *		whether the message is queued for sending.
 */
veBool updSendUpdateBegin(J1939Name* name, veBool panic)
{
	veOutAllocSf();

	if (n2kMsgOut == NULL)
		return veFalse;

	n2kMsgOut->msgId.w = J1939ID_PRI(7) | J1939ID_PGN(PGN0_VECAN_UPDATE_BEGIN);
	veOutUn16Le(N2K_MAN_TAG_VICTRON);

	if (panic) {
		veOutUn32Le(0xFFFFFFFF);
		veOutUn8(0xFF);
		veOutUn8(0xAA);
	}
	else {
		veOutUn32Le(name->low);
		veOutUn8(name->high.fields.function);
		veOutBits8Le(name->high.fields.reserved, 1);
		veOutBits8Le(name->high.fields.devClass, 7);
	}

	veOutSendSf();
	return veTrue;
}

/**
 * Send message to end the update.
 *
 * @retval
 *		whether the message is queued for sending.
 */
veBool updSendUpdateEnd(J1939Nad nad)
{
	veRegPrepareSf(nad, VE_REG_UPDATE_END);

	if (n2kMsgOut == NULL)
		return veFalse;
	veOutSendSf();

	return veTrue;
}
