#include <math.h>

#include <velib/velib_inc_mk2.h>

#include <velib/base/base.h>
#include <velib/base/ve_string.h>
#include <velib/mk2/mk2_strings.h>
#include <velib/mk2/vebus_device.h>
#include <velib/mk2/values_float.h>
#include <velib/nmea2k/n2k.h>
#include <velib/types/variant_print.h>

#define X(a,b,c) b,
char const* settingName[] = {
	VEBUS_SETTINGS
};

const char* winmonVarName[] = {
	VEBUS_RAMVARS
};

char const* flagName[] = {
	VEBUS_FLAGS
};

char const* systemName[] = {
	VEBUS_SYSTEM
};
#undef X

static VeVariantUnitFmt fmt[] = {
	/* VE_U_NR */			{0, ""},
	/* VE_U_V_DC */			{2, "V"},
	/* VE_U_A_DC */			{1, "A"},
	/* VE_U_V_AC */			{0, "V"},
	/* VE_U_A_AC */			{1, "A"},
	/* VE_U_W */			{0, "W"},
	/* VE_U_VA */			{0, "VA"},
	/* VE_U_HZ */			{1, "Hz"},
	/* VE_U_MSEC */			{0, "ms"},
	/* VE_U_DSEC */			{0, "ds"},
	/* VE_U_SEC */			{0, "s"},
	/* VE_U_MIN */			{0, "m"},
	/* VE_U_PREC */			{1, "%"},
	/* VE_U_AH */			{0, "Ah"},
	/* VE_U_J16 */			{0, "2^16 J"},
	/* VE_U_KWH */			{2, "kWh"},
	/* VE_U_STR */			{0, ""},
	/* VE_U_FLAG */			{0, ""},
	/* VE_U_DFLAG */		{0, ""},
	/* VE_U_FLAGS */		{0, ""},
	/* VE_U_PREC_FRAC */	{0, ""},
	/* VE_U_SELECT */		{0, ""}
};
#undef L
#undef F

char const* vebusGetFlagName(un8 nr)
{
	if (nr >= ARRAY_LENGTH(flagName))
		return "unknown";
	return flagName[nr];
}

char const* vebusGetSettingName(un8 nr)
{
	if (nr >= ARRAY_LENGTH(settingName))
		return "unknown";
	return settingName[nr];
}

char const* vebusGetRamVarName(un8 nr)
{
	if (nr >= ARRAY_LENGTH(winmonVarName))
		return "unknown";
	return winmonVarName[nr];
}

char const* vebusGetSystemVarName(un8 nr)
{
	if (nr >= ARRAY_LENGTH(systemName))
		return "unknown";
	return systemName[nr];
}

char const* vebusGetName(VebusValue nr)
{
	typedef char const *(*F)(un8 nr);
	VebusValueType tp = vebusGetValueType(nr);
	F const f[] = {
		vebusGetSettingName,
		vebusGetFlagName,
		vebusGetRamVarName,
		vebusGetSystemVarName
	};
	return (tp < ARRAY_LENGTH(f) ? f[tp](nr) : "unknown");
}

/**
 * Place a textual description of the value of the variant into
 * the buf of size len, with the corresponding unit.
 */
size_t vebusFormatValue(VebusUnit unit, VeVariant *var, char* buf, size_t len)
{
	switch (unit)
	{
	case VE_U_FLAGS:
		return ve_snprintf(buf, len, "0x%04X", var->value.UN32);
	case VE_U_MIN:
		{
			float value = (float) var->value.UN32;
			int min = (value < 0 ? (unsigned int) (value - 0.5) : (unsigned int) (value + 0.5));
			int sec = (int) ((value - min) * 60);
			if (sec != 0)
				return ve_snprintf(buf, len, "%s%d:%02um", sec < 0 ? "-" : "", min, (sec < 0 ? -sec : sec));
			else
				return ve_snprintf(buf, len, "%s%dm", sec < 0 ? "-" : "", min);
		}
		break;
	case VE_U_DSEC:
		return ve_snprintf(buf, len, "%.1fms", var->value.Float * 100);
	case VE_U_INVALID:
		return ve_snprintf(buf, len, "%s", "invalid");
	case VE_U_PERC_FRAC:
		return ve_snprintf(buf, len, "%.1f%%", var->value.Float * 100);
	case VE_U_PERC:
		return ve_snprintf(buf, len, "%.1f%%", var->value.Float);
	default:
		return veVariantFmt(var, &fmt[unit], buf, len);
	}
}

