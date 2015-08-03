#include <velib/velib_inc_mk2.h>

#include <velib/mk2/frame_handler.h>
#include <velib/mk2/vebus_aux_device.h>
#include <velib/mk2/vebus_system.h>
#include <velib/base/endian_generic.h>
#include <velib/utils/ve_logger.h>

static Mk2EventReply dmcHandler(Mk2Event event, Mk2MsgRx* msg)
{
	switch (event)
	{
	case MK2_FRAME:
		if (msg->length >= 10 && msg->buf[0] == 0x20 && msg->buf[5] == 0x80) {
			vebusSystem.dmcVersion = le_u32_to_cpu_p(&msg->buf[6]);
			logI("aux_devices", "PMC found");
			return MK2_DONE;
		}
		return MK2_CONTINUE;

	case MK2_TIMEOUT:
		logI("aux_devices", "no PMC found");
		return MK2_DONE;

	default:
		return MK2_CONTINUE;
	}
}

/*
 * Other devices than Inverters / Chargers / Multi´s can exist on vebus,
 * which are not supported or limited supported. E.g. VE.Bus BMS / Multi
 * control panels and other accesories.
 */
void vebusCheckDmcPresence(Mk2Callback *callback)
{
	Mk2MsgTx *msg = mk2TxAlloc();

	/* poll PMC version */
	msg->length = 0x06;
	msg->buf[0] = 0x21;
	msg->buf[1] = 0xFF;
	msg->buf[2] = 0xFF;
	msg->buf[3] = 0xFF;
	msg->buf[4] = 0xFF;
	msg->buf[5] = 0x80;
	msg->buf[6] = 0x4D;
	msg->addChecksum = veFalse;
	msg->callback = callback;
	msg->handler = dmcHandler;
	msg->retries = 2;

	logI("aux", "check PMC presence");
	mk2FhQueueFrame(msg);
}

static Mk2EventReply bmsHandler(Mk2Event event, Mk2MsgRx* msg)
{
	switch (event)
	{
	case MK2_FRAME:
		if (msg->length >= 10 && msg->buf[0] == 0x20 && msg->buf[5] == 0x83) {
			vebusSystem.bmsVersion = le_u32_to_cpu_p(&msg->buf[6]);
			logI("aux_devices", "BMS found");
			return MK2_DONE;
		}
		return MK2_CONTINUE;

	case MK2_TIMEOUT:
		logI("aux_devices", "no BMS found");
		return MK2_DONE;

	default:
		return MK2_CONTINUE;
	}
}

/*
 * Other devices than Inverters / Chargers / Multi´s can exist on vebus,
 * which are not supported or limited supported. E.g. VE.Bus BMS / Multi
 * control panels and other accesories.
 */
void vebusCheckBmsPresence(Mk2Callback *callback)
{
	Mk2MsgTx *msg = mk2TxAlloc();

	/* poll PMC version */
	msg->length = 0x06;
	msg->buf[0] = 0x21;
	msg->buf[1] = 0xFF;
	msg->buf[2] = 0xFF;
	msg->buf[3] = 0xFF;
	msg->buf[4] = 0xFF;
	msg->buf[5] = 0x83;
	msg->buf[6] = 0x4A;
	msg->addChecksum = veFalse;
	msg->callback = callback;
	msg->handler = bmsHandler;
	msg->retries = 2;

	logI("aux", "check BMS presence");
	mk2FhQueueFrame(msg);
}
