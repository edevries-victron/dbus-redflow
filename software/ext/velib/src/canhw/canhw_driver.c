#ifdef _MSC_VER
#pragma warning( disable : 4068 )
#endif

#if !CFG_CANHW_DRIVER_STATIC
# ifdef WIN32
#  ifdef _MSC_VER
#   pragma comment(lib, "ShLwApi.lib")
#  endif
#  include <ShLwApi.h>
#  include <string.h>
#  include <stdio.h>
#  include <tchar.h>
# endif
#endif

#include <velib/velib_inc_J1939.h>

#include <velib/base/ve_string.h>
#include <velib/canhw/canhw_driver.h>
#include <velib/canhw/canhw_console.h>
#include <velib/J1939/J1939_device.h>
#include <velib/nmea2k/n2k_fp.h>
#include <velib/types/ve_str.h>
#include <velib/types/ve_stamp.h>
#include <velib/utils/ve_logger.h>

#define MOD "canhw_drv"
#define error(f, ...)	logE(MOD, f, ##__VA_ARGS__)
#define warning(f, ...)	logW(MOD, f, ##__VA_ARGS__)
#define info(f, ...)	logI(MOD, f, ##__VA_ARGS__)

/*
 * Send an reviece functions are created add runtime, so it
 * can be configured on a per gateway bases and expanded.
 * These defines sort the functions.
 */
#define SORT_SEND_FP_DUMP		50
#define SORT_SEND				100
#define SORT_SEND_LOOPBACK		150
#define SORT_SEND_DUMP			200

/* note: the read function is skipped if the loopback returns a msg */
#define SORT_READ_LOOPBACK		50
#define SORT_READ				100
#define SORT_READ_DUMP			200
#define SORT_READ_NAD			300

static VeCanDriver *drivers;
static VeCanGateway *lastUsedGw;

#if CFG_WITH_NMEA2K
static void veCanGwAddN2kSupport(VeCanGateway* gw);
static veBool monitorAddressChange(VeCanGateway* gw, J1939Msg *msg);
#endif
static void veCanGwAddSendN2kFun(VeCanGateway* gw, VeCanGwSendN2kStub* send);
static veBool sendDoLoop(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg);
static veBool readIsLoop(VeCanGateway* gw, VeRawCanMsg *rawCanMsg);
static void veCanGwAddSendFun(VeCanGateway* gw, VeCanGwSendStub* send);
static void veCanGwAddReadFun(VeCanGateway* gw, VeCanGwReadStub* read);

void veCanDrvRegister(VeCanDriver *driver)
{
	VeCanDriver *tmp = drivers;

	/* don't add the same driver multiple times */
	while (tmp) {
		if (strcmp(tmp->id, driver->id) == 0)
			return;
		tmp = tmp->next;
	}

	info("adding driver %s", driver->id);
	driver->next = drivers;
	drivers = driver;
}

/* CAN Drivers call this to register a new gateway */
void veCanDrvNewGw(VeCanDriver *driver, VeCanGateway *gw)
{
	VeCanGateway *last;

	gw->next = NULL;

	gw->sendStub.sort = SORT_SEND;
	gw->send = &gw->sendStub;
	gw->readStub.sort = SORT_READ;
	gw->read = &gw->readStub;
	gw->sendN2kStub.sort = SORT_SEND;
	gw->sendN2k = &gw->sendN2kStub;
	gw->readN2kStub.sort = SORT_READ;
	gw->readN2k = &gw->readN2kStub;

#if CFG_WITH_NMEA2K
	{
		/*
		 * note: this only works with 1 NMEA2000 gateway at the moment,
		 * since there is only 1 device by default, which is good enough.
		 */
		static VeCanGwSendN2kStub nadStub;

		/*
		 * add software fast-packet <-> CAN message conversion
		 * for gateways which interface the bus directly
		 */
		if (gw->flags & VE_CAN_GW_CAN) {
			veCanGwAddN2kSupport(gw);
		} else if (gw->flags & VE_CAN_GW_NMEA2000) {
			nadStub.fun = monitorAddressChange;
			nadStub.sort = SORT_READ_NAD;
			veCanGwAddSendN2kFun(gw, &nadStub);
		}
	}
#endif

	/* add a loopback if the gateway doesn't loop */
	if (!(gw->flags & VE_CAN_GW_LOOPBACK)) {
		gw->loopSendStub.fun = sendDoLoop;
		gw->loopSendStub.sort = SORT_SEND_LOOPBACK;
		veCanGwAddSendFun(gw, &gw->loopSendStub);

		gw->loopReadStub.fun = readIsLoop;
		gw->loopReadStub.sort = SORT_READ_LOOPBACK;
		veCanGwAddReadFun(gw, &gw->loopReadStub);
	}

	if (!driver->gateways) {
		driver->gateways = gw;
		return;
	}

	last = driver->gateways;
	while (last->next)
		last = last->next;

	last->next = gw;
}


