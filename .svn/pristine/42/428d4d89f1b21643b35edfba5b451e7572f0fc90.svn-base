#ifndef _VELIB_CANHW_CAN_DRIVER_H_
#define _VELIB_CANHW_CAN_DRIVER_H_

#include <stdarg.h>

#include <velib/base/base.h>
#include <velib/J1939/J1939.h>
#include <velib/nmea2k/n2k_fp.h>
#include <velib/types/ve_stamp.h>

#define VE_CAN_DRIVER_API		3

/*
 * canhw.h defines a set of functions which read/write a single
 * CAN bus message to / from the hardware. For embedded purposes /
 * CAN analyzer tools this is enough.
 *
 * NMEA Interfaces are not always capable of sending single message,
 * but require e.g. the whole fastpacket, might do the ACL procedure
 * for us, need rx / tx pgns etc.
 *
 */
struct VeCanGatewayS;

/* Some functions drivers must implement to find the supported gateways */
typedef void VeCanDrvEnumerate(void);
typedef void VeCanDrvGwFun(struct VeCanGatewayS *gw, void *ctx);
typedef veBool VeCanGwFun(struct VeCanGatewayS* gw);

/* Per gateway versions of the functions in canhw.h */
typedef veBool VeCanGwBitrateFun(struct VeCanGatewayS* gw, un16 bitrate);
typedef veBool VeCanGwReadFun(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg);
typedef veBool VeCanGwSendFun(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg);

typedef void VeCanGwTickFun(struct VeCanGatewayS* gw);
/*
 * Abstraction how to send fast-packets. Actisense and other certified
 * gateways typically expects the whole fastpackets. If there is direct
 * access to the hardware, the fastpacket fragments are sent to the gateway.
 */
typedef J1939Msg* VeCanGwReadN2kFun(struct VeCanGatewayS* gw, J1939Msg* msg);
typedef veBool VeCanGwSendN2kFun(struct VeCanGatewayS* gw, J1939Msg* msg);
typedef un8 VeCanGwGetN2kNadFun(struct VeCanGatewayS* gw);

typedef size_t VeCanGwIdFun(struct VeCanGatewayS* gw, char *buf, size_t len);
typedef size_t VeCanGwIdDescFun(struct VeCanGatewayS* gw, char *buf, size_t len);

typedef veBool VeCanGwRxEventCb(struct VeCanGatewayS* gw);

/* Allow chain the send /receive routines, to insert traces etc */

typedef struct VeCanGwReadStubS {
	struct VeCanGwReadStubS *next;
	VeCanGwReadFun *fun;
	un32 sort;
} VeCanGwReadStub;

typedef struct VeCanGwSendStubS {
	struct VeCanGwSendStubS *next;
	VeCanGwSendFun *fun;
	un32 sort;
} VeCanGwSendStub;

typedef struct VeCanGwReadN2kStubS {
	struct VeCanGwReadN2kStubS *next;
	VeCanGwReadN2kFun *fun;
	un32 sort;
} VeCanGwReadN2kStub;

typedef struct VeCanGwSendN2kStubS {
	struct VeCanGwSendN2kStubS *next;
	VeCanGwSendN2kFun *fun;
	un32 sort;
} VeCanGwSendN2kStub;

/* Gateway which can sent raw CAN messages */
#define VE_CAN_GW_CAN				1
/* NMEA 2000 gateway expecting complete fastpackets */
#define VE_CAN_GW_NMEA2000			2
/* Some drivers support virtual busses, i.o.w. not a physical bus  */
#define VE_CAN_GW_VIRTUAL			4
/* Set when the gateways returns send messages */
#define VE_CAN_GW_LOOPBACK			8

typedef enum {
	VE_CAN_RX_NONE,
	VE_CAN_RX_EV_ASYNC_CALLBACK,
	VE_CAN_RX_EV_FD
} VeCanRxEventType;

typedef union {
	int fd;
} VeCanRxEventData;

/* A gateway and it's functions, not to be called directly */
typedef struct VeCanGatewayS
{
	struct VeCanGatewayS *next;

	/* init */
	VeCanGwFun *openFun;
	VeCanGwBitrateFun *bitrateFun;
	VeCanGwFun *busOnFun;
	VeCanGwTickFun *tickFun;

	/* CAN level */
	VeCanGwReadStub readStub;
	VeCanGwSendStub sendStub;
	VeCanGwReadStub* read;
	VeCanGwSendStub* send;

	/* NMEA 2000 support */
	VeCanGwReadN2kStub readN2kStub;
	VeCanGwSendN2kStub sendN2kStub;
	VeCanGwReadN2kStub *readN2k;
	VeCanGwSendN2kStub *sendN2k;
	VeCanGwGetN2kNadFun *getN2kNad;

	/* info */
	VeCanGwIdFun *idFun;
	VeCanGwIdDescFun *descFun;

	/* callback */
	VeCanGwRxEventCb* rxEventCb;

	/* local settings */
	un32 flags;
	void *ctx;
	veBool dump;

	/* time when the gateway is opened */
	VeStamp timeStart;

	VeCanGwSendStub loopSendStub;
	VeCanGwReadStub loopReadStub;
	J1939Msg *loopFront;
	J1939Msg *loopTail;

	VeCanRxEventType (*rxType)(struct VeCanGatewayS* gw, VeCanRxEventData *data);
} VeCanGateway;

