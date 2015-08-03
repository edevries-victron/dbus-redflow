/**
 * The items is a tree of the available values related to the vebus system and
 * the devices in it. This is optional, the intention is to provide a common
 * access to the values and hide the implementation details of the mk2 / vebus.
 */

#include <velib/velib_inc_mk2.h>
#include <velib/base/ve_string.h>
#include <velib/mk2/vebus_items.h>
#include <velib/mk2/vebus_values.h>
#include <velib/mk2/mk2_strings.h>
#include <velib/types/ve_item.h>

/* textual representation of a vebus value */
static size_t textVersion(VeVariant *var, void const *ctx, char *buf, size_t len)
{
	VeItem *item = (VeItem *) ctx;
	un16 nr = veItemCtx(item)->value;

	if (veVariantIsValid(var)) {
		VebusUnit unit = vebusGetUnit(nr, VE_UNITS_VSC);
		if (unit == VE_U_PERC_FRAC) {
			VeVariant perc = *var;
			perc.value.Float /= 100;
			return vebusFormatValue(unit, &perc, buf, len);
		}

		return vebusFormatValue(unit, var, buf, len);
	}

	return ve_snprintf(buf, len, "%s", "");
}

size_t getDescription(struct VeItem *item, char *buf, size_t len)
{
	return ve_snprintf(buf, len, "%s", vebusGetName(veItemCtx(item)->value));
}

/* create a item corresponding to a vebus value */
static void addItem(VeItem *root, un16 nr, char const *uid, VeItem *item)
{
	veItemAddChildByUid(root, uid, item);
	veItemSetFmt(item, textVersion, item);
	veItemSetGetDescr(item, getDescription);
	item->variant.type.tp = VE_FLOAT;
	veItemInvalidate(item);

	veItemCtx(item)->value = nr;
}

#ifdef CFG_VEBUS_DEVICE_ITEMS

static veBool handleSettingChange(struct VeItem *item, void *ctx, VeVariant *variant)
{
	VebusDevice *dev = (VebusDevice *) ctx;

	if (!dev)
		return veFalse;

	veVariantToFloat(variant);
	if (!vebusLocalSetSetting(dev, veItemCtx(item)->value, variant->value.Float))
		return veFalse;

	mk2ActSetRawSetting(dev, veItemCtx(item)->value);

	return veTrue;
}

static void handleGetSetting(struct VeItem *item)
{
	VebusDevice *dev = (VebusDevice *) veItemSetCtx(item);

	if (!dev)
		return;

	mk2ActGetRawSetting(dev, veItemCtx(item)->value);
}

/* create the item and also register its callbacks */
static void addSetting(struct VeItem *root, un16 nr, char const *uid, struct VeItem *item)
{
	addItem(root, nr, uid, item);
	veItemSetSetter(item, handleSettingChange, NULL);
	veItemSetGetter(item, handleGetSetting);
}

static veBool handleFlagChange(struct VeItem *item, void *ctx, VeVariant *variant)
{
	VebusDevice *dev = (VebusDevice *) ctx;

	if (!dev)
		return veFalse;

	veVariantToN32(variant);
	if (!vebusLocalSetFlag(dev, veItemCtx(item)->value, variant->value.UN32 != 0))
		return veFalse;

	mk2ActSetFlag(dev, veItemCtx(item)->value);

	return veTrue;
}

static void handleGetFlag(struct VeItem *item)
{
	VebusDevice *dev = (VebusDevice *) veItemSetCtx(item);

	if (!dev)
		return;

	mk2ActGetRawFlag(dev, veItemCtx(item)->value);
}

/* create the item and also register its callbacks */
static void addFlag(VeItem* root, un16 nr, char const *uid, VeItem *item)
{
	addItem(root, nr, uid, item);
	veItemSetSetter(item, handleFlagChange, NULL);
	veItemSetGetter(item, handleGetFlag);
}


/* Create the device specific values. */
void vebusItemsCreateDevice(VeItem *root, VeBusItems *sys)
{
	VeItem* settings = veItemGetOrCreateUid(root, "Devices/0/Settings");

#define X(a,b)	addSetting(settings, a, b, &sys->devices[0].settings[a]);
	VE_DEVICE_SETTINGS
#undef X

#define X(a,b)	addFlag(settings, a, b, &sys->devices[0].flags[(un8) a]);
	VE_DEVICE_FLAGS
#undef X
}

#else

void vebusItemsCreateDevice(VeItem *root, VeBusItems *sys)
{
	VE_UNUSED(root);
	VE_UNUSED(sys);
}

#endif

/* Performs unit conversion and creates on change event for the item tree */
void updateItem(VebusDevice *device, VeItem *item)
{
	VeVariant value;
	VebusValue nr = (VebusValue) veItemCtx(item)->value;

	vebusGetValue(device, nr, &value, VE_UNITS_VSC);

	/* [0..1] to [0..100] range conversion */
	if (nr == VE_VAR_SOC && veVariantIsValid(&value))
		value.value.Float *= 100;

	veItemOwnerSet(item, &value);
}

/**
 * Create an item tree with vebus values.
 *
 * The tree with items can be used to navigate through the values in
 * vebus system, format / set / get them etc.
 */
void vebusItemsCreateSystem(VeItem *root, VeBusItems *sys)
{
#define X(a,b)	addItem(root, a, b, &sys->a);
	VE_SYSTEM_CREATE_ITEMS
#undef X

	updateItem(NULL, &sys->VE_SYS_DMC_VERSION);
	updateItem(NULL, &sys->VE_SYS_BMS_VERSION);
}