void veCanDrvEnumerate(void)
{
	VeCanDriver *driver = drivers;

	while (driver) {
		if (driver->enumFun)
			driver->enumFun();
		driver = driver->next;
	}
}

void veCanDrvForeachGw(VeCanDrvGwFun *fun, void *ctx)
{
	VeCanDriver *driver = drivers;

	while (driver) {
		VeCanGateway* gw = driver->gateways;
		while (gw) {
			fun(gw, ctx);
			gw = gw->next;
		}
		driver = driver->next;
	}
}

static void tick(struct VeCanGatewayS *gw, void *ctx)
{
	VE_UNUSED(ctx);

	if (gw->tickFun)
		gw->tickFun(gw);
}

void veCanDrvTick(void)
{
#if CFG_WITH_NMEA2K
	n2kFpTick();
#endif
	veCanDrvForeachGw(tick, NULL);
}

static void printGw(struct VeCanGatewayS *gw, void *ctx)
{
	char buf[100];
	VE_UNUSED(ctx);

	if (veCanGwId(gw, buf, sizeof(buf)) >= sizeof(buf))
		return;

	printf("%-20s ", buf);

	if (veCanGwDesc(gw, buf, sizeof(buf)) >= sizeof(buf))
		return;

	printf("%s\n", buf);
}

/* Print a list of found gateways to stdout */
void veCanDrvPrintGwList(void)
{
	veCanDrvForeachGw(printGw, NULL);
}

static void countGw(struct VeCanGatewayS *gw, void *ctx)
{
	un8 *gws = (un8 *)ctx;

	if ((gw->flags & VE_CAN_GW_VIRTUAL) == 0)
		(*gws)++;
}

/* Get the number of attached gateways */
un8 veCanDrvGetGwCount(void)
{
	un8 gws = 0;

	veCanDrvForeachGw(countGw, &gws);

	return gws;
}

/* helper for veCanGwGet */
static void matchGateway(struct VeCanGatewayS *gw, void *ctx)
{
	char buf[100];

	if (veCanGwId(gw, buf, sizeof(buf)) >= sizeof(buf))
		return;

	if (strcmp(buf, (char*) ctx) == 0)
		lastUsedGw = gw;
}

/* Get a gateway by its id */
VeCanGateway *veCanGwGet(char const *id)
{
	lastUsedGw = NULL;
	veCanDrvForeachGw(matchGateway, (void*) id);

	if (!lastUsedGw)
		logE("CANHW", "Could not find driver '%s'", id);

	return lastUsedGw;
}

/*
 * If there is only one physical gateway this function will return it.
 * NULL otherwise.
 */
VeCanGateway *veCanGwFindDefault(void)
{
	VeCanDriver *driver = drivers;
	VeCanGateway* retGw = NULL;

	while(driver)
	{
		VeCanGateway* gw = driver->gateways;
		while (gw)
		{
			if ((gw->flags & VE_CAN_GW_VIRTUAL) == 0)
			{
				/* If there is more then one physical gateway, let the user choose */
				if (retGw)
					return NULL;

				/* A good candidate for now, until another valid gw is found */
				retGw = gw;
			}

			gw = gw->next;
		}
		driver = driver->next;
	}

	lastUsedGw = retGw;
	return retGw;
}

