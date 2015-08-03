#ifndef _VELIB_IO_VE_TUNNEL_H_
#define _VELIB_IO_VE_TUNNEL_H_

#include <velib/io/ve_stream.h>
#include <velib/types/variant.h>
#include <velib/vecan/regs.h>

#define VE_REMOTE_DEV_EV_OPEN					0x00
#define VE_REMOTE_DEV_EV_DATA_TO_DEV			0x01
#define VE_REMOTE_DEV_EV_CLOSE					0x02
#define VE_REMOTE_DEV_EV_ERROR					0x03


#define VE_REMOTE_DEV_TO_CLIENT					0x80

#define VE_REMOTE_DEV_EV_OPENED					(VE_REMOTE_DEV_EV_OPEN | VE_REMOTE_DEV_TO_CLIENT)
#define VE_REMOTE_DEV_EV_DATA_FROM_DEV			(VE_REMOTE_DEV_EV_DATA_TO_DEV | VE_REMOTE_DEV_TO_CLIENT)
#define VE_REMOTE_DEV_EV_CLOSED					(VE_REMOTE_DEV_EV_CLOSE | VE_REMOTE_DEV_TO_CLIENT)


typedef un8 VeRemoteDevEv;
struct VeRemoteDev;

typedef void VeRemoteDevCb(struct VeRemoteDev *tunnel, VeRemoteDevEv ev, VeVariant *arg,
							un8 const *buf, un32 len);

typedef veBool VeRemoteDevPrepareOutputCb(struct VeRemoteDev *tunnel);
typedef veBool VeRemoteDevSendOutputCb(struct VeRemoteDev *tunnel);

typedef enum {
	VE_REMOTE_DEV_STATE_NONE,
	VE_REMOTE_DEV_STATE_CLOSED,
	VE_REMOTE_DEV_STATE_OPENING,
	VE_REMOTE_DEV_STATE_OPENED
} VeRemoteDevState;

#define VE_REMOTE_DEV_ROLE_SERVICE		0x00
#define VE_REMOTE_DEV_ROLE_CLIENT		0x01
#define VE_REMOTE_DEV_ROLE_PROXY		0x02

#define VE_REMOTE_DEV_ROLE_SERVICE_PROXY	(VE_REMOTE_DEV_ROLE_SERVICE | VE_REMOTE_DEV_ROLE_PROXY)
#define VE_REMOTE_DEV_ROLE_CLIENT_PROXY		(VE_REMOTE_DEV_ROLE_CLIENT | VE_REMOTE_DEV_ROLE_PROXY)
typedef un8 VeRemoteDevRole;


typedef struct VeRemoteDev {
	VeRemoteDevState state;
	VeRemoteDevRole role;
	VeRemoteDevPrepareOutputCb *prepareOutStream;
	VeRemoteDevSendOutputCb *sendOutStream;

	VeRemoteDevCb *appCb;
	void *appCtx;
} VeRemoteDev;

/* Client functions */
veBool veRemoteDevOpen(VeRemoteDev *dev);
void veRemoteDevClose(VeRemoteDev *dev);
un32 veRemoteDevDataToDevice(VeRemoteDev *dev, un8 const *buf, un32 len);

/* Device functions */
veBool veRemoteDevOpenResponse(VeRemoteDev *dev, VeRegAckCode code);
un32 veRemoteDevDataFromDevice(VeRemoteDev *dev, un8 const *buf, un32 len);
veBool veRemoteDevCloseResponse(VeRemoteDev *dev);

/* Tunnel */
void veRemoteDeviceCreateTunnel(VeRemoteDev *service, VeRemoteDev *client);

/* Common */
void veRemoteDevSetCallback(VeRemoteDev *dev, VeRemoteDevCb *callback, void *ctx);

#endif
