#include <velib/velib_inc_J1939.h>

#include <velib/base/types.h>
#include <velib/base/ve_string.h>
#include <velib/io/ve_remote_device_vecan.h>
#include <velib/J1939/J1939_device.h>
#include <velib/platform/console.h>
#include <velib/platform/plt.h>
#include <velib/platform/serial.h>
#include <velib/platform/serial_vecan.h>
#include <velib/utils/ve_logger.h>

#include "serial_internal.h"

static un32 sPutBufFromDev(VeSerialPort* port, un8 const* buf, un32 len)
{
	return veRemoteDevDataFromDevice(port->nt.vecan.remoteDev, buf, len);
}

static un32 sPutBufToDev(VeSerialPort* port, un8 const* buf, un32 len)
{
	return veRemoteDevDataToDevice(port->nt.vecan.remoteDev, buf, len);
}

static void tunnelEvent(struct VeRemoteDev *tunnel, VeRemoteDevEv ev, VeVariant *arg,
						un8 const *buf, un32 len)
{
	VeSerialPort* port = (VeSerialPort *) tunnel->appCtx;
	VE_UNUSED(arg);

	switch (ev) {
	case VE_REMOTE_DEV_EV_OPEN:
		veRemoteDevOpenResponse(tunnel, VACK_OK);
		break;
	case VE_REMOTE_DEV_EV_ERROR:
		veSerialHandleError(port, "error");
		break;
	case VE_REMOTE_DEV_EV_OPENED:
		veSerialHandleEvent(port, VE_SERIAL_EV_OPENED, NULL);
		break;
	case VE_REMOTE_DEV_EV_DATA_FROM_DEV:
		veSerialHandleData(port, buf, len);
		break;
	default:
		break;
	}
}

veBool veSerialVeCanOpen(VeSerialPort* port)
{
	int nad;
	int id;
	static un8 nadStatic;

	if (sscanf(port->dev, "vecan://%X/%X", &nad, &id ) < 2) {
		logE("dbus-serial", "invalid dbus path %s", port->dev);
		return veFalse;
	}

	/* This is a pointer since address changes _could_ be monitored, but not done... */
	nadStatic = nad;
	veCanRemoteDevInit(&port->nt.vecan.canRemoteDev, &nadStatic, id, VE_REMOTE_DEV_ROLE_CLIENT);
	port->nt.vecan.remoteDev = &port->nt.vecan.canRemoteDev.remoteDev;
	veRemoteDevSetCallback(port->nt.vecan.remoteDev, tunnelEvent, port);
	veRemoteDevOpen(port->nt.vecan.remoteDev);

	port->putBuf = sPutBufToDev;

	return veTrue;
}

// blocking version, Canonical Input Processing
veBool veSerialVeCanCreate(VeSerialPort* port)
{
	int nad;
	int id;

	if (sscanf(port->dev, "vecan://%X/%X", &nad, &id ) < 2) {
		logE("vecan-serial", "invalid path %s", port->dev);
		return veFalse;
	}

	if (nad != j1939Device.nad) {
		logE("vecan-serial", "out nad is %hhX not %hhX", j1939Device.nad, nad);
		return veFalse;
	}

	veCanRemoteDevInit(&port->nt.vecan.canRemoteDev, NULL, id, VE_REMOTE_DEV_ROLE_SERVICE);
	port->nt.vecan.remoteDev = &port->nt.vecan.canRemoteDev.remoteDev;
	veRemoteDevSetCallback(port->nt.vecan.remoteDev, tunnelEvent, port);

	port->putBuf = sPutBufFromDev;

	return veTrue;
}