veBool veCanGwOpen(VeCanGateway *gw)
{
	if (!gw)
		return veFalse;

	if (!gw->openFun) {
		error("no open function");
		return veFalse;
	}

#if CFG_WITH_NMEA2K
	if (gw->flags & VE_CAN_GW_NMEA2000) {
		/* NMEA 2000 gateways claim addresses themselves */
		if (gw->getN2kNad)
			j1939Device.nad = gw->getN2kNad(gw);
		j1939Device.flags = J1939_ACL_VALID_NAD | J1939_ACL_DISABLED;
	}
#endif

	return gw->openFun(gw);
}

veBool veCanGwSetBitRate(VeCanGateway *gw, un16 kbit)
{
	if (!gw)
		return veFalse;

	if (!gw->bitrateFun) {
		error("no bitrate function");
		return veFalse;
	}

	return gw->bitrateFun(gw, kbit);
}

veBool veCanGwBusOn(VeCanGateway *gw)
{
	if (!gw)
		return veFalse;

	if (!gw->busOnFun) {
		error("no bus on function");
		return veFalse;
	}

	return gw->busOnFun(gw);
}

#if CFG_CANHW_STAMP
static void handleTime(VeCanGateway* gw, J1939Msg *msg)
{
	/* loopback messages are stamped on their way out */
	if (msg->flags & VE_CAN_IS_LOOP)
		return;

	if (!gw->timeStart.us) {
		veStampGet(&gw->timeStart);
		gw->timeStart.us -= msg->extra.time.us;
	}
	msg->extra.time.us += gw->timeStart.us;

	if (msg->type.p.kind == J1939_SF)
		veStampSync(1, &msg->extra.time, 0);
}
#else
static void handleTime(VeCanGateway* gw, J1939Msg *msg)
{
	VE_UNUSED(gw);
	VE_UNUSED(msg);
}
#endif


veBool veCanGwRead(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg)
{
	VeCanGwReadStub* read;

	if (!gw)
		return veFalse;

	rawCanMsg->flags = 0;
	read = gw->read;

	/* Forward the message to all readers */
	do {
		if (!read->fun) {
			error("no read function");
			return veFalse;
		}
		/*
		 * To allow loopback message to go through the normal input path,
		 * the actual read routine must be skipped in case of a loopback msg
		 * or it might get overwritten.
		 */
		if (!(read->sort == SORT_READ && (rawCanMsg->flags & VE_CAN_IS_LOOP)))
		{
			/* stop on error */
			if (!read->fun(gw, rawCanMsg))
				return veFalse;
		}

		if (read->sort == SORT_READ)
			handleTime(gw, (J1939Msg*) rawCanMsg);

		read = read->next;
	} while (read);

	return veTrue;
}

veBool veCanGwSend(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg)
{
	VeCanGwSendStub* send;

	if (!gw)
		return veFalse;

	send = gw->send;

#if CFG_CANHW_STAMP
	veStampGet(&rawCanMsg->extra.time);
#endif

	/* Forward the message to all senders */
	do {
		if (!send->fun) {
			logE("CANHW", "no send function");
			return veFalse;
		}
		/* stop on error */
		if (!send->fun(gw, rawCanMsg))
			return veFalse;

		send = send->next;
	} while (send);

	return veTrue;
}

J1939Msg* veCanGwReadN2k(VeCanGateway* gw)
{
	VeCanGwReadN2kStub* read;
	J1939Msg* ret = NULL;

	if (!gw)
		return veFalse;

	read = gw->readN2k;

	/* Forward the message to all readers */
	do {
		if (!read->fun) {
			logE("CANHW", "no n2k read function");
			return veFalse;
		}

		ret = read->fun(gw, ret);
		/* stop on error */
		if (!ret)
			return NULL;

		/* hook in close to were the time is set */
		if (read->sort == SORT_READ && (gw->flags & VE_CAN_GW_NMEA2000))
			handleTime(gw, ret);

		read = read->next;
	} while (read);

	return ret;
}

