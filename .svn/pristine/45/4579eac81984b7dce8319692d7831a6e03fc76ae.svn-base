#include <velib/base/types.h>

#ifdef __CYGWIN__
#define _WIN32 1					// use windows definitions on cygwin.
#elif defined(_UNIX_)
#define canOPEN_ACCEPT_VIRTUAL 0	// not support on *NIX
#endif

#ifdef _MSC_VER
#pragma comment(lib, "canlib32.lib")
#endif

#include <canlib.h>
#include <memory.h>

#include <velib/base/ve_string.h>
#include <velib/canhw/canhw.h>
#include <velib/canhw/canhw_kvaser.h>
#include <velib/canhw/canhw_driver.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>

static VeCanGateway* activeGw;
static VeCanDriver canDriver;

/* Kvaser specific gateway variables */
typedef struct
{
	CanHandle handle;
	int channel;
	un32 lastTime;
	un32 overflows;
} GatewayPrivate;

/* optional time stamp support */
#if CFG_CANHW_STAMP

static veBool veKvMicroSecondStamp(VeCanGateway* gw)
{
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;
	canStatus stat;
	un32 i = 1; // Set resolution to 1 us

	stat = canIoCtl(priv->handle, canIOCTL_SET_TIMER_SCALE, &i, 4);
	if (stat != canOK)
	{
		logE("CANH-KV", "canIoCtl() failed to set us resolution");
		return veFalse;
	}
	return veTrue;
}

#else

static veBool veKvMicroSecondStamp(VeCanGateway* gw)
{
	return veFalse;
}

#endif

static void CANLIBAPI canEvent(CanHandle hnd, void* ctx, unsigned int ev)
{
	VeCanGateway* gw = (VeCanGateway*) ctx;
	VE_UNUSED(hnd);
	VE_UNUSED(ev);

	if (gw->rxEventCb)
		gw->rxEventCb(gw);
}

static veBool open(VeCanGateway* gw)
{
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;

	priv->handle = canOpenChannel(priv->channel, canOPEN_ACCEPT_VIRTUAL);
	if (priv->handle < 0) {
		logE("CANH-KV", "Connection could not be made for kvaser: %d", priv->channel);
		return veFalse;
	}

	activeGw = gw;
	veKvMicroSecondStamp(gw);

	kvSetNotifyCallback(priv->handle, canEvent, gw, canNOTIFY_RX);

	return veTrue;
}

static veBool setBitrate(VeCanGateway* gw, un16 kbit)
{
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;
	canStatus status;

	status = canSetBitrate(priv->handle, kbit * 1000);

	/* Work-around for Leaf bug */
	canSetBusOutputControl(priv->handle, canDRIVER_NORMAL);

	return status == canOK;
}

/* actually open the gateway */
static veBool busOn(VeCanGateway* gw)
{
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;

	return (veBool) (canBusOn(priv->handle) == canOK);
}

/* reading a raw CAN bus message */
static veBool readMsg(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg)
{
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;
	canStatus status;
	long canId;
	unsigned int dlc;
	unsigned int flags;
	unsigned long time;

	for(;;)
	{
		status = canRead(priv->handle, &canId, rawCanMsg->mdata, &dlc, &flags, &time);

		switch (status)
		{
		case canOK:
#if CFG_CANHW_STAMP
			if (time < priv->lastTime)
				priv->overflows++;
			priv->lastTime = time;
			rawCanMsg->extra.time.us = (un64) time + priv->overflows * 0xFFFFFFFF;
#endif
			rawCanMsg->length = (VeCanLength) dlc;
			rawCanMsg->flags = (un8) ( flags & (canMSG_STD | canMSG_EXT | canMSG_RTR));

			if (flags & canMSGERR_OVERRUN)
				rawCanMsg->flags |= VE_CAN_RX_OVERRUN;

			// Error frames also have other flag set.
			// (note it is assumed that error message do not contain valid data)
			if (flags & (canMSG_ERROR_FRAME | canMSGERR_BUSERR) )
			{
				veAssert(rawCanMsg->flags == 0);

				// skip the error message itself.
				continue;
			}

			rawCanMsg->canId = (un32) canId;
			return veTrue;

		case canERR_NOMSG:
			return veFalse;

		default:
			logE("CANHW-KV", "CAN-SEND returned unknown can state");
			return veFalse;
		}
	}

	// TODO: handle more appropriate?
	logE("CANHW-KV", "WARNING: unable to read from: %d", priv->handle);

};