/// returns a description of the vebusError
char const* led_vebusErrorString(VebusErrorCode vebusError)
{
	switch (vebusError)
	{
	case VEBUS_ERROR_NONE:
		return "no error";
	case VEBUS_ERROR_PHASE_MISSING:
		return "Device is switched off because one of the other phases in the system has switched off";
	case VEBUS_ERROR_INCORRECT_DEV_NUMBER:
		return "Not all or more than the expected devices were found in the system";
	case VEBUS_ERROR_ONLY_ONE_DEV:
		return "No other device whatsoever detected";
	case VEBUS_ERROR_AC_OUT_OVERVOLTAGE:
		return "Overvoltage on AC-out";
	case VEBUS_ERROR_SYNC:
		return "System time synchronisation problem occurred";
	case VEBUS_ERROR_SENDING:
		return "Device cannot transmit data";
	case VEBUS_ERROR_NO_DONGLE:
		return "Dongle is not connected";
	case VEBUS_ERROR_NEW_MASTER:
		return "One of the devices has assumed master status because the original master failed";
	case VEBUS_ERROR_OVERVOLTAGE:
		return "Overvoltage has occurred";
	case VEBUS_ERROR_ACT_AS_SLAVE:
		return "This device cannot function as slave";
	case VEBUS_ERROR_SWITCH_OVER:
		return "Switch-over system protection initiated";
	case VEBUS_ERROR_FIRMWARE_INCOMPATIBLE:
		return "Firmware incompatibility, the connected devices cannot operate together";
	case VEBUS_ERROR_INTERNAL_ERROR:
		return "Internal error";
	}
	return "";
}

#if CFG_MK2
/**
 * Return a textual representation of a setting with id, and raw value
 * into buf of max size len.
 */
void vebusFormatRawSettingValue(struct VebusDeviceS* dev, un8 id, un16 value, char* buf, size_t len)
{
	VeVariant var;
	veVariantFloat(&var, mk2UnpackSettingFloat(value, dev, id, vebusSystem.units));
	vebusFormatValue(vebusGetUnit(id, vebusSystem.units), &var, buf, len);
}

void vebusFormatRawVariableValue(struct VebusDeviceS* dev, un8 id, un16 value, char* buf, size_t len)
{
	VeVariant var;
	veVariantFloat(&var, mk2UnpackRamFloat(value, dev, id));
	vebusFormatValue(vebusGetUnit(id + VEBUS_RAMVAR_OFFSET, vebusSystem.units), &var, buf, len);
}
#endif


/* Textual version of the state (vebusSystem.convState.state) as reported by the multi */
size_t vebusStateText(int state, char* buf, size_t len)
{
	switch (state)
	{
	case VE_STATE_DOWN:
		return ve_snprintf(buf, len, "%s", "down");
	case VE_STATE_STARTUP:
		return ve_snprintf(buf, len, "%s", "startup");
	case VE_STATE_OFF:
		return ve_snprintf(buf, len, "%s", "off");
	case VE_STATE_SLAVE:
		return ve_snprintf(buf, len, "%s", "slave");
	case VE_STATE_INVERT_FULL:
	case VE_STATE_INVERT_HALF:
	case VE_STATE_INVERT_AES:
		return ve_snprintf(buf, len, "%s", "invert");
	case VE_STATE_POWER_ASSIST:
		return ve_snprintf(buf, len, "%s", "power assist");
	case VE_STATE_BYPASS:
		return ve_snprintf(buf, len, "%s", "by pass");
	case VE_STATE_CHARGE:
		return ve_snprintf(buf, len, "%s", "charge");
	case VE_STATE_UNKNOWN:
		return ve_snprintf(buf, len, "%s", "unknown");
	}

	return 0;
}

/* Textual version of the combined converter state as reported on the NMEA 2000 bus */
size_t vebusNmea2000StateText(int state, char* buf, size_t len)
{
	switch (state)
	{
	case N2K_CONVERTER_STATE_OFF:
		return ve_snprintf(buf, len, "%s", "off");
	case N2K_CONVERTER_STATE_LOW_POWER_MODE:
		return ve_snprintf(buf, len, "%s", "low power mode");
	case N2K_CONVERTER_STATE_FAULT:
		return ve_snprintf(buf, len, "%s", "fault");
	case N2K_CONVERTER_STATE_BULK:
		return ve_snprintf(buf, len, "%s", "bulk");
	case N2K_CONVERTER_STATE_ABSORPTION:
		return ve_snprintf(buf, len, "%s", "absorption");
	case N2K_CONVERTER_STATE_FLOAT:
		return ve_snprintf(buf, len, "%s", "float");
	case N2K_CONVERTER_STATE_STORAGE:
		return ve_snprintf(buf, len, "%s", "storage");
	case N2K_CONVERTER_STATE_EQUALIZE:
		return ve_snprintf(buf, len, "%s", "equalize");
	case N2K_CONVERTER_STATE_PASSTHRU:
		return ve_snprintf(buf, len, "%s", "passthru");
	case N2K_CONVERTER_STATE_INVERTING:
		return ve_snprintf(buf, len, "%s", "inverting");
	case N2K_CONVERTER_STATE_ASSISTING:
		return ve_snprintf(buf, len, "%s", "assisting");
	case N2K_CONVERTER_STATE_PSU:
		return ve_snprintf(buf, len, "%s", "power supply");
	case N2K_CONVERTER_STATE_UNAVAILABLE:
		return ve_snprintf(buf, len, "%s", "unavailable");
	}

	return 0;
}