veBool veCanGwSendN2k(VeCanGateway* gw, J1939Msg* msg)
{
	VeCanGwSendN2kStub* send;

	if (!gw)
		return veFalse;

	send = gw->sendN2k;

#if CFG_CANHW_STAMP
	veStampGet(&msg->extra.time);
#endif

	/* Forward the message to all senders */
	do {
		if (!send->fun) {
			error("CANHW", "no n2k send function");
			return veFalse;
		}
		/* stop on error */
		if (!send->fun(gw, msg))
			return veFalse;

		send = send->next;
	} while (send);

	return veTrue;
}

/* string identifier */
size_t veCanGwId(VeCanGateway* gw, char *buf, size_t len)
{
	if (!gw || !gw->idFun)
		return ve_snprintf(buf, len, "unknown");

	return gw->idFun(gw, buf, len);
}

/* Description */
size_t veCanGwDesc(VeCanGateway* gw, char *buf, size_t len)
{
	if (!gw || !gw->descFun)
		return ve_snprintf(buf, len, "unknown");

	return gw->descFun(gw, buf, len);
}

/* fast-packet reconstruction */

#if CFG_CANHW_J1939_COMPATIBLE != 1
#error need CFG_CANHW_J1939_COMPATIBLE
#endif

#if CFG_WITH_NMEA2K
/* local reconstruction */
static J1939Msg* readN2k(VeCanGateway* gw, J1939Msg* fwd)
{
	J1939Sf* msg = j1939SfRxAlloc();
	N2kFp* fp;
	VE_UNUSED(fwd);

	if (!msg)
		return NULL;

	for (;;)
	{
		if (!veCanGwRead(gw, (VeRawCanMsg*) msg)) {
			j1939SfRxFree(msg);
			return NULL;
		}

		if (!n2kIsFp(&msg->msgId))
			return (J1939Msg*) msg;

		fp = n2kFpData(msg);
		if (fp) {
			j1939SfRxFree(msg);
			return (J1939Msg*) fp;
		}
	}

	j1939SfRxFree(msg);
	return NULL;
}

/* local conversion to CAN messages */
static veBool sendN2k(VeCanGateway* gw, J1939Msg* msg)
{
	if (msg->type.p.kind == J1939_SF)
		return veCanGwSend(gw, (VeRawCanMsg*) msg);
	else if (msg->type.p.kind == N2K_FP) {
		/* n2kFpSend uses veCanSend, make sure msgs go to the correct gw */
		lastUsedGw = gw;
		return n2kFpSend((N2kFp*) msg);
	}

	return veFalse;
}

/* add local nmea2000 support */
static void veCanGwAddN2kSupport(VeCanGateway* gw)
{
	gw->readN2kStub.fun = readN2k;
	gw->sendN2kStub.fun = sendN2k;
}
#endif

/* note the read param must be persistant memory */
static void veCanGwAddReadFun(VeCanGateway* gw, VeCanGwReadStub* read)
{
	VeCanGwReadStub* stub = gw->read;
	VeCanGwReadStub* prev = NULL;

	read->next = NULL;

	while (stub)
	{
		if (stub->sort == read->sort)
			return;

		if (stub->sort > read->sort) {
			read->next = stub;
			if (prev)
				prev->next = read;
			else
				gw->read = read;
			return;
		}

		prev = stub;
		stub = stub->next;
	}

	if (!prev)
		return;

	prev->next = read;
}

