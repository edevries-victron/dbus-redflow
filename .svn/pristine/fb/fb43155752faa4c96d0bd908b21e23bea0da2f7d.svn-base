#include <string.h> // memcpy

#include <velib/io/ve_stream_n2k.h>
#include <velib/io/ve_remote_device.h>
#include <velib/io/ve_remote_device_vecan.h>
#include <velib/mk2/mk2_remote_device.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>
#include <velib/vecan/reg_rx.h>
#include <velib/vecan/regs.h>

static VeRemoteDev *mk2Dev;
static un8 timeout;

static void resetTimeout(void)
{
	timeout = 5 * 20;
}

static void mk2TakeOverRequestResponse(VeRegAckCode code)
{
	veRemoteDevOpenResponse(mk2Dev, code);
	resetTimeout();
}

static void onRxData(un8 const *buf, VeBufSize len)
{
	Mk2MsgTx *msg;

	if (len > (MK2_MAX_MESSAGE_LENGTH + 2) ) {
		logE("mk2-fw", "frame too long");
		return;
	}

	msg = mk2TxAlloc();
	if (!msg) {
		logE("mk2-fw", "no tx messages left");
		return;
	}

	memcpy(&msg->length, buf, len);
	msg->addChecksum = veFalse;
	msg->noDelay = veTrue;
	msg->retries = 0;

	mk2FhQueueFrame(msg);
	resetTimeout();
}

/** Handles incoming requests for the mk2 as character device */
static void deviceEventHandler(struct VeRemoteDev *dev, VeRemoteDevEv ev, VeVariant *arg,
						un8 const *buf, un32 len)
{
	VE_UNUSED(arg);

	switch (ev) {
	case VE_REMOTE_DEV_EV_ERROR:
		veRemoteDevClose(dev);
		break;
	case VE_REMOTE_DEV_EV_OPEN:
		mk2TakeOverRequest(mk2TakeOverRequestResponse);
		break;
	case VE_REMOTE_DEV_EV_DATA_TO_DEV:
		onRxData(buf, len);
		break;
	case VE_REMOTE_DEV_EV_CLOSE:
		mk2NoResponseCallback();
		break;
	default:
		break;
	}
}

void mk2RemoteDevTick(void)
{
	if (mk2Dev->state != VE_REMOTE_DEV_STATE_OPENED)
		return;

	if (timeout && --timeout == 0) {
		veRemoteDevCloseResponse(mk2Dev);
		mk2NoResponseCallback();
	}
}

/** hook to forward mk2 messages when opened */
void mk2RemoteDevForwardFrame(Mk2MsgRx *mk2msg)
{
	if (mk2Dev)
		veRemoteDevDataFromDevice(mk2Dev, &mk2msg->length, mk2msg->length + 2);
}

void mk2RemoteDevInit(VeRemoteDev *tunnel)
{
	veAssert(mk2Dev == NULL);
	mk2Dev = tunnel;
	veRemoteDevSetCallback(tunnel, deviceEventHandler, NULL);
}
