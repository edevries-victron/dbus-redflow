#include <velib/io/ve_remote_device.h>
#include <velib/types/variant.h>
#include <velib/utils/ve_assert.h>

#include "ve_remote_device_internal.h"

void veRemoteDevInit(VeRemoteDev *tunnel, VeRemoteDevPrepareOutputCb *getOutput,
					 VeRemoteDevSendOutputCb *sendOutput, VeRemoteDevRole role)
{
	tunnel->state = VE_REMOTE_DEV_STATE_CLOSED;
	tunnel->prepareOutStream = getOutput;
	tunnel->sendOutStream = sendOutput;
	tunnel->role = role;
}

static void veOutTunnelOpcodeBuf(VeRemoteDevEv ev, un8 const *buf, VeBufSize len)
{
	veOutUn8(ev);
	veOutUn8(len);
	if (len)
		veOutBuf(buf, (un8) len);
}

static void veOutDataFromDevice(un8 const *buf, VeBufSize len)
{
	veOutTunnelOpcodeBuf(VE_REMOTE_DEV_EV_DATA_FROM_DEV, buf, len);
}

static void veOutDataToDevice(un8 const *buf, VeBufSize len)
{
	veOutTunnelOpcodeBuf(VE_REMOTE_DEV_EV_DATA_TO_DEV, buf, len);
}

static void veOutTunnelOpcodeNoArgs(VeRemoteDevEv ev)
{
	veOutTunnelOpcodeBuf(ev, NULL, 0);
}

static void veOutTunnelAck(VeRemoteDevEv ev, VeRegAckCode code)
{
	veOutUn8(ev);
	veOutUn8(2);
	veOutUn16Le(code);
}


/* Client: attempt to open the device */
veBool veRemoteDevOpen(VeRemoteDev *dev)
{
	veAssert(dev->role == VE_REMOTE_DEV_ROLE_CLIENT && dev->state == VE_REMOTE_DEV_STATE_CLOSED);

	if (!dev->prepareOutStream(dev))
		return veFalse;

	veOutTunnelOpcodeNoArgs(VE_REMOTE_DEV_EV_OPEN);
	dev->sendOutStream(dev);

	return veTrue;
}

/** Called to confirm the port is now open */
veBool veRemoteDevOpenResponse(VeRemoteDev *dev, VeRegAckCode code)
{
	veAssert(dev->role == VE_REMOTE_DEV_ROLE_SERVICE && dev->state == VE_REMOTE_DEV_STATE_OPENING);

	if (!dev->prepareOutStream(dev))
		return veFalse;

	if (code == VACK_OK) {
		dev->state = VE_REMOTE_DEV_STATE_OPENED;
		veOutTunnelOpcodeNoArgs(VE_REMOTE_DEV_EV_OPENED);
	} else {
		dev->state = VE_REMOTE_DEV_STATE_CLOSED;
		veOutTunnelAck(VE_REMOTE_DEV_EV_ERROR, code);
	}

	return dev->sendOutStream(dev);
}

/* Called by the client to close a device */
void veRemoteDevClose(VeRemoteDev *dev)
{
	veAssert(dev->role == VE_REMOTE_DEV_ROLE_CLIENT);

	if (dev->state == VE_REMOTE_DEV_STATE_CLOSED)
		return;

	if (!dev->prepareOutStream(dev))
		return;
	veOutTunnelOpcodeNoArgs(VE_REMOTE_DEV_EV_CLOSE);
	dev->sendOutStream(dev);
}

/* Closes / acks a close of a provided device */
veBool veRemoteDevCloseResponse(VeRemoteDev *dev)
{
	veAssert(dev->role == VE_REMOTE_DEV_ROLE_SERVICE);

	if (!dev->prepareOutStream(dev))
		return veFalse;

	dev->state = VE_REMOTE_DEV_STATE_CLOSED;
	veOutTunnelOpcodeNoArgs(VE_REMOTE_DEV_EV_CLOSED);
	return dev->sendOutStream(dev);
}


/** Send any command to the other side of the tunnel */
static un32 veTunnelPutRaw(VeRemoteDev *dev, VeRemoteDevEv ev, un8 const *buf, un32 len)
{
	if (!dev->prepareOutStream(dev))
		return 0;
	veOutTunnelOpcodeBuf(ev, buf, len);

	return dev->sendOutStream(dev) ? len : 0;
}

/** sends data towards the client */
un32 veRemoteDevDataFromDevice(VeRemoteDev *dev, un8 const *buf, un32 len)
{
	if (dev->state != VE_REMOTE_DEV_STATE_OPENED)
		return 0;

	if (!dev->prepareOutStream(dev))
		return 0;
	veOutDataFromDevice(buf, len);

	return dev->sendOutStream(dev) ? len : 0;
}