/* note the read param must be persistant memory */
static void veCanGwAddSendFun(VeCanGateway* gw, VeCanGwSendStub* send)
{
	VeCanGwSendStub* stub = gw->send;
	VeCanGwSendStub* prev = NULL;

	send->next = NULL;

	while (stub)
	{
		if (stub->sort == send->sort)
			return;

		if (stub->sort > send->sort) {
			send->next = stub;
			if (prev)
				prev->next = send;
			else
				gw->send = send;
			return;
		}

		prev = stub;
		stub = stub->next;
	}

	if (!prev)
		return;

	prev->next = send;
}

/* note the read param must be persistant memory */
static void veCanGwAddReadN2kFun(VeCanGateway* gw, VeCanGwReadN2kStub* read)
{
	VeCanGwReadN2kStub* stub = gw->readN2k;
	VeCanGwReadN2kStub* prev = NULL;

	read->next = NULL;

	while (stub)
	{
		if (stub->sort == read->sort)
			return;

		if (stub->sort > read->sort) {
			read->next = stub;
			if (prev)
				prev->next = read;
			else
				gw->readN2k = read;
			return;
		}

		prev = stub;
		stub = stub->next;
	}

	if (!prev)
		return;

	prev->next = read;
}

/* note the read param must be persistant memory */
static void veCanGwAddSendN2kFun(VeCanGateway* gw, VeCanGwSendN2kStub* send)
{
	VeCanGwSendN2kStub* stub = gw->sendN2k;
	VeCanGwSendN2kStub* prev = NULL;

	send->next = NULL;

	while (stub)
	{
		if (stub->sort == send->sort)
			return;

		if (stub->sort > send->sort) {
			send->next = stub;
			if (prev)
				prev->next = send;
			else
				gw->sendN2k = send;
			return;
		}

		prev = stub;
		stub = stub->next;
	}

	if (!prev)
		return;

	prev->next = send;
}

void veCanGwSetRxEventCb(VeCanGateway* gw, VeCanGwRxEventCb *cb)
{
	if (gw)
		gw->rxEventCb = cb;
}

VeCanRxEventType veCanGwRxEventType(VeCanGateway* gw, VeCanRxEventData *data)
{
	if (gw == NULL || gw->rxType == NULL)
		return VE_CAN_RX_EV_ASYNC_CALLBACK;

	return gw->rxType(gw, data);
}

/// Prints a message to the console.
static void dumpMsg(VeCanGateway* gw, J1939Msg *const canMsg, char *str)
{
	int j;
	VeStr s;

	if (!gw->dump)
		return;

	veStrNew(&s, 1024, 1024);

#if CFG_CANHW_STAMP
	{
		VeStampDec now;

		veStampToDec(&canMsg->extra.time, &now);
		veStrAddFormat(&s, "%03d.%03d ",
			now.us / 1000000,
			(now.us / 1000) % 1000);

# if defined(CFG_LOG_STAMP_UNCORRECTED_TOO)
		{
			VeStamp pc;

			veStampGetAbs(&pc);
			veStampToDec(&pc, &now);
			veStrAddFormat(&s, "- %03d.%03d", now.us / 1000000, (now.us / 1000) % 1000);
			veStrAdd(&s, ": ");
		}
# endif
	}
#endif

	// CAN identifier in J1939 format
	veStrAddFormat(&s, "%s %d.%d.%d.%02X.%02X.%02X|",
		str,
		(canMsg->msgId.w >> 26) & 0x07,
		(canMsg->msgId.w >> 25) & 0x01,
		(canMsg->msgId.w >> 24) & 0x01,
		(un8) (canMsg->msgId.w >> 16),
		(un8) (canMsg->msgId.w >> 8),
		(un8) canMsg->msgId.w);


	for (j = 0; j < canMsg->length; j++)
		veStrAddFormat(&s, " 0x%2.2X", canMsg->mdata[j]);

	puts(veStrCStr(&s));

	fflush(stdout);

	veStrFree(&s);
}

