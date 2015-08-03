#include <velib/base/types.h>
#include <velib/base/ve_string.h>
#include <velib/canhw/canhw.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>
#include <velib/platform/plt.h>

#pragma comment(lib, "canusbdrv.lib")

#include <Ftd2xx.h>
#include <lawicel_can.h>
#include <velib/canhw/canhw_driver.h>

typedef struct
{
	VeRawCanMsg *first;
	VeRawCanMsg *last;
} CANMsgQueue;

static VeCanDriver			canDriver;
static veCanGwRxEventCb * 	rxEventCb = NULL;
static CANMsgQueue			canMsgQueue = { NULL, NULL };

static un32	lastTime = 0;
static un32	overflows = 0;

/* Lawicel specific gateway variables */
typedef struct
{
	CANHANDLE	handle;
	int			id;
	char		serial[32];
	LPCSTR		bitrate;
} GatewayPrivate;

static const char *err2str(int err)
{
	switch (err)
	{
	case ERROR_CANUSB_OK:
		return "no error";
	case ERROR_CANUSB_GENERAL:
		return "general error";
	case ERROR_CANUSB_OPEN_SUBSYSTEM:
		return "open subsystem";
	case ERROR_CANUSB_COMMAND_SUBSYSTEM:
		return "command subsystem";
	case ERROR_CANUSB_NOT_OPEN:
		return "channel not open";
	case ERROR_CANUSB_TX_FIFO_FULL:
		return "tx fifo full";
	case ERROR_CANUSB_INVALID_PARAM:
		return "invalid param";
	case ERROR_CANUSB_NO_MESSAGE:
		return "no message";
	case ERROR_CANUSB_MEMORY_ERROR:
		return "memort error";
	case ERROR_CANUSB_NO_DEVICE:
		return "no device";
	case ERROR_CANUSB_TIMEOUT:
		return "timeout";
	case ERROR_CANUSB_INVALID_HARDWARE:
		return "invalid hardware";
	default:
		return "unknown error";
	}
}

#if !CFG_CANUSB_USE_CALLBACK
static UINT __stdcall WINAPI thread(void *parm)
{
	VeCanGateway *	gw = (VeCanGateway *) parm;
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;
	
	for (;;)
	{
		VeRawCanMsg *	rawCanMsg;
		CANMsg			canMsg;
		int				result;
		un32			time;

		result = canusb_Read(priv->handle, &canMsg);

		if (result != ERROR_CANUSB_OK) {
			logE("CANHW-CANUSB", err2str(result));
			continue;
		}

		if (canMsg.len > sizeof(rawCanMsg->mdata)) {
			logE("CANHW-CANUSB", "overrun error");
			continue;
		}

		if ((rawCanMsg = calloc(1, sizeof(VeRawCanMsg))) != NULL)
		{
			rawCanMsg->length = (VeCanLength)canMsg.len;
			rawCanMsg->canId = (VeCanId)canMsg.id;
			
			if (rawCanMsg->flags & CANMSG_EXTENDED)
				rawCanMsg->flags = VE_CAN_EXT;
			else 
				rawCanMsg->flags = VE_CAN_STD;

			memcpy(rawCanMsg->mdata, canMsg.data, canMsg.len);

#if CFG_CANHW_STAMP
			time = canMsg.timestamp;
			if (time < lastTime)
				overflows++;
			lastTime = time;

			/* Timestamp is declared as an un32, however it overflows @ 0xFFFF */
			rawCanMsg->extra.time.us = (un64) time + overflows * 0xFFFF;
			rawCanMsg->extra.time.us *= 1000;
#endif

			/* Mutex lock */
			pltInterruptBegins(1);

			/* Add to queue */
			if (canMsgQueue.first) {
				((J1939Msg *)canMsgQueue.last)->next = (J1939Msg *)rawCanMsg;
				(J1939Msg *)canMsgQueue.last = ((J1939Msg *)canMsgQueue.last)->next;
			} else {
				/* First one */
				canMsgQueue.first = rawCanMsg;
				canMsgQueue.last = canMsgQueue.first;
			}

			if (rxEventCb)
				rxEventCb(NULL);

			/* Mutex free */
			pltInterruptEnds(1);
		}
	}
	return 0;
}
#endif

