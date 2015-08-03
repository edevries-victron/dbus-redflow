#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <linux/can/raw.h>

#include <velib/base/types.h>
#include <velib/base/ve_string.h>
#include <velib/canhw/canhw.h>
#include <velib/canhw/canhw_driver.h>
#include <velib/platform/plt.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>

static VeCanDriver canDriver;

#define info(f, ...)	logI("skcan", f, ##__VA_ARGS__)
#define error(f, ...)	logE("skcan", f, ##__VA_ARGS__)

/* Socketcan specific gateway variables */
typedef struct
{
	char *dev;
	int sd;
} GatewayPrivate;

#if CFG_WITH_LIBEVENT

#include <event2/event.h>

static void rxEvent(evutil_socket_t fd, short ev, void *ctx)
{
	VE_UNUSED(fd);
	VE_UNUSED(ev);
	VeCanGateway* gw = (VeCanGateway*) ctx;

	if (gw->rxEventCb)
		gw->rxEventCb(gw);
}

static void registerLibevent(VeCanGateway* gw)
{
	struct event_base *base = pltGetLibEventBase();
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;
	struct event *ev;

	ev = event_new(base, priv->sd, EV_READ | EV_PERSIST, rxEvent, gw);
	event_add(ev, NULL);

}

#else
static void registerLibevent(VeCanGateway* gw)
{
	VE_UNUSED(gw);
}

#endif

