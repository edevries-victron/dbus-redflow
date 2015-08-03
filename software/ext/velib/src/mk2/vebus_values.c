#include <velib/base/base.h>
#include <velib/mk2/vebus_device.h>
#include <velib/mk2/vebus_values.h>

/** @note this file should not include vebusDevice / vebusSystem etc */

#define X(a,b,c) c,
static VebusUnit const settingUnits[] = {
	VEBUS_SETTINGS
};

static VebusUnit const varUnits[] = {
	VEBUS_RAMVARS
};

static VebusUnit const flagUnits[] = {
	VEBUS_FLAGS
};

static VebusUnit const systemUnits[] = {
	VEBUS_SYSTEM
};

#undef X

static VebusUnit vebusGetSettingUnit(un8 id)
{
	if (id >= ARRAY_LENGTH(settingUnits))
		return VE_U_INVALID;

	return settingUnits[id];
}

VebusUnit vebusGetFlagUnit(un8 id)
{
	if (id >= ARRAY_LENGTH(flagUnits))
		return VE_U_INVALID;
	return flagUnits[id];
}

static VebusUnit vebusGetRamvarUnit(un8 id)
{
	if (id >= ARRAY_LENGTH(varUnits))
		return VE_U_INVALID;

	return varUnits[id];
}

static VebusUnit vebusGetSystemUnit(un8 id)
{
	if (id >= ARRAY_LENGTH(systemUnits))
		return VE_U_INVALID;

	return systemUnits[id];
}

static VebusUnit vebusGetUnitUnmasked(VebusValue nr)
{
	typedef VebusUnit (*F)(un8 id);
	F f[] = {vebusGetSettingUnit, vebusGetFlagUnit, vebusGetRamvarUnit, vebusGetSystemUnit};
	VebusValueType tp = vebusGetValueType(nr);

	if (tp >= ARRAY_LENGTH(f))
		return VE_U_INVALID;
	return f[tp]((un8) nr);
}

VebusUnit vebusGetUnit(VebusValue nr, VeUnitSet units)
{
	VebusUnit unit = vebusGetUnitUnmasked(nr);

	if (unit == VE_U_INVALID)
		return VE_U_INVALID;
	unit &= VE_U_MASK;

	if (units == VE_UNITS_MULTI) {
		switch (unit)
		{
		case VE_U_SEC:
			return VE_U_MIN;
		case VE_U_W:
			return VE_U_A_AC;
		/* values are reported in shifted Joules, gui typically want kWh */
		case VE_U_KWH:
			return VE_U_J16;
		default:
			;
		}
	}
	return unit;
}

/* returns the phase the value belongs to, 0 if none. otherwise 1,2,3 */
un8 vebusGetPhase(VebusValue nr)
{
	VebusUnit unit = vebusGetUnitUnmasked(nr);

	if (unit == VE_U_INVALID)
		return 0xFF;

	return unit >> 14;
}

un8 vebusGetAcIn(VebusValue nr)
{
	VebusUnit unit = vebusGetUnitUnmasked(nr);

	if (unit == VE_U_INVALID)
		return 0xFF;

	return (unit >> 11) & 7;
}

/* Gets the sid which contains the flag */
un8 vebusGetSettingIdFromFlag(un8 id)
{
	un8 const flags[] = {VE_FLAGS_0, VE_FLAGS_1, VE_FLAGS_2, VE_FLAGS_3};
	un8 flag = id >> 4;

	if (flag >= 4)
		return VEBUS_ID_INVALID;
	return flags[flag];
}

VebusValueType vebusGetValueType(un16 nr)
{
	un8 ret = nr >> 8;
	return (ret >= VEBUS_TP_INVALID ? VEBUS_TP_INVALID : ret);
}

/*
 * The CAN bus devices use a hexdecimal number for version.
 * To display the version number correctly to the user in the CAN
 * bus tools, it is converted to bcd.
 */
un32 mk2VersionToBcd(un32 version)
{
	un8 v1, v2, v3;

	v1 = (version / 100) % 10;
	v2 = (version / 10) % 10;
	v3 = version % 10;

	return (v1 << 8) | (v2 << 4) | v3;
}

un32 mk2VersionToProductBcd(un32 version)
{
	un8 v1, v2, v3, v4;

	version /= 1000;
	v1 = (version / 1000) % 10;
	v2 = (version / 100) % 10;
	v3 = (version / 10) % 10;
	v4 = version % 10;

	return (v1 << 12) | (v2 << 8) | (v3 << 4) | v4;
}

/* Returns veTrue if setting change requires reset of the device */
veBool vebusSettingChangeRequiresReset(struct VebusVersion version, un8 id)
{
	switch (id) {
		case VE_CURRENT_FOR_SWITCHING_TO_AES:
		case VE_HYST_FOR_AES_CURRENT:
		case VE_BATTERY_CAPACITY:
			return veTrue;
		case VE_BATTERY_CHARGE_PERCENTAGE:
			if ((version.number % 1000) < 200)
				return veTrue;
	}

	return veFalse;
}
