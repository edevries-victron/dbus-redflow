/**
 * TODO GPL header
 */

#include <velib/base/types.h>
#include <velib/base/ve_string.h>
#include <velib/canhw/canhw.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>
#include <velib/platform/plt.h>
#include <velib/canhw/canhw_driver.h>
#include <canal.h>						/* Canal driver */

#ifdef WIN32
#include <setupapi.h>					/* Device installation */
#include <initguid.h>					/* GUID stuff */

#pragma comment(lib, "usb2can.lib")
#pragma comment(lib, "setupapi.lib")

DEFINE_GUID(GUID_CLASS_CANUSB2, 0x3ea03b52, 0xc71a, 0x433c, 0xa6, 
			0x26, 0xe8, 0xad, 0x96, 0xf2, 0x57, 0x8b);

#define IOCTL_INDEX								0x0000

#define IOCTL_CANUSB2_GET_CONFIG_DESCRIPTOR		\
	CTL_CODE(FILE_DEVICE_UNKNOWN,				\
	IOCTL_INDEX,								\
	METHOD_BUFFERED,							\
	FILE_ANY_ACCESS)

#define IOCTL_CANUSB2_RESET_DEVICE				\
	CTL_CODE(FILE_DEVICE_UNKNOWN,				\
	IOCTL_INDEX + 1,							\
	METHOD_BUFFERED,							\
	FILE_ANY_ACCESS)

#define IOCTL_CANUSB2_RESET_PIPE				\
	CTL_CODE(FILE_DEVICE_UNKNOWN,				\
	IOCTL_INDEX + 2,							\
	METHOD_BUFFERED,							\
	FILE_ANY_ACCESS)

#define IOCTL_CANUSB2_GET_SERIALNUMBER_STRING	\
	CTL_CODE(FILE_DEVICE_UNKNOWN,				\
	IOCTL_INDEX + 3,							\
	METHOD_BUFFERED,							\
	FILE_ANY_ACCESS)

#endif

#define MODULE		"CANHW.CANUSB2"

static VeCanDriver canDriver;

/* canusb2 specific gateway variables */
typedef struct
{
	long	handle;
	char	serial[16];
	un16	kbit;
	un16	ch;
	un32	lastTime;
	un32	overflows;
} GatewayPrivate;

/* stub, opening is postponed till busOn since bitrate is needed. */
static veBool open(VeCanGateway *gw)
{
	return veTrue;
}

static veBool setBitrate(VeCanGateway *gw, un16 kbit)
{
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;

	/* Apply in busOn method */
	priv->kbit = kbit;

	return veTrue;
}

/* actually open the gateway */
static veBool busOn(VeCanGateway *gw)
{
	GatewayPrivate *priv;
	char			config[20];

	priv = (GatewayPrivate *) gw->ctx;

	sprintf(config, "%s ; %u", priv->serial, priv->kbit);

	if ((priv->handle = CanalOpen(config, 0)) != CANAL_ERROR_SUCCESS)
		return veTrue;

	return veFalse;
}

/* reading a raw CAN bus message */
static veBool readMsg(struct VeCanGatewayS *gw, VeRawCanMsg *rawCanMsg)
{
	GatewayPrivate *priv;
	canalMsg		msg;
	int				status;

	priv = (GatewayPrivate *) gw->ctx;

	for(;;)
	{
		status = CanalReceive(priv->handle, &msg);

		switch (status)
		{
		/* Message received */
		case CANAL_ERROR_SUCCESS:

			/* Error message -> skip */
			if (msg.flags & CANAL_IDFLAG_STATUS) {
				veAssert(rawCanMsg->flags == 0);
				continue;
			}

#if CFG_CANHW_STAMP

			if (msg.timestamp < priv->lastTime)
				priv->overflows++;
			priv->lastTime = msg.timestamp;
			rawCanMsg->extra.time.us = (un64)msg.timestamp + priv->overflows * 0xFFFFFFFF;
			rawCanMsg->extra.time.us *= 1000;
			logI(MODULE, "time: %u", (un32)msg.timestamp);

#endif

			/* Flags */
			rawCanMsg->flags = 0;

			/* 11 or 29 bits */
			if (msg.flags & CANAL_IDFLAG_EXTENDED) {
				rawCanMsg->flags |= (msg.flags & CANAL_IDFLAG_EXTENDED) ? VE_CAN_EXT : VE_CAN_STD;
			}

			if (msg.flags & CANAL_IDFLAG_RTR) {
				rawCanMsg->flags |= VE_CAN_RTR;
			}

			rawCanMsg->canId = (VeCanId)msg.id;
			rawCanMsg->length = (VeCanLength)msg.sizeData;
			(void)memcpy(rawCanMsg->mdata, msg.data, 8);
			return veTrue;

		/* No message available */
		case CANAL_ERROR_FIFO_EMPTY:

			return veFalse;

		/* Error */
		default:

			logE(MODULE, "readMsg returned unknown can state (%d)", status);
			return veFalse;
		}
	}

	/* Should not be possible to get here */
	logE(MODULE, "WARNING: unable to read from: %d", priv->handle);
	return veFalse;
}