static veBool open(VeCanGateway* gw)
{
	struct sockaddr_can addr;
	struct ifreq ifr;
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;
	int loopback = 1;

	if ((priv->sd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		error("socket failed: %s", priv->dev);
		return veFalse;
	}

	addr.can_family = PF_CAN;
	strncpy(ifr.ifr_name, priv->dev, sizeof(ifr.ifr_name));
	if (ioctl(priv->sd, SIOCGIFINDEX, &ifr)) {
		error("ioctl failed: %s", priv->dev);
		return veFalse;
	}
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(priv->sd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		error("bind failed: %d", priv->dev);
		return veFalse;
	}

	setsockopt(priv->sd, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &loopback, sizeof(loopback));

	registerLibevent(gw);

	return veTrue;
}

/* only convert here to bit rate code understood by the peakcan */
static veBool setBitrate(VeCanGateway* gw, un16 kbit)
{
	VE_UNUSED(gw);

	switch (kbit)
	{
	default:
		return veTrue;
	}

	return veFalse;
}

/* actually open the gateway */
static veBool busOn(VeCanGateway* gw)
{
	VE_UNUSED(gw);

	return veTrue;
}

/* reading a raw CAN bus message */
static veBool readMsg(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg)
{
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;

	struct msghdr msg;
	struct iovec iov[1];
	struct can_frame frame;
	ssize_t len;

	memset(&msg, 0, sizeof(msg));
	memset(iov, 0, sizeof(iov));
	iov[0].iov_base = &frame;
	iov[0].iov_len  = sizeof(frame);
	msg.msg_iov     = iov;
	msg.msg_iovlen  = 1;

	if ((len = recvmsg(priv->sd, &msg, MSG_DONTWAIT)) > 0)
	{
		// Type of message
		rawCanMsg->length = frame.can_dlc;
		rawCanMsg->canId = frame.can_id & CAN_EFF_MASK;
		rawCanMsg->flags = 0;
#if CFG_CANHW_STAMP
		{
			struct timeval tv;
			ioctl(priv->sd, SIOCGSTAMP, &tv);
			rawCanMsg->extra.time.us = tv.tv_sec * 1000000 + tv.tv_usec;
		}
#endif

		// doesn't seem to work..
		//if (msg.msg_flags & MSG_CONFIRM)
		//	info("MSG_CONFIRM");

		// don't know..
		//if (msg.msg_flags & MSG_DONTROUTE)
		//	info("MSG_DONTROUTE");

		if (rawCanMsg->length > 8)
			rawCanMsg->length = 8;

		if (frame.can_id & CAN_EFF_FLAG)
			rawCanMsg->flags |= VE_CAN_EXT;
		else
			rawCanMsg->flags |= VE_CAN_STD;

		// Remote request
		if (frame.can_id & CAN_RTR_FLAG)
			rawCanMsg->flags |= VE_CAN_RTR;

		memcpy(rawCanMsg->mdata, frame.data, rawCanMsg->length);
		return veTrue;
	}

	return veFalse;
}

/* sending a raw CAN bus message */
static veBool sendMsg(struct VeCanGatewayS* gw, VeRawCanMsg *rawCanMsg)
{
	GatewayPrivate *priv = (GatewayPrivate *) gw->ctx;
	struct can_frame frame;
	int ret;

	veAssert(rawCanMsg->length <= 8);

	memcpy(frame.data, rawCanMsg->mdata, 8);
	frame.can_dlc = rawCanMsg->length;
	frame.can_id = rawCanMsg->canId;

	if (rawCanMsg->flags & VE_CAN_EXT)
		frame.can_id |= CAN_EFF_FLAG;

	if (rawCanMsg->flags & VE_CAN_RTR)
		frame.can_id |= CAN_RTR_FLAG;

	ret = write(priv->sd, &frame, sizeof(frame));
	if (ret == -1) {
		if (errno != ENOBUFS)
			error("write failed: %d %d", ret, errno);
		return veFalse;
	}

	return veTrue;
}

static size_t gwId(VeCanGateway* gw, char *buf, size_t len)
{
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;

	return ve_snprintf(buf, len, "socketcan:%s", priv->dev);
}

static size_t gwDesc(VeCanGateway* gw, char *buf, size_t len)
{
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;

	return ve_snprintf(buf, len, "%s", priv->dev);
}

static VeCanRxEventType gwEventType(VeCanGateway* gw, VeCanRxEventData *data)
{
	GatewayPrivate* priv = (GatewayPrivate*) gw->ctx;

	data->fd = priv->sd;
	return VE_CAN_RX_EV_FD;
}

/* Get memory and setup a new socket CAN gateway */
static VeCanGateway* allocGateway(char *dev)
{
	VeCanGateway *gw = NULL;
	GatewayPrivate *priv;

	gw = calloc(1, sizeof(VeCanGateway));
	priv = calloc(1, sizeof(GatewayPrivate));
	priv->dev = strdup(dev);
	gw->ctx = priv;

	gw->flags = VE_CAN_GW_CAN | VE_CAN_GW_LOOPBACK;

	gw->openFun = open;
	gw->bitrateFun = setBitrate;
	gw->busOnFun = busOn;
	gw->readStub.fun = readMsg;
	gw->sendStub.fun = sendMsg;
	gw->idFun = gwId;
	gw->descFun = gwDesc;
	gw->rxType = gwEventType;

	return gw;
}

/* Add a gateway for all interfaces which might be CAN bus interfaces */
static void enumerate(void)
{
	struct ifaddrs *ifaddr, *ifa;
	VeCanGateway* gw;

	if (getifaddrs(&ifaddr) == -1) {
		error("getifaddrs");
		return;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		/* skip the devices with ethernet info */
		if (ifa->ifa_addr) {
			info("ignoring %s", ifa->ifa_name);
			continue;
		}

		if ((ifa->ifa_flags & IFF_UP) == 0) {
			info("ignoring %s, interface is not up",  ifa->ifa_name);
			continue;
		}

		if ((gw = allocGateway(ifa->ifa_name)))
			veCanDrvNewGw(&canDriver, gw);
	}

	freeifaddrs(ifaddr);
}

VeCanDriver* veCanSkRegister(void)
{
	canDriver.id = "socketcan";
	canDriver.enumFun = enumerate;

	return &canDriver;
}

