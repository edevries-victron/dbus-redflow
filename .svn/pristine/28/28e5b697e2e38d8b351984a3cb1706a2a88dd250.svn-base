#ifndef _VELIB_IO_VE_DEVICE_INTERNAL_H_
#define _VELIB_IO_VE_DEVICE_INTERNAL_H_

#include <velib/io/ve_remote_device.h>

void veRemoteDevInit(VeRemoteDev *tunnel, VeRemoteDevPrepareOutputCb *getOutput,
						VeRemoteDevSendOutputCb *cb, VeRemoteDevRole role);
void veRemoteDevTick(VeRemoteDev *dev);
void veRemoteDevRxData(VeRemoteDev *dev);

#endif
