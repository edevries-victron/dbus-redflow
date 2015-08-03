#ifndef _VELIB_PLATFORM_SERIAL_VECAN_H_
#define _VELIB_PLATFORM_SERIAL_VECAN_H_

#ifndef _VELIB_PLATFORM_SERIAL_H_
#error include velib/platform/serial.h instead.
#endif

#include <velib/velib_config.h>

#if CFG_WITH_VECAN_SERIAL

#include <velib/io/ve_remote_device_vecan.h>

typedef struct
{
	VeCanRemoteDev canRemoteDev;
	VeRemoteDev *remoteDev;
} SerialPortVeCan;

veBool veSerialVeCanOpen(struct VeSerialPortS* port);
veBool veSerialVeCanCreate(struct VeSerialPortS* port);
veBool veSerialVeCanCreateTunnel(struct VeSerialPortS* port, un8 id, void *ctx);

void veMsgRouterUpdate(void); // FIXME!
#else

typedef int SerialPortVeCan;

#endif
#endif
