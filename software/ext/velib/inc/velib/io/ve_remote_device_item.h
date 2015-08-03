#ifndef _VELIB_IO_VE_REMOTE_DEVICE_ITEM_H_
#define _VELIB_IO_VE_REMOTE_DEVICE_ITEM_H_

#include <velib/io/ve_remote_device.h>
#include <velib/types/ve_item.h>

typedef struct VeItemRemoteDev {
	VeRemoteDev remoteDev;
	VeItem *item;
	VeStream stream;
} VeItemRemoteDev;

void veItemRemoteDevInit(VeItemRemoteDev *dev, VeItem *item, VeRemoteDevRole role);

#endif
