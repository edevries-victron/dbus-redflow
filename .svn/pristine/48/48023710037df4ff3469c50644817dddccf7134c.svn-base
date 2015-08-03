
#include <velib/velib_inc_mk2.h>
#include <velib/mk2/mk2.h>
#include <velib/mk2/mk2_info.h>
#include <velib/mk2/vebus_device.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_logger.h>

/*
 * The multi does report raw values. Scaling information is needed to transform these into
 * normal units. The scaling might differ between devices, but is fixed for a certain
 * firmware version of a certain device. The scale also determines if a setting / value is
 * supported, scale == 0 means not supported. Note: scaling factors need to be interpreted,
 * which is different for settings and ram vars (run time values). Optionally, by defining
 * CFG_VEBUS_SCALES_ARE_STORED, these scales can be stored, allowing faster startup.
 * Obviously functions must be provided where to store them, mk2_info_files.c is an example
 * to just store them on a filesystem.
 *
 * note: only one device per vebus system at a time!
 */

#define N_RAM_VAR			ARRAY_LENGTH(vebusSystem.devices[0].ramInfo)
#define N_SETTINGS			ARRAY_LENGTH(vebusSystem.devices[0].settingInfo)

static void initDone(void)
{
	logI("vebus_startup", "settings scale obtained");
	vebusSystem.init.dev->validScales = veTrue;
	if (vebusSystem.init.cb) {
		Mk2Callback* cb = vebusSystem.init.cb;
		vebusSystem.init.cb = NULL;
		cb();
	}
}

/* Start obtaining the settings scale or load the cached values */
static void getNextSettingInfo(void)
{
	struct VebusDeviceS* dev = vebusSystem.init.dev;

	if (vebusSystem.init.id >= N_SETTINGS) {
#if CFG_VEBUS_SCALES_ARE_STORED
		mk2InfoSettingsStore(dev->version.number, dev->settingInfo, N_SETTINGS);
#endif
		initDone();
		return;
	}

	vebusGetSettingInfo(dev, vebusSystem.init.id, getNextSettingInfo, &dev->settingInfo[vebusSystem.init.id]);
	vebusSystem.init.id++;
}

/* Start obtaining the settings scale or load the cached values */
static void getSettings(void)
{
#if CFG_VEBUS_SCALES_ARE_STORED
	VebusDevice* dev = vebusSystem.init.dev;

	if (mk2InfoSettingsLoad(dev->version.number, dev->settingInfo, N_SETTINGS))
	{
		logI("vebus_startup", "using stored settings scale info");
		initDone();
		return;
	}
#endif

	vebusSystem.init.id = 0;
	getNextSettingInfo();
}

/* Get the next RAM var scale of start with the settings scale */
static void getNextRamInfo(void)
{
	struct VebusDeviceS* dev = vebusSystem.init.dev;

	if (vebusSystem.init.id >= N_RAM_VAR) {
#if CFG_VEBUS_SCALES_ARE_STORED
		mk2InfoRamStore(dev->version.number, dev->ramInfo, N_RAM_VAR);
#endif
		getSettings();
		return;
	}

	vebusGetRamInfo(dev, vebusSystem.init.id, getNextRamInfo, &dev->ramInfo[vebusSystem.init.id]);
	vebusSystem.init.id++;
}

/* Start with downloading all RAM var scales or load locally cached values */
static void getRamInfo(void)
{
#if CFG_VEBUS_SCALES_ARE_STORED
	VebusDevice* dev = vebusSystem.init.dev;

	if (mk2InfoRamLoad(dev->version.number, dev->ramInfo, N_RAM_VAR))
	{
		logI("vebus_startup", "using stored ram scale info");
		getSettings();
		return;
	}
#endif

	vebusSystem.init.id = 0;
	getNextRamInfo();
}

/*
 * Get the settings and ram vars scales for a device.
 *
 * Note only 1 device per vebusSystem at a time.
 * (parallel download would need address change messages all the time, making it even slower!).
 * The callback will be called when all are obtained.
 */
void vebusQueryDevice(struct VebusDeviceS* dev, VeUnitSet units, Mk2Callback* callback)
{
	veAssert(vebusSystem.init.cb == NULL);

	vebusSystem.init.cb = callback;
	vebusSystem.init.dev = dev;
	vebusSystem.units = units;
	dev->resetRequired = veFalse; /* init */
	/*
	 * Multi firmware versions >= 308 return resetRequired with version frame remainder and
	 * with settings read/write commands
	 */
	vebusGetTargetVersion(dev, getRamInfo, &dev->version);
}
