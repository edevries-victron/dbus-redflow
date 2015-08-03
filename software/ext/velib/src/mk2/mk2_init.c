#include <stdio.h>
#include <stdlib.h>

#include <velib/velib_inc_mk2.h>
#include <velib/base/base.h>
#include <velib/mk2/frame_handler.h>
#include <velib/mk2/mk2.h>
#include <velib/mk2/mk2_update.h>
#include <velib/mk2/vebus_device.h>
#include <velib/platform/console.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_logger.h>

/** create the vebusDevice corresponding to the devices found on the bus */
static void findNewDevice(void)
{
	un32 newId = vebusSystem.state.shortIds & ~vebusSystem.state.knownIds;
	un8 addr = 0;

	while (newId)
	{
		if (newId & 1) {
			VebusDevice* dev;
			if (!(dev = vebusDeviceAlloc()))
				return;

			dev->addr = addr;
			dev->next = vebusSystem.devices;
			vebusSystem.devices = dev;
			vebusSystem.state.knownIds |= (1 << addr);
			logI("vebus", "added device %d", dev->addr);
			mk2NewDeviceFound(dev);
		}

		addr++;
		newId >>= 1;
	}
}

static void mk2GetVebusStatusDone(void)
{
	logI("task", "short ids flags %X", vebusSystem.state.shortIds);

	findNewDevice();

	if (vebusSystem.init.cb) {
		Mk2Callback* cb = vebusSystem.init.cb;
		vebusSystem.init.cb = NULL;
		cb();
	}
}

static void mk2SendSetStateDone(void)
{
	/* Get the mask with addresses of the connected devices */
	vebusGetShortIds(mk2GetVebusStatusDone);
}

static void mk2SendSetState(void)
{
	mk2.state.sFlags = MK2_S_FLAG_AUTO_FORWARD_PANEL_FRAMES | MK2_S_FLAG_DO_NOT_SEND_STATE;
	mk2SetState(&mk2.state, mk2SendSetStateDone);
}

static void mk2GetVersionDone(void)
{
	if ((mk2.version % 1000) < 131) {
		logE("task", "mk2 firmware is too old, upgrade to at least 131", mk2.version);
		pltExit(100);
	}

	mk2SendSetState();
}

static void mk2GetStateDone(void)
{
	/* Check if the mk2 needs to be updated */
	mk2GetVersion(&mk2.version, mk2GetVersionDone);
}

/*
 * First step: lets see if the mk2 found a vebus device,
 * if not it needs to be restarted
 */
void mk2Start(Mk2Callback* cb)
{
	veAssert(vebusSystem.init.cb == NULL);

	vebusSystem.init.cb = cb;
	mk2GetState(&mk2.state, mk2GetStateDone);
}
