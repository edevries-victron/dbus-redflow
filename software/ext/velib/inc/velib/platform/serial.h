#ifndef _VELIB_PLATFORM_SERIAL_H_
#define _VELIB_PLATFORM_SERIAL_H_

#include <velib/base/types.h>

struct VeSerialPortS;

#include <velib/platform/serial_dbus.h>
#include <velib/platform/serial_posix.h>
#include <velib/platform/serial_vecan.h>
#include <velib/platform/serial_win.h>
#include <velib/types/variant.h>

typedef enum {
	VE_SERIAL_EV_ERROR,
	VE_SERIAL_EV_OPENED
} VeSerialEvent;

typedef void veSerialRxCb(struct VeSerialPortS* port, un8 const *buf, un32 len);
typedef void veSerialEventCb(struct VeSerialPortS* port, VeSerialEvent event, char const *desc);

typedef struct VeSerialPortS
{
	int fh;
	char const* dev; /* com port */
	veSerialRxCb* rxCallback;
	veSerialEventCb* eventCallback;

	un32 baudrate;
	un8 intLevel;

	/* "private" */
	veBool (*close)(struct VeSerialPortS* port);
	veBool (*putByte)(struct VeSerialPortS* port, un8 byte);
	un32 (*putBuf)(struct VeSerialPortS* port, un8 const* buf, un32 len);
	veBool (*setBaud)(struct VeSerialPortS* port, un32 baudrate);
	veBool (*setBreak)(struct VeSerialPortS* port);
	veBool (*clearBreak)(struct VeSerialPortS* port);
	VeVariant *(*getDeviceNumber)(struct VeSerialPortS* port, VeVariant *var);
	veBool (*setEol)(struct VeSerialPortS* port, sn16 eol);

	union {
		SerialPortDbus dbus;
		posixSerialPort posix;
		SerialPortVeCan vecan;
		SerialPortWin win;
	} nt; /* native */

	void* ctx;
	void* event;
} VeSerialPort;

veBool	veSerialOpen(VeSerialPort* port, void* ctx);
veBool	veSerialClose(VeSerialPort* port);
veBool	veSerialPutByte(VeSerialPort* port, un8 byte);
un32	veSerialPutBuf(VeSerialPort* port, un8 const* buf, un32 len);
veBool	veSerialSetBaud(VeSerialPort* port, un32 baudrate);
veBool	veSerialSetEol(VeSerialPort* port, sn16 eol);
veBool	veSerialBreak(VeSerialPort* port);
veBool	veSerialBreakClear(VeSerialPort* port);
veBool	veSerialRxCallback(VeSerialPort* port, veSerialRxCb* callback);
veBool	veSerialEventCallback(VeSerialPort *port, veSerialEventCb *callback);
VeVariant *veSerialGetDeviceNumber(VeSerialPort* port, VeVariant *var);

veBool veSerialCreate(VeSerialPort* port, void* ctx);

#endif