static veBool dumpMsgRx(VeCanGateway* gw, VeRawCanMsg *rawCanMsg)
{
#if CFG_WITH_NMEA2K
	/* don't dump the fragments of a fastpacket unless -dd */
	if (n2kIsFp((J1939Id const*) &rawCanMsg->canId) && gw->dump < 2)
		return veTrue;
#endif

	dumpMsg(gw, (J1939Msg*) rawCanMsg, rawCanMsg->flags & VE_CAN_RTR ? "[" :
						(rawCanMsg->flags & VE_CAN_IS_LOOP ? "{" : "<")
						);
	return veTrue;
}

static veBool dumpMsgTx(VeCanGateway* gw, VeRawCanMsg *rawCanMsg)
{
#if CFG_WITH_NMEA2K
	/* don't dump the fragments of a fastpacket unless -dd */
	if (n2kIsFp((J1939Id const*) &rawCanMsg->canId) && gw->dump < 2)
		return veTrue;
#endif

	dumpMsg(gw, (J1939Msg*) rawCanMsg, (rawCanMsg->flags & VE_CAN_RTR ? "]" : ">"));
	return veTrue;
}

#if CFG_WITH_NMEA2K
static J1939Msg* dumpMsgRxN2k(VeCanGateway* gw, J1939Msg *msg)
{
	if (msg) {
		/* these are already printed at CAN level in case of direct bus access */
		if ((gw->flags & VE_CAN_GW_CAN) && !n2kIsFp(&msg->msgId))
			return msg;

		dumpMsg(gw, msg, "<");
	}
	return msg;
}

static veBool dumpMsgTxN2k(VeCanGateway* gw, J1939Msg *msg)
{
	/* these are already printed at CAN level in case of direct bus access */
	if ((gw->flags & VE_CAN_GW_CAN) && !n2kIsFp(&msg->msgId))
		return veTrue;

	dumpMsg(gw, msg, ">");
	return veTrue;
}
#endif

veBool veCanGwShowTrace(VeCanGateway* gw, veBool dump)
{
	static VeCanGwReadStub readStub;
	static VeCanGwSendStub sendStub;
	static VeCanGwReadN2kStub readStubN2k;
	static VeCanGwSendN2kStub sendStubN2k;
	veBool ret;

	if (!gw)
		return veFalse;

	ret = gw->dump;

	readStub.fun = dumpMsgRx;
	readStub.sort = SORT_READ_DUMP;

	sendStub.fun = dumpMsgTx;
	sendStub.sort = SORT_SEND_DUMP;

#if CFG_WITH_NMEA2K
	readStubN2k.fun = dumpMsgRxN2k;
	readStubN2k.sort = SORT_READ_DUMP;

	sendStubN2k.fun = dumpMsgTxN2k;
	sendStubN2k.sort = SORT_SEND_FP_DUMP;
#endif

	gw->dump = dump;

	veCanGwAddReadFun(gw, &readStub);
	veCanGwAddSendFun(gw, &sendStub);
	veCanGwAddReadN2kFun(gw, &readStubN2k);
	veCanGwAddSendN2kFun(gw, &sendStubN2k);

	return ret;
}

#if CFG_WITH_NMEA2K
static veBool monitorAddressChange(VeCanGateway* gw, J1939Msg *msg)
{
	VE_UNUSED(gw);

	if (j1939Device.nad != msg->msgId.p.src) {
		j1939Device.nad = msg->msgId.p.src;
		j1939Device.flags |= J1939_ACL_STORE_NAD;
	}

	return veTrue;
}
#endif

static veBool sendDoLoop(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg)
{
	J1939Sf *tx = (J1939Sf*) rawCanMsg;
	J1939Msg *loop;

	if ((rawCanMsg->flags & VE_CAN_DO_LOOP) == 0)
		return veTrue;

	if (!(loop = (J1939Msg*) j1939SfRxAlloc()))
		return veFalse;

#if CFG_CAN_MSG_EXTRA
	loop->extra = tx->extra;
#endif
	memcpy(&loop->mdata, tx->mdata, 8);
	loop->length = tx->length;
	loop->msgId = tx->msgId;
	loop->flags = rawCanMsg->flags & (VE_CAN_STD | VE_CAN_EXT);

	if (gw->loopFront) {
		gw->loopTail->next = loop;
		gw->loopTail = loop;
	} else {
		gw->loopFront = loop;
		gw->loopTail = loop;
	}

	return veTrue;
}

