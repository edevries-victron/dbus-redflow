#include <stdio.h>

#include <velib/base/types.h>
#include <velib/io/ve_remote_device.h>
#include <velib/platform/console.h>
#include <velib/platform/plt.h>
#include <velib/platform/serial.h>
#include <velib/platform/serial_dbus.h>
#include <velib/types/ve_dbus_item.h>
#include <velib/types/ve_values.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>

#include "serial_internal.h"

static veBool sClose(VeSerialPort *port)
{
	veRemoteDevClose(port->nt.dbus.remoteDev);
	return veTrue;
}

static un32 sPutBuf(VeSerialPort *port, un8 const *buf, un32 len)
{
	return veRemoteDevDataToDevice(port->nt.dbus.remoteDev, buf, len);
}

static veBool sPutByte(VeSerialPort *port, un8 byte)
{
	return sPutBuf(port, &byte, 1) == 1;
}

static veBool sSetBaud(VeSerialPort *port, un32 baudrate)
{
	VE_UNUSED(port);
	VE_UNUSED(baudrate);

	return veTrue;
}

static void onTunnelEvent(struct VeRemoteDev *tunnel, VeRemoteDevEv ev, VeVariant *arg,
				   un8 const *buf, un32 len)
{
	VeSerialPort *port = (VeSerialPort *) tunnel->appCtx;
	VE_UNUSED(arg);

	switch (ev) {
		break;
	case VE_REMOTE_DEV_EV_OPENED:
		logI("dbus-serial", "port opened");
		break;
	case VE_REMOTE_DEV_EV_DATA_FROM_DEV:
		if (len)
			veSerialHandleData(port, buf, len);
		break;
	default:
		logI("dbus-serial", "no idea %d", ev);
	}
}

veBool veSerialDbusOpen(VeSerialPort *port)
{
	struct VeDbus *conn;
	char service[100];
	char path[100];
	VeItem *item;

	port->close = sClose;
	port->putByte = sPutByte;
	port->putBuf = sPutBuf;
	port->setBaud = sSetBaud;

	if (sscanf(port->dev, "dbus://%99[^/]/%99s", service, path) < 2) {
		logE("dbus-serial", "invalid dbus path %s", port->dev);
		return veFalse;
	}

	/* Get the default dbus connection */
	if (! (conn = veDbusGetDefaultBus()) ) {
		logE("dbus-serial", "dbus connection failed\n");
		return veFalse;
	}

	/* And use that one to listen to the dbus */
	veDbusSetListeningDbus(conn);

	/* check it the remote party is present */
	if (!veDbusAddRemoteService(service, &port->nt.dbus.service, veTrue)) {
		logE("dbus-serial", "veDbusAddRemoteService failed");
		return veFalse;
	}

	/* Create dbus item */
	item = veItemGetOrCreateUid(&port->nt.dbus.service, path);
	if (item == NULL) {
		logE("dbus-serial", "creating tunnel failed failed");
		return veFalse;
	}

	/* Setup the receiving side */
	veItemRemoteDevInit(&port->nt.dbus.dbusRemoteDev, item, VE_REMOTE_DEV_ROLE_CLIENT);
	port->nt.dbus.remoteDev = &port->nt.dbus.dbusRemoteDev.remoteDev;
	veRemoteDevSetCallback(port->nt.dbus.remoteDev, onTunnelEvent, port);
	veRemoteDevOpen(port->nt.dbus.remoteDev);

	return veTrue;
}

veBool veSerialDbusCreate(struct VeSerialPortS *port)
{
	struct VeDbus *conn;
	char service[100];
	char path[100];
	struct VeItem *root = veValueTree();
	VeItem *item;

	port->putBuf = sPutBuf;

	if (sscanf(port->dev, "dbus://%99[^/]/%99s", service, path) < 2) {
		logE("dbus-serial", "invalid dbus path %s", port->dev);
		return veFalse;
	}

	/* Get the default dbus connection */
	if (! (conn = veDbusGetDefaultBus()) ) {
		logE("dbus-serial", "dbus connection failed\n");
		return veFalse;
	}

	/* Create the variable to export */
	item = veItemGetOrCreateUid(root, path);
	if (item == NULL) {
		logE("dbus-serial", "creating tunnel failed failed");
		return veFalse;
	}

	port->nt.dbus.remoteDev = &port->nt.dbus.dbusRemoteDev.remoteDev;
	veItemRemoteDevInit(&port->nt.dbus.dbusRemoteDev, item, VE_REMOTE_DEV_ROLE_SERVICE);

	/* make the values also available on the dbus and get a service name */
	veDbusItemInit(conn, root);

	if (!veDbusChangeName(conn, service)) {
		logE("dbus_service", "registering name failed\n");
		return veFalse;
	}

	return veTrue;
}
