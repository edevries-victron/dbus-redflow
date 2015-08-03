#ifndef _VELIB_MK2_MK2_REMOTE_DEVICE_H_
#define _VELIB_MK2_MK2_REMOTE_DEVICE_H_

#include <velib/io/ve_remote_device.h>
#include <velib/mk2/mk2.h>
#include <velib/utils/ve_events.h>

typedef void Mk2TakeOverRequestCb(VeRegAckCode code);

void mk2RemoteDevInit(VeRemoteDev *tunnel);
void mk2RemoteDevTick(void);
void mk2RemoteDevForwardFrame(Mk2MsgRx *mk2msg);
void mk2TakeOverRequest(Mk2TakeOverRequestCb* cb);

#endif