#if CFG_CANUSB_USE_CALLBACK
static void __stdcall canEvent(CANMsg *canMsg)
{
	VeRawCanMsg *	rawCanMsg;
	un32			time;

	if (canMsg->len > sizeof(rawCanMsg->mdata))
		return;

	if ((rawCanMsg = calloc(1, sizeof(VeRawCanMsg))) != NULL)
	{
		rawCanMsg->length = (VeCanLength)canMsg->len;
		rawCanMsg->canId = (VeCanId)canMsg->id;
		
		if (rawCanMsg->flags & CANMSG_EXTENDED)
			rawCanMsg->flags = VE_CAN_EXT;
		else 
			rawCanMsg->flags = VE_CAN_STD;

		memcpy(rawCanMsg->mdata, canMsg->data, canMsg->len);

#if CFG_CANHW_STAMP
		time = canMsg->timestamp;
		if (time < lastTime)
			overflows++;
		lastTime = time;

		/* Timestamp is declared as an un32, however it overflows @ 0xFFFF */
		rawCanMsg->extra.time.us = (un64) time + overflows * 0xFFFF;
		rawCanMsg->extra.time.us *= 1000;
#endif

		/* Mutex lock */
		pltInterruptBegins(1);

		/* Add to queue */
		if (canMsgQueue.first) {
			((J1939Msg *)canMsgQueue.last)->next = (J1939Msg *)rawCanMsg;
			(J1939Msg *)canMsgQueue.last = ((J1939Msg *)canMsgQueue.last)->next;
		} else {
			/* First one */
			canMsgQueue.first = rawCanMsg;
			canMsgQueue.last = canMsgQueue.first;
		}

		if (rxEventCb)
			rxEventCb(NULL);

		/* Mutex free */
		pltInterruptEnds(1);
	}
}
#endif

/* stub, opening is postponed till busOn since bitrate is needed. */
static veBool open(VeCanGateway *gw)
{
	return veTrue;
}

/* only convert here to bit rate code understood by the lawicel */
static veBool setBitrate(VeCanGateway *gw, un16 kbit)
{
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;

	switch (kbit)
	{
	case 1000:
		priv->bitrate = "1000";
		return veTrue;
	case 800:
		priv->bitrate = "800";
		return veTrue;
	case 500:
		priv->bitrate = "500";
		return veTrue;
	case 250:
		priv->bitrate = "250";
		return veTrue;
	case 100:
		priv->bitrate = "100";
		return veTrue;
	case 50:
		priv->bitrate = "50";
		return veTrue;
	case 20:
		priv->bitrate = "20";
		return veTrue;
	case 10:
		priv->bitrate = "10";
		return veTrue;
	}

	return veFalse;
}

/* actually open the gateway */
static veBool busOn(VeCanGateway *gw)
{
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;
	unsigned long	flags = CANUSB_FLAG_BLOCK;

#if CFG_CANHW_STAMP
	flags |= CANUSB_FLAG_TIMESTAMP;
#endif

	priv->handle = canusb_Open(priv->serial, priv->bitrate, CANUSB_ACCEPTANCE_CODE_ALL, 
		CANUSB_ACCEPTANCE_MASK_ALL, flags);

	if (!priv->handle) {
		logE("CANHW-CANUSB", "failed to open channel");
		return veFalse;
	}

	/* Set Rx event callback, same for al gw's */
	if (!rxEventCb)
		rxEventCb = gw->rxEventCb;

#if CFG_CANUSB_USE_CALLBACK
	/* Local rx callback */
	if (canusb_setReceiveCallBack(priv->handle, (LPFNDLL_RECEIVE_CALLBACK)canEvent) <= 0) {
		logE("CANHW-CANUSB", "failed to set rx callback");
		return veFalse;
	}
#else
	CreateThread(NULL, 0, thread, gw, 0, NULL);
#endif

	return veTrue;
}

