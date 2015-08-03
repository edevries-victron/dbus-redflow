#include <string.h>

#include <velib/velib_inc_mk2.h>
#include <velib/mk2/vebus_device.h>

static VebusDevice devices[CFG_VEBUS_DEVICES];
static VebusDevice* freeList;

void vebusDeviceAllocInit(void)
{
	un8 n;

	for (n = 0; n < CFG_VEBUS_DEVICES; n++) {
		devices[n].next = freeList;
		freeList = &devices[n];
	}
}

VebusDevice* vebusDeviceAlloc(void)
{
	VebusDevice* ret = freeList;

	if (!ret)
		return NULL;

	freeList = ret->next;
	memset(ret, 0, sizeof(VebusDevice));

	return ret;
}

void vebusDeviceFree(VebusDevice* dev)
{
	dev->next = freeList;
	freeList = dev;
}