static veBool readIsLoop(VeCanGateway* gw, VeRawCanMsg *rawCanMsg)
{
	J1939Msg* front;

	if (!gw->loopFront)
		return veTrue;

	*rawCanMsg = *(VeRawCanMsg*) gw->loopFront;
	rawCanMsg->flags |= VE_CAN_IS_LOOP;
	front = gw->loopFront;
	gw->loopFront = gw->loopFront->next;
	if (!gw->loopFront)
		gw->loopTail = NULL;
	j1939SfRxFree((J1939Sf*) front);

	return veTrue;
}

#if !CFG_CANHW_FUNCTIONS_ARE_MACROS
/*
 * Stubs for a single active gateway compatible
 * with what canhw.h expects.
 */

/// Initialises the CANbus driver
void veCanInit(void)
{
	veCanDrvInit();
}

/*== Single gateway stubs which is enough for the majority of cases ===*/

VeCanGateway *veCanGwActive(void)
{
	return lastUsedGw;
}

veBool veCanOpen(void)
{
	return veCanGwOpen(lastUsedGw);
}

veBool veCanSend(VeRawCanMsg *rawCanMsg)
{
	return veCanGwSend(lastUsedGw, rawCanMsg);
}

veBool veCanRead(VeRawCanMsg * rawCanMsg)
{
	return veCanGwRead(lastUsedGw, rawCanMsg);
}

veBool veCanSetBitRate(un16 kbit)
{
	return veCanGwSetBitRate(lastUsedGw, kbit);
}

veBool veCanBusOn(void)
{
	return veCanGwBusOn(lastUsedGw);
}

#endif

veBool veCanSendN2k(J1939Msg* msg)
{
	return veCanGwSendN2k(lastUsedGw, msg);
}

J1939Msg* veCanReadN2k(void)
{
	return veCanGwReadN2k(lastUsedGw);
}

veBool veCanShowTrace(veBool dump)
{
	return veCanGwShowTrace(lastUsedGw, dump);
}

/* dynamic linking */

typedef VeCanDriver* regFun(VeCanDriverCallbacks *callbacks);
static VeCanDriverCallbacks app;

#if !CFG_CANHW_DRIVER_STATIC
#ifdef WIN32

/* Windows, load driver by name, e.g. passing kvaser loads canhw_kvaser.dll */
void veCanDrvLoad(char const *driver)
{
	HMODULE module;
	char filename[250];
	regFun *registerFunction;
	VeCanDriver* drv;
	UINT oldErrorMode;

	/*
	 * Prevent a popup if the dll fails to load. The dll may fail to
	 * load if a driver is not installed, which might be the case, otherwise
	 * all supported drivers must always be installed.
	 *
	 * GetErrorMode() only exists on Vista and higher,
	 * call SetErrorMode() twice to achieve the same effect.
	 */
	oldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
	SetErrorMode(oldErrorMode | SEM_FAILCRITICALERRORS);

	if (ve_snprintf(filename, sizeof(filename), "canhw_%s.dll", driver) >=  sizeof(filename))
		goto out;

	if (! (module = LoadLibraryA(filename)) ) {
		logW("canhw_drv", "failed loading module %s", filename);
		goto out;
	}

	/* Check if it has any entry point*/
	registerFunction = (regFun*) GetProcAddress(module, "veCanhwRegister");

	/* Try to init and add the gateway when succesfull */
	if (registerFunction)
	{
		if ((drv = registerFunction(&app)))
			veCanDrvRegister(drv);
		else
			logW("canhw_drv", "gateway not registerd");
	}
	else
	{
		FreeLibrary(module);
		logW("canhw_drv", "veCanhwRegister not found");
	}

out:
	SetErrorMode(oldErrorMode);
}