/*
 * Definition of a driver, which might support one or more
 * gateways. Typically a dll's or kernel interfaces.
 */
typedef struct VeCanDriverS
{
	/* single linked list */
	struct VeCanDriverS *next;
	/* find gateways supported by this driver */
	VeCanDrvEnumerate *enumFun;
	/* list of found gateways */
	VeCanGateway *gateways;
	/* id, include a colon, for command line selection */
	char const *id;
	/* driver specific data */
	void *ctx;
} VeCanDriver;

/* exposed functions */
typedef void VeCanDrvNewGwFun(VeCanDriver *driver, VeCanGateway *gw);
typedef J1939Sf* J1939SfRxAllocFun(void);
typedef veBool N2kIsFpFun(J1939Id const *id);
typedef N2kFp* N2kFpRxAllocFun(void);
typedef void N2kFpRxFreeFun(N2kFp *msg);
typedef void logFun(char const *module, char const *format, va_list lst);

/* Callbacks from the drivers */
typedef struct
{
	un32 version;

	/* CAN gateways */
	VeCanDrvNewGwFun *newGwFun;
	J1939SfRxAllocFun *j1939SfRxAllocFun;

	/* NMEA gateways */
	N2kIsFpFun *n2kIsFpFun;
	N2kFpRxAllocFun *n2kFpRxAllocFun;
	N2kFpRxFreeFun *n2kFpRxFreeFun;

	logFun *logE;
	logFun *logI;
} VeCanDriverCallbacks;

VE_DCL void veCanDrvInit(void);
VE_DCL void veCanDrvLoad(char const *driver);
VE_DCL void veCanDrvDynamicInit(void);

/*== register and locate functions ==*/
void veCanDrvRegister(VeCanDriver *driver);
void veCanDrvNewGw(VeCanDriver *driver, VeCanGateway *gw);
VE_DCL void veCanDrvEnumerate(void);
VE_DCL void veCanDrvForeachGw(VeCanDrvGwFun *fun, void *ctx);
void veCanDrvTick(void);
VE_DCL VeCanGateway *veCanGwGet(char const *id);
VeCanGateway *veCanGwActive(void);
VeCanGateway *veCanGwFindDefault(void);
void veCanDrvPrintGwList(void);
un8 veCanDrvGetGwCount(void);

/* Gateway info */
VE_DCL size_t veCanGwId(VeCanGateway* gw, char *buf, size_t len);
VE_DCL size_t veCanGwDesc(VeCanGateway* gw, char *buf, size_t len);

/*== per gateway functions ==*/
VE_DCL veBool veCanGwOpen(VeCanGateway *gw);
VE_DCL veBool veCanGwSetBitRate(VeCanGateway *gw, un16 kbit);
VE_DCL veBool veCanGwBusOn(VeCanGateway *gw);
veBool veCanGwRead(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg);
veBool veCanGwSend(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg);
J1939Msg* veCanGwReadN2k(VeCanGateway* gw);
veBool veCanGwSendN2k(VeCanGateway* gw, J1939Msg* msg);
veBool veCanGwShowTrace(VeCanGateway* gw, veBool dump);
void veCanGwSetRxEventCb(VeCanGateway* gw, VeCanGwRxEventCb *cb);
VeCanRxEventType veCanGwRxEventType(VeCanGateway *gw, VeCanRxEventData *data);

/* Single gateway stubs, extending canhw.h */
VE_DCL veBool veCanOpen(void);
veBool veCanSendN2k(J1939Msg* msg);
J1939Msg* veCanReadN2k(void);

/* Some implementations, which can be dynamically loaded */
veBool veCanDrvDynInit(VeCanDriverCallbacks* callbacks);
VeCanDriver* vePeakRegister(void);
VeCanDriver* veKvaserRegister(void);
VeCanDriver* veCanLoopbackRegister(void);
VeCanDriver* veCanSkRegister(void);
VeCanDriver* veCanusbRegister(void);
VeCanDriver* veCanUsb2Register(void);

#endif