#define X(a,b)	updateItem(dev, &sys->a);

void vebusItemsActionDone(VeBusItems *sys, VebusDevice *dev, Mk2Actions done)
{
	switch (done)
	{
	case MK2_ACT_AC_CONFIG:
		{
			/*
			 * The active AC in min / max is only reported once in use.
			 * So keep a copy around.
			 */
			un8 activeIn = vebusActiveIn(&vebusSystem.acInConfig);
			if (activeIn < ARRAY_LENGTH(vebusSystem.acInConfigPerAcIn))
				vebusSystem.acInConfigPerAcIn[activeIn] = vebusSystem.acInConfig;
		}
		VE_AC_CONFIG_VALUES
		break;
	case MK2_ACT_STATUS:
		/* update the alarms derived from the leds and state */
		vebusSystem.alarms = vebusAlarmsFromState(&vebusSystem.state);
		VE_SYSTEM_ALARMS
		VE_SYSTEM_STATE
		VE_SYSTEM_LEDS
		break;
	case MK2_ACT_CONV_STATE:
		VE_SYSTEM_CONV_VARS_VEBUS
		break;
	case MK2_ACT_DC_VALUES:
		VE_SYSTEM_DC_VARS
		break;
	case MK2_ACT_AC_VALUES:
		VE_SYSTEM_AC_ITEMS
		break;
	case MK2_ACT_GET_KWH_COUNTERS:
		VE_SYSTEM_KWH_COUNTERS
		break;
	case MK2_ACT_GET_ASSISTANT_VALUES:
		VE_SYSTEM_AC_SENSORS
		break;
	default:
		;
	}
}

#undef X

#ifdef CFG_VEBUS_DEVICE_ITEMS

/* update all flags at once */
#define X(a,b)	updateItem(dev, &sys->devices[0].flags[(un8) a]);

void vebusItemsSettingChanged(VeBusItems *sys, struct VebusDeviceS* dev, un8 id)
{
	if (dev->addr != 0)
		return;

	if (id == VE_FLAGS_0 || id == VE_FLAGS_1 || id == VE_FLAGS_2 || id == VE_FLAGS_3) {
		VE_DEVICE_FLAGS
		return;
	}

	updateItem(dev, &sys->devices[0].settings[id]);
}

#undef X

#else

void vebusItemsSettingChanged(VeBusItems *sys, struct VebusDeviceS* dev, un8 id)
{
	VE_UNUSED(sys);
	VE_UNUSED(dev);
	VE_UNUSED(id);
}

#endif

void vebusItemsRamVarObtained(VeBusItems *sys, struct VebusDeviceS* dev, un8 id)
{
	/* The SOC of the master is the SOC of the system */
	if (dev->addr == 0 && id == VE_VAR_SOC_RAMVAR)
		updateItem(dev, &sys->VE_VAR_SOC);

}

static void attachDevice(struct VeItem *item, void *ctx)
{
	veItemSetSetter(item, NULL, ctx);
}

void vebusItemsInitDone(VeBusItems *sys, VeItem *root, VebusDevice *masterDevice)
{
	VE_UNUSED(root);

#ifdef CFG_VEBUS_DEVICE_ITEMS
	un8 n;

	veItemForeach(veItemByUid(root, "Devices/0/"), attachDevice, masterDevice);

	for (n = 0; n < VEBUS_SETTINGS_COUNT; n++) {
		VeVariant var;

		veVariantFloat(&var, vebusGetSettingMaximum(masterDevice, n, VE_UNITS_VSC));
		veItemSetMax(&sys->devices[0].settings[n], &var);

		veVariantFloat(&var, vebusGetSettingMinimum(masterDevice, n, VE_UNITS_VSC));
		veItemSetMin(&sys->devices[0].settings[n], &var);
	}
#endif

/* Set info about the vebus system */
#define X(a,b)	updateItem(masterDevice, &sys->a);
	VE_SYSTEM_GENERAL
	VE_SYSTEM_STORED_DEVICE_VERSIONS
#undef X

	updateItem(masterDevice, &sys->VE_SYS_MODE_IS_ADJUSTABLE);
}

#define X(a, b)	veItemInvalidate(&sys->a);
void vebusItemsInvalidate(VeBusItems *sys, VeItem *root)
{
	VE_SYSTEM_INVALIDATE_ITEMS

	veItemForeach(veItemByUid(root, "Devices/0/"), attachDevice, NULL);

	/* If the device is off, allow it to be turned on in case no BMS was detected */
	updateItem(NULL, &sys->VE_SYS_MODE_IS_ADJUSTABLE);
}
#undef X

#if 0
/**
 * @brief request a system variable by its id
 * @param id of the system variable
 * @return wether succesfull
 */
veBool mk2ReqSystemVar(un8 id)
{
#define X(a,b)	case a:

	switch (id)
	{
	VE_SYSTEM_DC_VARS
		mk2ActMark(MK2_ACT_DC_VALUES);
		return veTrue;

	VE_SYSTEM_STATE_VARS
		mk2ActMark(MK2_ACT_STATUS);
		return veTrue;

	VE_SYSTEM_CONV_VARS
		mk2ActMark(MK2_ACT_CONV_STATE);
		return veTrue;

	VE_SYSTEM_AC_VARS
		mk2ActMark(MK2_ACT_AC_VALUES);
		return veTrue;

	VE_SYSTEM_CURRENT_LIMIT
		mk2ActMark(MK2_ACT_AC_CONFIG);
		return veTrue;

	default:
		return veFalse;
	}

#undef X
}

#endif
