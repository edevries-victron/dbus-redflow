#include <string.h>

#include <velib/io/ve_stream_n2k.h>
#include <velib/io/ve_remote_device.h>
#include <velib/io/ve_remote_device_vecan.h>
#include <velib/nmea2k/n2k_fp.h>
#include <velib/types/ve_str.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_logger.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/reg_rx.h>

#include "ve_remote_device_internal.h"

static VeCanRemoteDev *theTunnel;

static un8 getRemoteNad(VeCanRemoteDev *tunnel)
{
	if (tunnel->remoteDev.role & VE_REMOTE_DEV_ROLE_CLIENT)
		return *tunnel->remoteNad;
	return 0xFF;
}

static veBool prepareFp(VeRemoteDev *tunnel)
{
	VeCanRemoteDev *canTunnel = (VeCanRemoteDev *) tunnel;

	veRegPrepareFp(getRemoteNad(canTunnel), VE_REG_TUNNEL);
	if (n2kMsgOut == NULL) {
		logE("can_tunnel", "veTunnelPutBuf out of frames");
		return veFalse;
	}

	veOutUn8(canTunnel->vupId);

	return veTrue;
}

static veBool sendFp(VeRemoteDev *tunnel)
{
	veAssert(n2kStreamOut.error == veFalse);

	VE_UNUSED(tunnel);
	veOutSendFp(&veRegFpSeq);

	return n2kStreamOut.error;
}

static VeRemoteDev *findRemoteDevById(un8 id)
{
	VE_UNUSED(id);

	return (VeRemoteDev *) theTunnel;
}

/* this handles the reception of commands */
void veCanRemoteDevRegHandler(VeEvent ev)
{
	VeRemoteDev *tunnel;
	un8 tunnelId;

	if ((ev != VE_EV_VREG_RX_CMD && ev != VE_EV_VREG_RX) || veRegRxId != VE_REG_TUNNEL)
		return;

	tunnelId = veInUn8();
	tunnel = findRemoteDevById(tunnelId);
	if (!tunnel)
		return;

	veRemoteDevRxData(tunnel);
	veRegRxHandled = veTrue;
}

/**
 * Initialises a device to listen to a remote VE.Can device.
 *
 * note: clients need to supply _a pointer_ to the remote NAD. The location
 * should live as long as the tunnel. It is a pointer since the NAD _might_ change.
 * Services can simple pass NULL.
 */
void veCanRemoteDevInit(VeCanRemoteDev *tunnel, un8 *nad, un8 id, VeRemoteDevRole role)
{
	tunnel->remoteNad = nad;
	tunnel->vupId = id;
	theTunnel = tunnel;

	veRemoteDevInit(&tunnel->remoteDev, prepareFp, sendFp, role);
}