/** sends data towards the device */
un32 veRemoteDevDataToDevice(VeRemoteDev *dev, un8 const *buf, un32 len)
{
	if (dev->state != VE_REMOTE_DEV_STATE_OPENED)
		return 0;

	if (!dev->prepareOutStream(dev))
		return 0;
	veOutDataToDevice(buf, len);

	return dev->sendOutStream(dev) ? len : 0;
}

/** Sets a callback where the received data should finally be send to */
void veRemoteDevSetCallback(VeRemoteDev *dev, VeRemoteDevCb *callback, void *ctx)
{
	dev->appCb = callback;
	dev->appCtx = ctx;
}

static void setState(VeRemoteDev *dev, VeRemoteDevState state)
{
	if (dev->role == VE_REMOTE_DEV_ROLE_CLIENT_PROXY || dev->role == VE_REMOTE_DEV_ROLE_SERVICE_PROXY)
		return;
	dev->state = state;
}

/** called to forward commands towards the device  */
void veRemoteDevRxData(VeRemoteDev *dev)
{
	while (veInBytesLeft())
	{
		un8 opcode = veInUn8();
		un8 len = veInUn8();
		VeVariant arg;
		un8 const *buf = veInPtr();
		VeBufSize index = veStreamIn->index;

		arg.type.tp = VE_UNKNOWN;

		switch (opcode) {
		case VE_REMOTE_DEV_EV_OPEN:
			/*
			 * FIXME: the error should actually be addresses specifically to the device
			 * trying to reopen an existing port, not broadcasted.
			 */
			if (dev->role == VE_REMOTE_DEV_ROLE_SERVICE && dev->state != VE_REMOTE_DEV_STATE_CLOSED) {
				veOutTunnelAck(VE_REMOTE_DEV_EV_ERROR, VACK_ERR_BUSY);
				return;
			}
			setState(dev, VE_REMOTE_DEV_STATE_OPENING);
			break;

		case VE_REMOTE_DEV_EV_OPENED:
			setState(dev, VE_REMOTE_DEV_STATE_OPENED);
			break;

		case VE_REMOTE_DEV_EV_CLOSED:
			setState(dev, VE_REMOTE_DEV_STATE_CLOSED);
			break;

		case VE_REMOTE_DEV_EV_ERROR:
			arg.value.UN16 = veInUn16Le();
			arg.type.tp = VE_UN16;
			break;

		default:
			break;
		}

		if (arg.type.tp == VE_UNKNOWN) {
			arg.value.CPtr = veInFixedString(len);
			arg.type.tp = VE_BUF;
			arg.type.len = len;
		} else {
			/*
			 * Note: additional data might be added, so make sure len bytes are
			 * skipped, not only the bytes known to us.
			 */
			veInMove(index + len);
		}

		if (dev->appCb)
			dev->appCb(dev, opcode, &arg, buf, len);

		if (veStreamIn->error)
			return;
	}
}

/* tunnel support */

static void devEventHandler(struct VeRemoteDev *dev, VeRemoteDevEv ev, VeVariant *arg,
						un8 const *buf, un32 len)
{
	VeRemoteDev *otherDevice = (VeRemoteDev *) dev->appCtx;
	VE_UNUSED(arg);

#if 0
	switch (ev) {
	case VE_REMOTE_DEV_EV_ERROR:
		logI("tunnel", "forwarding open request");
		break;
	case VE_REMOTE_DEV_EV_OPEN:
		logI("tunnel", "forwarding open request");
		break;
	case VE_REMOTE_DEV_EV_OPENED:
		logI("tunnel", "opened");
		break;
	case VE_REMOTE_DEV_EV_DATA_TO_DEV:
		logI("", "data forward");
		break;
	case VE_REMOTE_DEV_EV_CLOSE:
		logI("tunnel", "forwarding close request");
		break;
	default:
		break;
	}
#endif

	veTunnelPutRaw(otherDevice, ev, buf, len);
}

/* Forwards all data from service to client and vice versa */
void veRemoteDeviceCreateTunnel(VeRemoteDev *service, VeRemoteDev *client)
{
	service->state = VE_REMOTE_DEV_STATE_NONE;
	client->state = VE_REMOTE_DEV_STATE_NONE;
	service->role |= VE_REMOTE_DEV_ROLE_PROXY;
	client->role |= VE_REMOTE_DEV_ROLE_PROXY;

	veRemoteDevSetCallback(service, devEventHandler, client);
	veRemoteDevSetCallback(client, devEventHandler, service);
}