/* sending a raw CAN bus message */
static veBool sendMsg(struct VeCanGatewayS *gw, VeRawCanMsg *rawCanMsg)
{
	GatewayPrivate *priv;
	canalMsg		msg;

	priv = (GatewayPrivate *) gw->ctx;

	msg.flags = 0;
	if (rawCanMsg->flags & VE_CAN_EXT) {
		msg.flags |= CANAL_IDFLAG_EXTENDED;
	}

	if (rawCanMsg->flags & VE_CAN_RTR) {
		msg.flags |= CANAL_IDFLAG_RTR;
	}

	msg.id = rawCanMsg->canId;
	msg.sizeData = (unsigned char)rawCanMsg->length;
	(void)memcpy(msg.data, rawCanMsg->mdata, 8);

	return CanalSend(priv->handle, &msg) == CANAL_ERROR_SUCCESS;
}

static size_t gwId(VeCanGateway *gw, char *buf, size_t len)
{
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;

	return ve_snprintf(buf, len, "canusb2:%u", priv->ch);
}

static size_t gwDesc(VeCanGateway *gw, char *buf, size_t len)
{
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;

	return ve_snprintf(buf, len, "Victron Energy CANUSB2 [%s]", priv->serial);
}

/* Get memory and setup a new lawicel gateway */
static VeCanGateway *allocGateway(const char *serial)
{
	static int ch = 0;

	VeCanGateway *	gw = NULL;
	GatewayPrivate *priv;

	gw = calloc(1, sizeof(VeCanGateway));
	priv = calloc(1, sizeof(GatewayPrivate));

	strcpy(priv->serial, serial);
	priv->ch = ch++;

	gw->ctx = priv;

	gw->flags = VE_CAN_GW_CAN;

	gw->openFun = open;
	gw->bitrateFun = setBitrate;
	gw->busOnFun = busOn;
	gw->readStub.fun = readMsg;
	gw->sendStub.fun = sendMsg;
	gw->idFun = gwId;
	gw->descFun = gwDesc;

	return gw;
}

#ifdef WIN32
static int getSerial(TCHAR *devPath, char *buf, int len)
{
	HANDLE	h;
	ULONG	read;
	BOOL	result;

	/* Open device */
	h = CreateFile(devPath, GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (h == INVALID_HANDLE_VALUE)
		return veFalse;

	/* Read serial */
	result = DeviceIoControl(h, IOCTL_CANUSB2_GET_SERIALNUMBER_STRING, 
		0, 0, buf, len, &read, NULL);

	/* Cleanup */
	CloseHandle(h);

	return result;
}

static void enumerate(void)
{
	VeCanGateway *				gw;
	un16						devIdx;
	HDEVINFO					devs;
	SP_DEVICE_INTERFACE_DATA	dev;

	/* Get set of CANUSB2 devices */
	devs = SetupDiGetClassDevs(&GUID_CLASS_CANUSB2, NULL,
		NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	/* No gw's found */
	if (devs == INVALID_HANDLE_VALUE) {
		return;
	}

	dev.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	for (devIdx = 0; SetupDiEnumDeviceInterfaces(devs, 0, &GUID_CLASS_CANUSB2, devIdx, &dev); devIdx++)
	{
		PSP_DEVICE_INTERFACE_DETAIL_DATA	devData;
		SP_DEVINFO_DATA						devInfo;
		ULONG								len;
		char								serial[100];

		/* Get data length and allocate */
		SetupDiGetDeviceInterfaceDetail(devs, &dev, NULL, 0, &len, NULL);
		if ((devData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(len)) == NULL)
			continue;

		devData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		devInfo.cbSize = sizeof (SP_DEVINFO_DATA);
		if (!SetupDiGetDeviceInterfaceDetail(devs, &dev, devData, len, &len, &devInfo)) {
			free(devData);
			continue;
		}

		/* Get serial from device */
		if (!getSerial(devData->DevicePath, serial, sizeof(serial))) {
			free(devData);
			continue;
		}

#ifdef _UNICODE
		/* Make single byte */
		wcstombs(serial, (wchar_t *)serial, wcslen((wchar_t *)serial));
#endif

		/* usb2can 8 char serial */
		if (strstr(serial, "ED") == serial)
			serial[8] = 0;

		/* canusb2 11 char serial */
		if (strstr(serial, "HQ") == serial)
			serial[11] = 0;

		if ((gw = allocGateway(serial)))
			veCanDrvNewGw(&canDriver, gw);

		free(devData);
	}

	/* Cleanup */
	SetupDiDestroyDeviceInfoList(devs);
}
#endif

VeCanDriver *veCanUsb2Register(void)
{
	canDriver.id = "canusb2:";
	canDriver.enumFun = enumerate;

	return &canDriver;
}
