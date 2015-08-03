#ifndef _VELIB_IO_REMOTE_DEVICE_VECAN_H_
#define _VELIB_IO_REMOTE_DEVICE_VECAN_H_

#include <velib/base/base.h>
#include <velib/io/ve_remote_device.h>
#include <velib/utils/ve_events.h>
#include <velib/vecan/regs.h>

typedef struct VeCanRemoteDev {
	VeRemoteDev remoteDev;
	un8 *remoteNad;
	un8 vupId;
} VeCanRemoteDev;

void veCanRemoteDevInit(VeCanRemoteDev *tunnel, un8 *nad, un8 id, VeRemoteDevRole role);
void veCanRemoteDevRegHandler(VeEvent ev);

#endif