/* reading a raw CAN bus message */
static veBool readMsg(struct VeCanGatewayS *gw, VeRawCanMsg *rawCanMsg)
{
	J1939Msg *next;

	/* Mutex lock */
	pltInterruptBegins(1);

	if (!canMsgQueue.first) {
		pltInterruptEnds(1);
		return veFalse;
	}

	/* Copy message, but leave type untouched */
	rawCanMsg->flags = canMsgQueue.first->flags;
	rawCanMsg->length = canMsgQueue.first->length;
	rawCanMsg->canId = canMsgQueue.first->canId;
	rawCanMsg->extra.time.us = canMsgQueue.first->extra.time.us;
	memcpy(rawCanMsg->mdata, canMsgQueue.first->mdata, rawCanMsg->length);

	/* Next queued message */
	next = ((J1939Msg *)canMsgQueue.first)->next;
	free(canMsgQueue.first);
	canMsgQueue.first = (VeRawCanMsg *)next;

	/* Mutex free */
	pltInterruptEnds(1);

	return veTrue;
}

/* sending a raw CAN bus message */
static veBool sendMsg(struct VeCanGatewayS *gw, VeRawCanMsg *rawCanMsg)
{
	CANMsgEx			canMsg;
	GatewayPrivate *	priv;
	int					result;

	priv = (GatewayPrivate *) gw->ctx;

	if ( (rawCanMsg->flags & (VE_CAN_EXT | VE_CAN_STD)) == 0) {
		logE("CANHW-CANUSB", "msg type not supported");
		return veFalse;
	}

	pltInterruptSuspend();

	canMsg.len = (un8)rawCanMsg->length;
	canMsg.id = (un32)rawCanMsg->canId;
	canMsg.flags = 0;

	if (rawCanMsg->flags & VE_CAN_EXT)
		canMsg.flags |= CANMSG_EXTENDED;

	result = canusb_WriteEx(priv->handle, &canMsg, rawCanMsg->mdata);

	if (result != ERROR_CANUSB_OK) {
		logE("CANHW-CANUSB", "%s; status: 0x%02X", err2str(result), canusb_Status(priv->handle));
		return veFalse;
	}

	pltInterruptResume();

	return veTrue;
}

static size_t gwId(VeCanGateway *gw, char *buf, size_t len)
{
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;

	return ve_snprintf(buf, len, "%s%d", canDriver.id, priv->id);
}

static size_t gwDesc(VeCanGateway *gw, char *buf, size_t len)
{
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;

	return ve_snprintf(buf, len, "Victron CANUSB; Serial %s", priv->serial);
}

/* Get memory and setup a new lawicel gateway */
static VeCanGateway *allocGateway(LPCSTR serial)
{
	static int id = 0;

	VeCanGateway *	gw = NULL;
	GatewayPrivate *priv;

	gw = calloc(1, sizeof(VeCanGateway));
	priv = calloc(1, sizeof(GatewayPrivate));

	strcpy(priv->serial, serial);
	priv->id = id++;

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

static void enumerate(void)
{
	VeCanGateway *	gw;
	int				count;
	char 			serial[32];
	
	/* Get the number of adapters */
	count = canusb_getFirstAdapter(serial, sizeof(serial));

	do
	{
		gw = allocGateway(serial);
		if (gw)
			veCanDrvNewGw(&canDriver, gw);
		count--;
	} while (count && canusb_getNextAdapter(serial, sizeof(serial)));
}

VeCanDriver *veCanusbRegister(void)
{
	static veBool once = veFalse;

	/* Mutexes */
	if (!once) {
		once = veTrue;
		pltInitCtx();
	}
	
	canDriver.id = "canusb:";
	canDriver.enumFun = enumerate;

	return &canDriver;
}
