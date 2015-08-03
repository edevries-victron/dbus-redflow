#include <velib/base/ve_string.h>
#include <velib/mk2/hal.h>
#include <velib/mk2/mk2_update.h>
#include <velib/platform/serial.h>
#include <velib/utils/ve_logger.h>

#include <string.h>

VeSerialPort mk2port;

void mk2HalInit(void)
{
}

void mk2RxByteStub(struct VeSerialPortS* port, un8 const *buf, un32 len)
{
	VE_UNUSED(port);

	while (len--)
		mk2RxByte(*buf++);
}

#if CFG_WITH_MK2_UPDATE
void mk2RxByteUpdateStub(struct VeSerialPortS* port, un8 const *buf, un32 len)
{
	VE_UNUSED(port);

	while (len--)
		mk2RxUpdateRxByte(*buf++);
}
#endif

veBool mk2Open(char const* dev)
{
	memset(&mk2port, 0, sizeof(mk2port));
	mk2port.dev = strdup(dev);
	mk2port.baudrate = 2400;
	mk2port.rxCallback = mk2RxByteStub;
	mk2port.intLevel = 0;
	return veSerialOpen(&mk2port, NULL);
}

veBool mk2Close(void)
{
	return veSerialClose(&mk2port);
}

void mk2TxBuf(un8 const* buf, un8 len)
{
	veSerialPutBuf(&mk2port, buf, len);
	mk2DumpBuf(">", buf, len);
	mk2FhTxDone();
}

void mk2TxByte(un8 byte)
{
	if (!veSerialPutByte(&mk2port, byte))
	{
		logE("mk2 serial", "cant write to port");
	}
	mk2FhTxDone();
}

void mk2SetBaudRate(Mk2BaudRate rate)
{
	switch (rate)
	{
	default:
	case MK2_BAUD_2400:
		veSerialSetBaud(&mk2port, 2400);
		break;
	case MK2_BAUD_19200:
		veSerialSetBaud(&mk2port, 19200);
		break;
	case MK2_BAUD_115200:
		veSerialSetBaud(&mk2port, 115200);
		break;
	}
}

void mk2EnableUpdate(veBool update)
{
#if CFG_WITH_MK2_UPDATE
	mk2port.rxCallback = (update ? mk2RxByteUpdateStub : mk2RxByteStub);
#else
	VE_UNUSED(update);
	mk2port.rxCallback = mk2RxByteStub;
#endif
}

void mk2Break(void)
{
	veSerialBreak(&mk2port);
}

void mk2BreakClear(void)
{
	veSerialBreakClear(&mk2port);
}

VeVariant *mk2Instance(VeVariant *var)
{
	return veSerialGetDeviceNumber(&mk2port, var);
}
