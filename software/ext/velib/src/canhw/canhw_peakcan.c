#include <velib/base/types.h>
#include <velib/base/ve_string.h>
#include <velib/canhw/canhw.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>

typedef un8 BYTE;

#include <PCANBasic.h>
#include <velib/canhw/canhw_driver.h>

static VeCanDriver canDriver;

/* Peak specific gateway variables */
typedef struct
{
	TPCANHandle handle;
	TPCANBaudrate baudrate;
	char descr[32];
	HANDLE rxEvent;
} GatewayPrivate;

static UINT __stdcall WINAPI thread(void *parm)
{
	VeCanGateway *gw = (VeCanGateway *) parm;
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;

	for (;;) {
		if (WaitForSingleObject(priv->rxEvent, INFINITE) != WAIT_OBJECT_0)
			break;

		ResetEvent(priv->rxEvent);

		if (gw->rxEventCb)
			gw->rxEventCb(gw);
	}
	return 0;
}

/* stub, opening in postponed till busOn since bitrate is needed. */
static veBool open(VeCanGateway* gw)
{
	return veTrue;
}

/* only convert here to bit rate code understood by the peakcan */
static veBool setBitrate(VeCanGateway* gw, un16 kbit)
{
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;

	switch (kbit)
	{
	case 1000:
		priv->baudrate = PCAN_BAUD_1M;
		return veTrue;
	case 800:
		priv->baudrate = PCAN_BAUD_800K;
		return veTrue;
	case 500:
		priv->baudrate = PCAN_BAUD_500K;
		return veTrue;
	case 250:
		priv->baudrate = PCAN_BAUD_250K;
		return veTrue;
	case 125:
		priv->baudrate = PCAN_BAUD_125K;
		return veTrue;
	case 100:
		priv->baudrate = PCAN_BAUD_100K;
		return veTrue;
	}

	return veFalse;
}

/* actually open the gateway */
static veBool busOn(VeCanGateway* gw)
{
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;
	TPCANStatus status;

	status = CAN_Initialize(priv->handle, priv->baudrate, 0, 0, 0);
	if (status != PCAN_ERROR_OK)
		return veFalse;

	priv->rxEvent = CreateEvent(0, TRUE, FALSE, TEXT("rx"));
	CAN_SetValue(priv->handle, PCAN_RECEIVE_EVENT, &priv->rxEvent, sizeof(priv->rxEvent));
	CreateThread(NULL, 0, thread, gw, 0, NULL);

	return veTrue;
}

/* reading a raw CAN bus message */
static veBool readMsg(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg)
{
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;
	TPCANStatus status;
	TPCANMsg peakMsg;
	TPCANTimestamp peakStamp;

	status = CAN_Read(priv->handle, &peakMsg, &peakStamp);
	if (status == PCAN_ERROR_QRCVEMPTY)
		return veFalse;

	if (status != PCAN_ERROR_OK) {
		/* @todo Changed to logI, better fix the cause */
		logI("CAN-PEAK", "error reading %d", status);
		return veFalse;
	}

	if (peakMsg.MSGTYPE == PCAN_MESSAGE_EXTENDED)
		rawCanMsg->flags = VE_CAN_EXT;
	else if (peakMsg.MSGTYPE == PCAN_MESSAGE_STANDARD)
		rawCanMsg->flags = VE_CAN_STD;
	else {
		logE("CAN-PEAK", "rx, msg type not supported, dropped");
		return veFalse;
	}

	rawCanMsg->canId = peakMsg.ID;
	rawCanMsg->length = peakMsg.LEN;
	if (peakMsg.LEN > sizeof(rawCanMsg->mdata))
		return veFalse;
	memcpy(rawCanMsg->mdata, peakMsg.DATA, peakMsg.LEN);

#if CFG_CANHW_STAMP
	rawCanMsg->extra.time.us = (un64) peakStamp.millis + 0xFFFFFFFF * peakStamp.millis_overflow;
	rawCanMsg->extra.time.us *= 1000;
	rawCanMsg->extra.time.us += peakStamp.micros;
#endif

	return veTrue;
}

/* sending a raw CAN bus message */
static veBool sendMsg(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg)
{
	TPCANMsg peakMsg;
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;

	if (rawCanMsg->length > sizeof(peakMsg.DATA)) {
		logE("CAN-PEAK", "msg too long");
		return veFalse;
	}

	if ( (rawCanMsg->flags & (VE_CAN_EXT | VE_CAN_STD)) == 0) {
		logE("CAN-PEAK", "msg type not supported");
		return veFalse;
	}

	memcpy(peakMsg.DATA, rawCanMsg->mdata, rawCanMsg->length);
	peakMsg.LEN = (BYTE) rawCanMsg->length;
	peakMsg.ID = rawCanMsg->canId;

	if (rawCanMsg->flags & VE_CAN_EXT)
		peakMsg.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	else if (rawCanMsg->flags & VE_CAN_STD)
		peakMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;

	return CAN_Write(priv->handle, &peakMsg) == PCAN_ERROR_OK;
}

static size_t gwId(VeCanGateway* gw, char *buf, size_t len)
{
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;

	return ve_snprintf(buf, len, "peak:usb%d", priv->handle - PCAN_USBBUS1 + 1);
}

static size_t gwDesc(VeCanGateway* gw, char *buf, size_t len)
{
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;

	return ve_snprintf(buf, len, "%s", priv->descr);
}

/* Get memory and setup a new peak geteway */
static VeCanGateway* allocGateway(TPCANHandle handle)
{
	VeCanGateway *gw = NULL;
	GatewayPrivate *priv;

	gw = calloc(1, sizeof(VeCanGateway));
	priv = calloc(1, sizeof(GatewayPrivate));
	priv->handle = handle;
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

static TPCANHandle const devices[] =
{
	PCAN_USBBUS1,
	PCAN_USBBUS2,
	PCAN_USBBUS3,
	PCAN_USBBUS4,
	PCAN_USBBUS5,
	PCAN_USBBUS6,
	PCAN_USBBUS7,
	PCAN_USBBUS8,
	PCAN_NONEBUS
};

/* there is no enumario*/
static void enumerate(void)
{
	TPCANStatus status;
	VeCanGateway *gw;
	GatewayPrivate* priv;
	TPCANHandle const* handle = devices;

	while (*handle != PCAN_NONEBUS)
	{
		status = CAN_Initialize(*handle, PCAN_BAUD_250K, 0, 0, 0);
		if (status != PCAN_ERROR_OK) {
			handle++;
			continue;
		}

		gw = allocGateway(*handle);
		priv = (GatewayPrivate*) gw->ctx;

		/* note, getting description only works with opened driver */
		status = CAN_GetValue(*handle, PCAN_HARDWARE_NAME, priv->descr, sizeof(priv->descr));
		if (status != PCAN_ERROR_OK)
			strcpy(priv->descr, "error");

		veCanDrvNewGw(&canDriver, gw);
		CAN_Uninitialize(*handle);

		handle++;
	}
}

VeCanDriver* vePeakRegister(void)
{
	canDriver.id = "peak:";
	canDriver.enumFun = enumerate;

	return &canDriver;
}
