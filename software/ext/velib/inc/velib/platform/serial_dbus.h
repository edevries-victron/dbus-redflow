#ifndef _VELIB_PLATFORM_SERIAL_DBUS_H_
#define _VELIB_PLATFORM_SERIAL_DBUS_H_

#ifndef _VELIB_PLATFORM_SERIAL_H_
#error include velib/platform/serial.h instead.
#endif

#include <velib/velib_config.h>

#if CFG_WITH_DBUS_SERIAL

#include <velib/io/ve_remote_device_item.h>
#include <velib/types/ve_item.h>
#include <velib/types/ve_item_def.h>

typedef struct
{
    VeItem service;
    VeItemRemoteDev dbusRemoteDev;
    VeRemoteDev *remoteDev;
} SerialPortDbus;

veBool veSerialDbusOpen(struct VeSerialPortS* port);
veBool veSerialDbusCreate(struct VeSerialPortS* port);
veBool veSerialDbusCreateItem(struct VeSerialPortS *port, VeItem *item, void *ctx);
#else

typedef int SerialPortDbus;

#endif
#endif
