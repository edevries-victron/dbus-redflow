#include <string.h>

#include <velib/platform/serial.h>
#include <velib/platform/plt.h>
#include <velib/utils/ve_logger.h>

#include "serial_internal.h"

veBool veSerialOpen(VeSerialPort* port, void* ctx)
{
	port->ctx = ctx;

#if CFG_WITH_VECAN_SERIAL
	if (strncmp(port->dev, "vecan://", strlen("vecan://")) == 0)
		return veSerialVeCanOpen(port);
#endif

#if CFG_WITH_DBUS_SERIAL
	if (strncmp(port->dev, "dbus://", strlen("dbus://")) == 0)
		return veSerialDbusOpen(port);
#endif

#if defined(_UNIX_)
	return veSerialPosixOpen(port);
#elif defined(_WIN32)
	return veSerialWinOpen(port);
#else
	return veFalse;
#endif
}

veBool veSerialClose(VeSerialPort* port)
{
	if (!port->close)
		return veFalse;

	return port->close(port);
}

veBool veSerialPutByte(VeSerialPort* port, un8 byte)
{
	if (!port->putByte)
		return veFalse;

	return port->putByte(port, byte);
}

un32 veSerialPutBuf(VeSerialPort* port, un8 const* buf, un32 len)
{
	if (!port->putBuf)
		return veFalse;

	return port->putBuf(port, buf, len);
}

veBool veSerialSetBaud(VeSerialPort* port, un32 baudrate)
{
	if (!port->setBaud)
		return veFalse;
	return port->setBaud(port, baudrate);
}

veBool veSerialBreak(VeSerialPort* port)
{
	if (!port->setBreak)
		return veFalse;
	return port->setBreak(port);
}

veBool veSerialBreakClear(VeSerialPort* port)
{
	if (!port->clearBreak)
		return veFalse;
	return port->clearBreak(port);
}

veBool veSerialSetEol(VeSerialPort *port, sn16 eol)
{
	if (!port->setEol)
		return veFalse;

	return port->setEol(port, eol);
}

veBool veSerialRxCallback(VeSerialPort* port, veSerialRxCb* callback)
{
	pltInterruptSuspend();
	port->rxCallback = callback;
	pltInterruptResume();
	return veTrue;
}

veBool veSerialEventCallback(VeSerialPort *port, veSerialEventCb *callback)
{
	pltInterruptSuspend();
	port->eventCallback = callback;
	pltInterruptResume();
	return veTrue;
}

VeVariant *veSerialGetDeviceNumber(VeSerialPort* port, VeVariant *var)
{
	if (!port->getDeviceNumber) {
		var->type.tp = VE_UNKNOWN;
		return var;
	}
	return port->getDeviceNumber(port, var);
}

/* internal */
void veSerialHandleEvent(struct VeSerialPortS *port, VeSerialEvent event, char const *descr)
{
	if (port->eventCallback)
		port->eventCallback(port, event, descr);
}

void veSerialHandleError(struct VeSerialPortS *port, char const *descr)
{
	logE("serial", "%s", descr);
	veSerialHandleEvent(port, VE_SERIAL_EV_ERROR, descr);
}

veBool veSerialHandleData(struct VeSerialPortS *port, un8 const *buf, sn32 len)
{
	if (len < 0) {
		veSerialHandleError(port, "serial read failure");
		return veFalse;
	}

	if (len == 0) {
		veSerialHandleError(port, "Ready for reading but read 0 bytes. Device removed?");
		return veFalse;
	}

	if (!port->rxCallback)
		return veTrue;

	if (port->intLevel == 0)
	{
		port->rxCallback(port, buf, len);
		return veTrue;
	}

#if CFG_NO_PREEMPTION
	veSerialHandleError(port, "serial thread support is not compiled in");
#else
	if (port->intLevel == 1) {
		sn32 n;

		for (n = 0; n < len; n++) {
			pltInterruptBegins(port->intLevel);
			port->rxCallback(port, &buf[n], 1);
			pltInterruptEnds(port->intLevel);
		}
	} else {
		pltInterruptBegins(port->intLevel);
		port->rxCallback(port, buf, len);
		pltInterruptEnds(port->intLevel);
	}
#endif

	return veTrue;
}

veBool veSerialCreate(VeSerialPort* port, void* ctx)
{
	port->ctx = ctx;

#if CFG_WITH_VECAN_SERIAL
	if (strncmp(port->dev, "vecan://", strlen("vecan://")) == 0)
		return veSerialVeCanCreate(port);
#endif

#if CFG_WITH_DBUS_SERIAL
	if (strncmp(port->dev, "dbus://", strlen("dbus://")) == 0)
		return veSerialDbusCreate(port);
#endif

#if defined(_UNIX_)
	return veSerialPosixOpen(port);
#endif

	return veFalse;
}
