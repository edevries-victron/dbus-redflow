#include <stdlib.h>

#include <velib/base/types.h>
#include <velib/base/ve_string.h>
#include <velib/canhw/canhw.h>
#include <velib/canhw/canhw_driver.h>

static VeCanDriver canDriver;

static veBool open(VeCanGateway* gw)
{
	return veTrue;
}

static veBool setBitrate(VeCanGateway* gw, un16 kbit)
{
	return veTrue;
}

static veBool busOn(VeCanGateway* gw)
{
	return veTrue;
}

/* reading a raw CAN bus message */
static veBool readMsg(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg)
{
	return veFalse;
};

/* sending a raw CAN bus message */
static veBool sendMsg(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg)
{
	rawCanMsg->flags |= VE_CAN_DO_LOOP;

	return veTrue;
}

static size_t gwId(VeCanGateway* gw, char *buf, size_t len)
{
	return ve_snprintf(buf, len, "loop");
}

static size_t gwDesc(VeCanGateway* gw, char *buf, size_t len)
{
	return ve_snprintf(buf, len, "loopback interface");
}

/* Get memory and setup a new peak geteway */
static VeCanGateway* allocGateway(void)
{
	VeCanGateway *gw = NULL;

	gw = calloc(1, sizeof(VeCanGateway));

	gw->flags = VE_CAN_GW_CAN | VE_CAN_GW_VIRTUAL;

	gw->openFun = open;
	gw->bitrateFun = setBitrate;
	gw->busOnFun = busOn;
	gw->readStub.fun = readMsg;
	gw->sendStub.fun = sendMsg;
	gw->idFun = gwId;
	gw->descFun = gwDesc;

	return gw;
}

/* there is no enumeration, just add the loopack */
static void enumerate(void)
{
	VeCanGateway *gw = allocGateway();
	if (gw)
		veCanDrvNewGw(&canDriver, gw);
}

VeCanDriver* veCanLoopbackRegister(void)
{
	canDriver.id = "loop";
	canDriver.enumFun = enumerate;

	return &canDriver;
}