/* sending a raw CAN bus message */
static veBool sendMsg(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg)
{
	unsigned int flags;
	canStatus status;
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;

	/* standard flags are identical. */
	flags = rawCanMsg->flags & (canMSG_STD | canMSG_EXT | canMSG_RTR);

	/* void cast is a const cast */
	status = canWrite(priv->handle, rawCanMsg->canId, (void*) rawCanMsg->mdata,
										rawCanMsg->length, flags);

	return (veBool) (status == canOK);
}

/* Get gateway string identifier, e,g. kvaser:0 */
static size_t gwId(VeCanGateway* gw, char *buf, size_t len)
{
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;

	return ve_snprintf(buf, len, "kvaser:%d", priv->channel);
}

/* Get gateway description */
static size_t gwDesc(VeCanGateway* gw, char *buf, size_t len)
{
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;

	if (canGetChannelData(priv->channel, canCHANNELDATA_DEVDESCR_ASCII,
										buf, len) == canOK)
		return strlen(buf);

	return ve_snprintf(buf, len, "error");
}

/* Get memory and setup a new kvaser gateway */
static VeCanGateway* allocGateway(int channel)
{
	VeCanGateway *gw = NULL;
	GatewayPrivate *priv;
	un32 type;

	gw = calloc(1, sizeof(VeCanGateway));
	priv = calloc(1, sizeof(GatewayPrivate));
	priv->channel = channel;
	gw->ctx = priv;

	gw->flags = VE_CAN_GW_CAN;
	if (canGetChannelData(channel, canCHANNELDATA_CARD_TYPE, &type, sizeof(type)) == canOK &&
			type == canHWTYPE_VIRTUAL)
			gw->flags |= VE_CAN_GW_VIRTUAL;

	gw->openFun = open;
	gw->bitrateFun = setBitrate;
	gw->busOnFun = busOn;
	gw->readStub.fun = readMsg;
	gw->sendStub.fun = sendMsg;
	gw->idFun = gwId;
	gw->descFun = gwDesc;

	return gw;
}

/* Find kvaser CAN gateways */
static void enumerate(void)
{
	int count;
	int n;
	VeCanGateway *gw;

	if (canGetNumberOfChannels(&count) != canOK)
		return;

	for (n = 0; n < count; n++) {
		gw = allocGateway(n);
		if (gw)
			veCanDrvNewGw(&canDriver, gw);
	}
}

/* Add Kvaser support */
VeCanDriver* veKvaserRegister(void)
{
	canDriver.id = "kvaser:";
	canDriver.enumFun = enumerate;

	canInitializeLibrary();

	return &canDriver;
}

#if CFG_CANHW_KVASER_COMPAT

/*=== stubs for backwards compatibility ===*/

/* to be removed, use the canhw_driver equivalant */

/// Implements @ref veCanInit
void veKvCanInit(void)
{
	veKvaserRegister();
}

/// Implements @ref veCanSetBitRate.
veBool veKvCanSetBitRate(un16 kbit)
{
	return veCanGwSetBitRate(activeGw, kbit);
}

VE_DCL veBool veKvCanRead(VeRawCanMsg *rawCanMsg)
{
	return veCanGwRead(activeGw, rawCanMsg);
}

veBool veKvCanSend(VeRawCanMsg *rawCanMsg)
{
	return veCanGwSend(activeGw, rawCanMsg);
}

/// Implements @ref veCanBusOn
veBool veKvCanBusOn(void)
{
	return veCanGwBusOn(activeGw);
}

/// Opens the n'th channel of the Kvaser driver.
VeKvCanHndl veKvCanOpen(int channel)
{
	GatewayPrivate* priv;

	if (!activeGw) {
		logE("CANHW-KVASER", "no gateway active");
		return veFalse;
	}

	if (!open(activeGw))
		return -1;

	priv = (GatewayPrivate*) activeGw->ctx;
	return priv->handle;
}

void veKvCanSetHandle(VeKvCanHndl handle)
{
	VeCanGateway* gw = canDriver.gateways;

	while (gw)
	{
		GatewayPrivate* priv = (GatewayPrivate*) activeGw->ctx;
		if (priv->handle == handle) {
			activeGw = gw;
			return;
		}
	}

	activeGw = NULL;
}

VeKvCanHndl veKvCanGetHandle(void)
{
	GatewayPrivate* priv;

	if (!activeGw)
		return -1;

	priv = (GatewayPrivate*) activeGw->ctx;
	return priv->handle;
}

#endif