static void veCanDrvDynamicInitDir(TCHAR *dir)
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	TCHAR DirSpec[MAX_PATH];

	_tcscpy(DirSpec, dir);
	PathAppend(DirSpec, TEXT("*.dll"));

	hFind = FindFirstFile(DirSpec, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		/* find dll named canhw_nnn.dll */
		if (_tcsstr(findFileData.cFileName, TEXT("canhw_")) == findFileData.cFileName)
		{
			TCHAR *moduleName = &findFileData.cFileName[6];	/* skip canhw_*/
			int	len = lstrlen(moduleName) - 4; /* 4 = remove .dll */
			char *moduleNameCstr;

			moduleNameCstr = (char *) malloc(len + 1);
			if (!moduleNameCstr) {
				logE("canhw_drv", "out of memory");
				exit(1);
			}
			WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)moduleName, len, moduleNameCstr, len, NULL, NULL);
			moduleNameCstr[len] = 0;
			logI("canhw_drv", "loading module %s", moduleNameCstr);
			veCanDrvLoad(moduleNameCstr);
			free(moduleNameCstr);
		}

	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);
}

/* Windows, load all drivers matching canhw_*.dll */
void veCanDrvDynamicInit(void)
{
	TCHAR DirSpec[MAX_PATH];

	if (GetModuleFileName(NULL, DirSpec, sizeof(DirSpec)) <= 0)
		return;
	PathRemoveFileSpec(DirSpec);
	veCanDrvDynamicInitDir(DirSpec);

	veCanDrvDynamicInitDir(TEXT("C:\\Program Files (x86)\\Victron Energy\\CAN"));
}

#elif defined(_UNIX_)
#include <dlfcn.h>
#include <dirent.h>

static char *path = "/usr/local/lib/canhw";

/* posix, load driver by name, e.g. passing kvaser loads canhw_kvaser.so */
void veCanDrvLoad(char const *driver)
{
	void *handle;
	regFun *registerFunction;
	VeCanDriver* drv;
	char filename[NAME_MAX];
	size_t n;

	n = ve_snprintf(filename, sizeof(filename), "%s/canhw_%s.so", path, driver);
	if (n >= sizeof(filename))
		return;

	info("loading '%s'", filename);
	if ( !(handle = dlopen(filename, RTLD_LAZY)) ) {
		warning("cannot open library '%s' '%s'", filename, dlerror());
		return;
	}

	info("locating veCanhwRegister", driver);
	if ( !(registerFunction = (regFun*) dlsym(handle, "veCanhwRegister")) ) {
		warning("veCanhwRegister not found '%s' '%s'", driver, dlerror());
		dlclose(handle);
		return;
	}

	drv = registerFunction(&app);
	if (drv)
		veCanDrvRegister(drv);
}

void veCanDrvDynamicInit(void)
{
	DIR *dir;
	struct dirent *dp;

	info("scanning %s", path);

	if ((dir = opendir(path)) == NULL) {
		error("opendir %s failed", path);
		return;
	}

	while ((dp = readdir(dir)) != NULL)
	{
		char driver[NAME_MAX];

		if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
			continue;

		if (sscanf( dp->d_name, "canhw_%[a-z]", driver) == 1)
			info("found %s", driver);

		veCanDrvLoad(driver);
	}

	closedir(dir);
}
#endif
#endif

void veCanDrvInit(void)
{
	app.version = VE_CAN_DRIVER_API;

	app.newGwFun = veCanDrvNewGw;

	app.j1939SfRxAllocFun = j1939SfRxAlloc;

#if CFG_WITH_NMEA2K
	app.n2kIsFpFun = n2kIsFp;
	app.n2kFpRxAllocFun = n2kFpRxAlloc;
	app.n2kFpRxFreeFun = n2kFpRxFree;
#endif

#if CFG_WITH_LOG_PROTO
	app.logE = logErrorVa;
	app.logI = logInfoVa;
#endif
}
