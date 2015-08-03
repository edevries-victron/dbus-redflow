/**
 * As mentioned in e.g. Ve.Bus device / Ve.Bus system, values obtained from
 * vebus are in a raw format. Scaling info from the devices (see vebus device
 * init) is needed to turn this in to a meaningfull value. If resources are
 * limited direct conversion from int / int can be defined to prevent float
 * libraries from being pulled in. This implementation converts all values to
 * either floats. There are two units systems defined at the moment. One with
 * the units mentioned in the interface document, the other one are the units
 * as shown to the user in VE.configure (Which happens to be the format vsc
 * file are stored in).
 *
 * @note This file only gets / sets the correct values in the vebusDevice /
 * vebusSystem object. It does not do / queue any actual communication to
 * leave room for an application to decide in which order request and sets
 * etc are handled. There is a default implementation though, see mk2_actions.
 */

#include <velib/velib_inc_mk2.h>
#include <velib/base/ve_string.h>
#include <velib/mk2/values_float.h>
#include <velib/mk2/vebus_device.h>
#include <velib/types/types.h>
#include <velib/types/variant.h>
#include <velib/utils/ve_logger.h>
#include <velib/vecan/products.h>

/**
 * @brief Convert raw RAM var to a floating point representation
 * @param raw	value as recevied from vebus
 * @param dev	device the value came from
 * @param id	RAM id of the variable
 * @return		float representation
 */
float mk2UnpackRamFloat(sn32 raw, struct VebusDeviceS* dev, un8 id)
{
	VebusRamInfo *ramInfo = vebusGetRamInfoOfDevice(dev);
	VebusRamInfo const* info = &ramInfo[id];
	sn16 scale;
	float value;

	if (info->scale == 0)
		return 0;

	/* bit field support */
	if (info->offset == (sn16) 0x8000)
		return (raw & (1 << (info->scale - 1)) ? 1.0F : 0.0F);

	// this will make an normal unit of it..
	if (info->scale < 0) {
		raw = (sn16) raw;
		scale = -info->scale;
	} else
		scale = info->scale;

	value = (float) raw + info->offset;

	if (scale >= 0x4000)
	{
		scale = 0x8000 - scale;
		value /= scale;
	} else
	{
		logE("mk2ScaleRamFloat", "NOT SUPPORTED");
		value *= info->scale;
	}

	return value;
}

/** conversion for special frame from vebus_system.c */
void mk2UnpackDc(struct VebusDeviceS* dev, Mk2RawDc const* raw, Mk2DcF* out)
{
	out->V = mk2UnpackRamFloat(raw->V, dev, VE_VAR_UBAT_RAMVAR);
	out->I = mk2UnpackRamFloat(raw->chargerI, dev, VE_VAR_IBAT_RAMVAR);
	out->I -= mk2UnpackRamFloat(raw->inverterI, dev, VE_VAR_IBAT_RAMVAR);

	/* convert period to frequency in Hz */
	out->period = mk2UnpackRamFloat(raw->period, dev, VE_VAR_INVERTER_PERIOD_RAMVAR);
	if (out->period)
		out->frequency = 10.0F / out->period;

	logI("mk2-values", "dc V: %.1f V", out->V);
	logI("mk2-values", "dc I: %.1f A", out->I);
	logI("mk2-values", "output frequency %.1f Hz", out->frequency);
}

/** conversion for special frame from vebus_system.c */
void mk2UnpackAcInConfig(VebusAcInConfig const* raw, VebusAcInConfigF* out)
{
	out->limit = raw->limit * 0.1F;
	out->max = raw->max * 0.1F;
	out->min = raw->min * 0.1F;
}

/**
 * @brief conversion of settings scale
 * @param dev	device the settings is from
 * @param id	settings id
 * @param units	intended unit system
 * @return float representation of the setting
 */
float vebusSettingScale(struct VebusDeviceS* dev, un8 id, VeUnitSet units)
{
	float scale;
	VebusSettingInfo* settingInfo = vebusGetSettingInfoOfDevice(dev);

	if (settingInfo[id].scale >= 0)
		scale = settingInfo[id].scale;
	else
		scale = 1 / - (float) settingInfo[id].scale;

	if (units == VE_UNITS_VSC) {
		VebusUnit unit = vebusGetUnit(id, units);

		/* Hysteris for this value must be subtracted */
		if (id == VE_HYST_FOR_HIGHEST_ACCEPTED_U_MAINS)
			scale *= -1;

		if (unit == VE_U_SEC) {
			/* multi uses minutes, vsc seconds */
			scale *= 60;
		} else if (unit == VE_U_W) {
			/* multi uses current, vsc Watts */
			scale *= settingInfo[VE_U_INV_SETPOINT].defaultValue;
		}
	}

	return scale;
}

/**
 * @brief offset of a setting
 * @param dev	device the settings is of
 * @param id	the settings id
 * @param units	intended unit
 * @return offset of the settings value
 *
 * @note VSC format some hystoresis values are reported as absolute values.
 *		This function mimics that behaviour when units == VSC
 */
sn16 vebusSettingOffset(struct VebusDeviceS* dev, un8 id, VeUnitSet units)
{
	VebusSettingInfo* settingInfo = vebusGetSettingInfoOfDevice(dev);
	sn16 offset = settingInfo[id].offset;

	if (units == VE_UNITS_VSC) {
		switch (id) {
		/* VSC uses absolute values for some hysteris settings, add the dependend values */
		case 12:
			return dev->settings[11] + settingInfo[11].offset;
		case 45:
			return dev->settings[44] + settingInfo[44].offset;
		case 47:
			return -(dev->settings[46] + settingInfo[46].offset);
		case 63:
			/* note -0x8000 because scale is made negative */
			return dev->settings[12] + dev->settings[11] + settingInfo[11].offset - 0x8000;
		}
	}
	return offset;
}

/**
 * @brief Float representation of a setting
 * @param raw	value as obtained
 * @param dev	device the value was obtained from
 * @param id	setting id of the value
 * @param units	intended unit system
 * @return float representation
 */
float mk2UnpackSettingFloat(sn32 raw, struct VebusDeviceS* dev, un8 id, VeUnitSet units)
{
	VebusSettingInfo* settingInfo = vebusGetSettingInfoOfDevice(dev);
	if (settingInfo[id].scale == 0)
		return 0;

	return (float) (raw + vebusSettingOffset(dev, id, units)) * vebusSettingScale(dev, id, units);
}

/**
 * @brief Converts a float to the raw format of the multi
 * @param value		The settings value as float
 * @param dev		vebus device to send the value to
 * @param id		setting id
 * @param units		Unit system of the value given
 * @return			packed format as send to the vebus device.
 */
un16 mk2PackSettingFloat(float value, struct VebusDeviceS* dev, un8 id, VeUnitSet units)
{
	VebusSettingInfo* settingInfo = vebusGetSettingInfoOfDevice(dev);
	if (settingInfo[id].scale == 0)
		return 0;

	value /= vebusSettingScale(dev, id, units);
	return (un16) value - vebusSettingOffset(dev, id, units);
}

/**
 * @brief locates a bit in the settings and changes it value (not send)
 * @param dev		device of which to alter the flags
 * @param id		bit id to alter
 * @param value		the new value
 * @return success
 */
static veBool setFlag(struct VebusDeviceS* dev, un8 id, veBool value)
{
	un8 bit;
	un8 flag;
	un8 const flags[] = {VE_FLAGS_0, VE_FLAGS_1, VE_FLAGS_2, VE_FLAGS_3};

	bit = id & 0xF;
	flag = id >> 4;
	if (flag >= 4)
		return veFalse;

	if (value)
		dev->settings[flags[flag]] |= 1 << bit;
	else
		dev->settings[flags[flag]] &= ~(1 << bit);

	return veTrue;
}

/**
 * @brief sets a settings locally
 * @param dev	device to act on
 * @param id	id of the setting
 * @param value	float value
 * @return	wether succesful
 */
veBool vebusLocalSetSetting(struct VebusDeviceS * dev, un8 id, float value)
{
	un16 raw;
	if (id >= VEBUS_SETTINGS_COUNT)
		return veFalse;
	raw = mk2PackSettingFloat(value, dev, id, vebusSystem.units);
	if (raw < vebusGetSettingRawMinimum(dev, id) || raw > vebusGetSettingRawMaximum(dev, id))
		return veFalse;

	dev->settings[id] = raw;
	return veTrue;
}

/**
 * @brief locates a bit in the settings and changes it value (not send) with sanity checks
 * @param dev		device of which to alter the flags
 * @param id		bit id to alter
 * @param value		the new value
 * @return success
 */
veBool vebusLocalSetFlag(struct VebusDeviceS* dev, un8 id, veBool value)
{
	/* do not change not promoted options */
	if (vebusGetUnit(id + VEBUS_FLAGS_OFFSET, VE_UNITS_MULTI) == VE_U_DFLAG)
		return veFalse;

	if (!setFlag(dev, id, value))
		return veFalse;

	/* These must be kept in sync */
	if (id == VE_DISABLE_WAVE_CHECK_INV_FLAG)
		return setFlag(dev, VE_DISABLE_WAVE_CHECK_FLAG, !value);

	if (id == VE_DISABLE_WAVE_CHECK_FLAG)
		return setFlag(dev, VE_DISABLE_WAVE_CHECK_INV_FLAG, !value);

	return veTrue;
}

/**
 * @brief return a flag value of a Ve.Bus device
 * @param dev	device to return the flag of
 * @param id	flag id
 * @return	value of the flag. (always false with illegal parameters)
 */
void vebusGetFlag(struct VebusDeviceS *dev, VeVariant *var, un8 id)
{
	un8 bit;
	int flag;

	bit = id & 0xF;
	flag = vebusGetSettingIdFromFlag(id);

	if (flag == VEBUS_ID_INVALID) {
		var->type.tp = VE_UNKNOWN;
		return;
	}

	var->type.tp = VE_BIT1;
	var->value.UN32 = (dev->settings[flag] & (1 << bit) ? veTrue : veFalse);
}

/** common proto for getSetting / RamValue with bounds check  */
void vebusGetSettingValue(struct VebusDeviceS* dev, un8 id, VeVariant *var, VeUnitSet units)
{
	if (id < ARRAY_LENGTH(dev->settings)) {
		var->type.tp = VE_FLOAT;
		var->value.Float = mk2UnpackSettingFloat(dev->settings[id], dev, id, units);
		return;
	}

	var->type.tp = VE_UNKNOWN;
}

/** common proto for getSetting / RamValue with bounds check  */
void vebusGetRamValue(struct VebusDeviceS* dev, un8 id, VeVariant *var, VeUnitSet units)
{
	VE_UNUSED(units);

	if (id < ARRAY_LENGTH(dev->vars)) {
		var->type.tp = VE_FLOAT;
		var->value.Float = mk2UnpackRamFloat(dev->vars[id], dev, id);
		return;
	}

	var->type.tp = VE_UNKNOWN;
}

/*
 * The Multi Compact can report high voltages on its input when nothing is connected.
 * This looks a bit weird in graphs etc, so force zero in these cases.
 */
static float correctVoltage(float voltage)
{
	return (voltage < 50.0f ? 0.0f : voltage);
}

static float acInputVoltage(struct VebusDeviceS* dev, Mk2RawAcPhase const *raw)
{
	return correctVoltage(mk2UnpackRamFloat(raw->inputV, dev, VE_VAR_UINPUT_RMS_RAMVAR));
}

/**
 * @note The input current used to be unsigned. At the time of writing vebus, does return
 * a signed value, but the value info is not updated accordingly yet. Hence the cast to
 * force it a signed value.
 */
static float acInputCurrent(struct VebusDeviceS* dev, Mk2RawAcPhase const *raw)
{
	return mk2UnpackRamFloat((sn16) raw->inputI, dev, VE_VAR_IINPUT_RMS_RAMVAR) * raw->backfeedFactor;
}

static float acOutputVoltage(struct VebusDeviceS* dev, Mk2RawAcPhase const *raw)
{
	return correctVoltage(mk2UnpackRamFloat(raw->inverterV, dev, VE_VAR_UINVERTER_RMS_RAMVAR));
}

static float acOutputCurrent(struct VebusDeviceS* dev, Mk2RawAcPhase const *raw)
{
	return mk2UnpackRamFloat(raw->inverterI, dev, VE_VAR_IINVERTER_RMS_RAMVAR) * raw->inverterFactor;
}

static float acInputFrequency(struct VebusDeviceS* dev, Mk2RawAcPhase const *raw, float *period)
{
	float ret;

	*period = mk2UnpackRamFloat(raw->inputPeriod, dev, VE_VAR_AC_INPUT_PERIOD_RAMVAR);
	ret = *period;

	if (ret)
		ret = 10.0F / ret;
	/* if there is no input voltage, the system reports its last value, report 0 instead */
	if (acInputVoltage(dev, raw) == 0)
		ret = 0;

	return ret;
}

#if CFG_MK2_KWH_COUNTERS

static float energyCounter(un8 id, VeUnitSet units)
{
	float energyRaw = (float) vebusSystem.kwh.totals[id - VE_SYS_ENERGY_COUNTER_FIRST_SYS];

	/* this is the values as received from the multi (shifted Joules) */
	if (units == VE_UNITS_MULTI)
		return energyRaw;

	 /* otherwise convert to kWh */
	return energyRaw * 65536.0f / 3600.0f / 1000.0f;
}

#else

static float energyCounter(un8 id, VeUnitSet units)
{
	VE_UNUSED(id);
	VE_UNUSED(units);

	return -1;
}

#endif

/** conversion for special frame from vebus_system.c */
void mk2UnpackAcPhase(struct VebusDeviceS* dev, Mk2RawAcPhase const* raw,
					  Mk2AcInPhaseF* acIn, Mk2AcOutPhaseF* acOut)
{
	acIn->V = acInputVoltage(dev, raw);
	acIn->I = acInputCurrent(dev, raw);
	acIn->frequency = acInputFrequency(dev, raw, &acIn->period);
	acOut->V = acOutputVoltage(dev, raw);
	acOut->I = acOutputCurrent(dev, raw);

	logI("mk2-values", "input V: %.1f V", acIn->V);
	logI("mk2-values", "input I: %.1f A", acIn->I);
	logI("mk2-values", "inverter voltage %.1f V", acOut->V);
	logI("mk2-values", "inverter current %.1f A", acOut->I);
	logI("mk2-values", "input frequency %.1f Hz", acIn->frequency);
}

#if CFG_MK2_AC_SENSOR

/* The value of the voltage needs to be unpacked depending on the input / output location */
static float acSensorVoltage(VeCurrentSensor *sensor)
{
	if ((sensor->infoFlags & 0x1F) == 0)
		return mk2UnpackRamFloat(sensor->voltage, sensor->dev, VE_VAR_UINPUT_RMS_RAMVAR);
	return mk2UnpackRamFloat(sensor->voltage, sensor->dev, VE_VAR_UINVERTER_RMS_RAMVAR);
}

static void acSensorValue(un8 id, VeVariant *var, VeUnitSet units)
{
	un8 instance = (id - VE_SYS_AC_SENSOR_FIRST_SYS) / VE_SYS_AC_SENSOR_REG_COUNT;
	VeCurrentSensor *sensor = &vebusSystem.iSense.sensors[instance];
	VE_UNUSED(units);

	if (instance >= vebusSystem.iSense.count) {
		var->type.tp = VE_UNKNOWN;
		return;
	}

	id -= instance * VE_SYS_AC_SENSOR_REG_COUNT;
	switch (id)
	{
	case VE_SYS_AC_SENSOR0_VOLTAGE_SYS:
		veVariantFloat(var, acSensorVoltage(sensor));
		return;

	case VE_SYS_AC_SENSOR0_CURRENT_SYS:
		veVariantFloat(var, sensor->current / 100.0f);
		return;

	case VE_SYS_AC_SENSOR0_POWER_SYS:
		veVariantFloat(var, acSensorVoltage(sensor) * sensor->current / 100.0f);
		return;

	case VE_SYS_AC_SENSOR0_ENERGY_SYS:
		veVariantFloat(var, sensor->kwhTotal * 65536.0f / 3600.0f / 1000.0f);
		return;

	case VE_SYS_AC_SENSOR0_POSITION_SYS:
		/*
		 * 00000 = PV inverter on input
		 * 00001 = PV inverter on output
		 */
		veVariantUn32(var, sensor->infoFlags & 0x1F);
		return;

	case VE_SYS_AC_SENSOR0_PHASE_SYS:
		/* 00 = L1, 01 = L2, 10 = L3 */
		veVariantUn32(var, (sensor->infoFlags >> 5) & 0x3);
		return;
	}
}
#else
static void acSensorValue(un8 id, VeVariant *var, VeUnitSet units)
{
	VE_UNUSED(id);
	VE_UNUSED(var);
	VE_UNUSED(units);
}
#endif

static void getLed(VeVariant *var, un8 flag)
{
	un8 on = (vebusSystem.state.leds & flag ? 1 : 0);
	un8 blink = (vebusSystem.state.ledsBlink & flag ? 1 : 0);
	veVariantUn8(var, on + 2 * blink);
}

/**
 * Unpack a vebus system value in case the raw values are stored.
 */
void vebusGetSystemValue(struct VebusDeviceS* dev, un8 id, VeVariant *var, VeUnitSet units)
{
	un8 phase;
	Mk2RawAcPhase const* raw;
	VeAcConn acConn;

	var->type.tp = VE_UNKNOWN;
	phase = vebusGetPhase(id + VEBUS_SYS_OFFSET);

	/* phase 0 -> not phase specific */
	if (!phase) {
		switch (id) {
#if CFG_WITH_VECAN_PRODUCT_NAMES
		case VE_SYS_PRODUCT_NAME_SYS:
			veVariantStr(var, veProductGetName(mk2VersionToProductBcd(dev->version.number)));
			return;
#endif
		case VE_SYS_INVERTER_FREQUENCY_SYS:
			{
				float ret = mk2UnpackRamFloat(vebusSystem.dc.period, dev, VE_VAR_INVERTER_PERIOD_RAMVAR);
				if (ret)
					ret = 10.0F / ret;
				veVariantFloat(var, ret);
				return;
			}

		case VE_SYS_UBAT_SYS:
			veVariantFloat(var, mk2UnpackRamFloat(vebusSystem.dc.V, dev, VE_VAR_UBAT_RAMVAR));
			return;

		case VE_SYS_IBAT_SYS:
			veVariantFloat(var, mk2UnpackRamFloat(vebusSystem.dc.chargerI, dev, VE_VAR_IBAT_RAMVAR) -
						mk2UnpackRamFloat(vebusSystem.dc.inverterI, dev, VE_VAR_IBAT_RAMVAR));
			return;

		case VE_SYS_VEBUS_ERROR_SYS:
			veVariantUn32(var, vebusSystem.state.vebusError);
			return;

		case VE_SYS_MK2_VERSION_SYS:
			veVariantUn32(var, mk2.version);
			return;

		case VE_SYS_VEBUS_VERSION_SYS:
			veVariantUn32(var, dev->version.number);
			return;

		case VE_SYS_BMS_VERSION_SYS:
			if (vebusSystem.bmsVersion)
				veVariantUn32(var, vebusSystem.bmsVersion);
			return;

		case VE_SYS_DMC_VERSION_SYS:
			if (vebusSystem.dmcVersion)
				veVariantUn32(var, vebusSystem.dmcVersion);
			return;

		case VE_SYS_PHASES_SYS:
			veVariantUn32(var, vebusSystem.ac[0].phaseCount);
			return;

		case VE_SYS_ALARM_LOW_BATTERY_SYS:
			veVariantBit(var, 2, vebusSystem.state.alarms & VE_ALARM_FLAG_LOW_BATTERY ? VEBUS_ALARM_ACTIVE :
					vebusSystem.state.alarms & VE_ALARM_FLAG_LOW_BATTERY_PRE ? VEBUS_ALARM_WARNING : VEBUS_ALARM_OK);
			return;

		case VE_SYS_ALARM_TEMPERATURE_SYS:
			veVariantBit(var, 2, vebusSystem.state.alarms & VE_ALARM_FLAG_TEMPERATURE ? VEBUS_ALARM_ACTIVE :
					vebusSystem.state.alarms & VE_ALARM_FLAG_TEMPERATURE_PRE ? VEBUS_ALARM_WARNING : VEBUS_ALARM_OK);
			return;

		case VE_SYS_ALARM_OVERLOAD_SYS:
			veVariantBit(var, 2, vebusSystem.state.alarms & VE_ALARM_FLAG_OVERLOAD ? VEBUS_ALARM_ACTIVE :
					vebusSystem.state.alarms & VE_ALARM_FLAG_OVERLOAD_PRE ? VEBUS_ALARM_WARNING : VEBUS_ALARM_OK);
			return;

		case VE_SYS_ALARM_U_BAT_RIPPLE_SYS:
			veVariantBit(var, 2, vebusSystem.state.alarms & VE_ALARM_FLAG_U_BAT_RIPPLE ? VEBUS_ALARM_ACTIVE :
					vebusSystem.state.alarms & VE_ALARM_FLAG_U_BAT_RIPPLE_PRE ? VEBUS_ALARM_WARNING : VEBUS_ALARM_OK);
			return;

		case VE_SYS_ALARM_TEMP_SENSOR_SYS:
			veVariantBit(var, 2, vebusSystem.alarms & VE_ALARM_FLAG_TEMP_SENSOR ? VEBUS_ALARM_ACTIVE : VEBUS_ALARM_OK);
			return;

		case VE_SYS_ALARM_VOLTAGE_SENSOR_SYS:
			veVariantBit(var, 2, vebusSystem.alarms & VE_ALARM_FLAG_VOLTAGE_SENSOR ? VEBUS_ALARM_ACTIVE : VEBUS_ALARM_OK);
			return;

		case VE_SYS_AC_IN_ACTIVE_SYS:
			veVariantUn32(var, vebusActiveIn(&vebusSystem.acInConfig));
			return;

		/* Note: this is the current limit of the active input */
		case VE_SYS_CURRENT_LIMIT_IS_ADJUSTABLE_SYS:
			veVariantBit(var, 2, vebusActiveCurrentLimitChangable(&vebusSystem.acInConfig));
			return;

		case VE_SYS_MODE_IS_ADJUSTABLE_SYS:
			/*
			 * If the device is off no panel frames are sent, i.o.w. the control state
			 * is disabled (Since the BMS might want to turn the device on again and
			 * On / Off messages are send simultaneously). If there is no BMS, the device
			 * could never be turned on again if only the control state was checked. In
			 * order to be able to turn the device on in that case, the (stored) presence
			 * of the BMS is used to determin if control is possible while the device is off.
			 */
			veVariantBit(var, 2, mk2.state.mode == SP_SWITCH_OFF ? vebusCanBeControlled() :
									vebusSystem.ctrl.state != VEBUS_PANEL_DISABLED);
			return;

#if CFG_MK2_AC_SENSOR
		case VE_SYS_AC_SENSOR_COUNT_SYS:
			veVariantUn32(var, vebusSystem.iSense.count);
			return;
#endif
		case VE_SYS_QUIRKS_SYS:
			veVariantUn16(var, vebusSystem.quirks);
			return;

		case VE_SYS_AC_POWER_MEASUREMENT_SYS:
			veVariantUn32(var, vebusSystem.power.measurementType);
			return;

		case VE_SYS_AC_IN_P_SYS:
			if (vebusSystem.power.flags & VE_POWER_VALID) {
				un8 n;
				sn32 total = 0;

				for (n = 0; n < 3; n++)
					total += vebusSystem.power.input[n];
				veVariantSn32(var, total);
				return;
			}
			// fall through, use apparent power

		case VE_SYS_AC_IN_APPARENT_P_SYS:
		{
			sn32 total = 0;
			un8 n;

			for (n = 0; n < vebusSystem.ac[0].phaseCount; n++) {
				raw = &vebusSystem.ac[n];
				total += (sn32) (acInputVoltage(dev, raw) * acInputCurrent(dev, raw));
			}

			veVariantSn32(var, total);
			return;
		}

		// total power of the output phases
		case VE_SYS_AC_OUT1_P_SYS:
			if (vebusSystem.power.flags & VE_POWER_VALID) {
				un8 n;
				sn32 total = 0;

				for (n = 0; n < 3; n++)
					total += vebusSystem.power.output[n];
				veVariantSn32(var, total);
				return;
			}

		// fall through, use apparent power
		case VE_SYS_AC_OUT1_APPARENT_P_SYS:
		{
			sn32 total = 0;
			un8 n;

			for (n = 0; n < vebusSystem.ac[0].phaseCount; n++) {
				raw = &vebusSystem.ac[n];
				total += (sn32) (acOutputVoltage(dev, raw) * acOutputCurrent(dev, raw));
			}

			veVariantSn32(var, total);
			return;
		}

		case VE_SYS_AC_IN_LIM_SYS:
			veVariantFloat(var, vebusSystem.acInConfig.limit / 10.0f);
			return;

		case VE_SYS_AC_IN1_LIM_SYS:
			veVariantFloat(var, vebusSystem.acInConfigPerAcIn[0].limit / 10.0f);
			return;

		case VE_SYS_AC_IN2_LIM_SYS:
			veVariantFloat(var, vebusSystem.acInConfigPerAcIn[1].limit / 10.0f);
			return;

		case VE_SYS_SHORT_IDS_SYS:
			veVariantUn32(var, vebusSystem.state.shortIds);
			return;

		case VE_SYS_MAINSTATE_SYS:
			veVariantUn32(var, vebusSystem.convState.state);
			return;

		case VE_SYS_SUBSTATE_SYS:
			veVariantUn32(var, vebusSystem.convState.substate);
			return;

		case VE_SYS_DC_POWER_SYS:
			if (vebusSystem.power.flags & VE_POWER_VALID) {
				un8 n;
				sn32 total = 0;

				for (n = 0; n < 3; n++)
					total += vebusSystem.power.inverter[n];
				veVariantSn32(var, total);
			} else {
				float current = mk2UnpackRamFloat(vebusSystem.dc.chargerI, dev, VE_VAR_IBAT_RAMVAR) -
						mk2UnpackRamFloat(vebusSystem.dc.inverterI, dev, VE_VAR_IBAT_RAMVAR);
				float voltage = mk2UnpackRamFloat(vebusSystem.dc.V, dev, VE_VAR_UBAT_RAMVAR);
				veVariantSn32(var, (sn32) (current * voltage));
			}
			return;

		case VE_SYS_LED_MAINS_SYS:
		case VE_SYS_LED_ABSORPTION_SYS:
		case VE_SYS_LED_BULK_SYS:
		case VE_SYS_LED_FLOAT_SYS:
		case VE_SYS_LED_INVERTER_SYS:
		case VE_SYS_LED_OVERLOAD_SYS:
		case VE_SYS_LED_LOW_BATTERY_SYS:
		case VE_SYS_LED_TEMPERATURE_SYS:
			getLed(var, 1 << (id - VE_SYS_LED_MAINS_SYS));
			return;

		default:
			if (id >= VE_SYS_ENERGY_COUNTER_FIRST_SYS && id <= VE_SYS_ENERGY_COUNTER_LAST_SYS) {
				veVariantFloat(var, energyCounter(id, units));
				return;
			}

			if (id >= VE_SYS_AC_SENSOR_FIRST_SYS && id <= VE_SYS_AC_SENSOR_LAST_SYS) {
				acSensorValue(id, var, units);
				return;
			}

			var->type.tp = VE_UNKNOWN;
			return;
		}
	}

	acConn = vebusGetAcIn(id + VEBUS_SYS_OFFSET);
	if (acConn == 5 /* active ac in */)
		acConn = vebusActiveIn(&vebusSystem.acInConfig);

	if (phase > vebusSystem.ac[0].phaseCount || (acConn != VE_ACOUT && acConn != vebusActiveIn(&vebusSystem.acInConfig))) {
		var->type.tp = VE_UNKNOWN;
		return;
	}

	raw = &vebusSystem.ac[phase - 1];

	/*
	 * There are many ac in phase fields, and there is already pointer to the data
	 * needed for this field (raw), so normalize the value for ACIN / phase A so not
	 * all ids need to be listed in the switch.
	 */
	if (id >= VE_SYS_ACIN_PHASE_FIRST_SYS && id <= VE_SYS_ACIN_PHASE_LAST_SYS)
		id = ((id - VE_SYS_ACIN_PHASE_FIRST_SYS) % VE_SYS_ACIN_PHASE_FIELDS) + VE_SYS_ACIN_PHASE_FIRST_SYS;

	switch (id) {
	case VE_SYS_AC_IN_A_V_SYS:
		veVariantFloat(var, acInputVoltage(dev, raw));
		return;

	case VE_SYS_AC_IN_A_I_SYS:
		veVariantFloat(var, acInputCurrent(dev, raw));
		return;

	case VE_SYS_AC_IN_A_FREQUENCY_SYS:
		{
			float period;
			veVariantFloat(var, acInputFrequency(dev, raw, &period));
			return;
		}

	case VE_SYS_AC_IN_A_P_SYS:
		if (vebusSystem.power.flags & VE_POWER_VALID)
			veVariantSn32(var, vebusSystem.power.input[phase - 1]);
		else
			veVariantSn32(var, (sn32) (acInputVoltage(dev, raw) * acInputCurrent(dev, raw)));
		return;

	case VE_SYS_AC_IN_A_APPARENT_P_SYS:
		veVariantSn32(var, (sn32) (acInputVoltage(dev, raw) * acInputCurrent(dev, raw)));
		break;

	case VE_SYS_AC_OUT1_A_V_SYS:
	case VE_SYS_AC_OUT1_B_V_SYS:
	case VE_SYS_AC_OUT1_C_V_SYS:
		veVariantFloat(var, acOutputVoltage(dev, raw));
		return;

	case VE_SYS_AC_OUT1_A_I_SYS:
	case VE_SYS_AC_OUT1_B_I_SYS:
	case VE_SYS_AC_OUT1_C_I_SYS:
		veVariantFloat(var, acOutputCurrent(dev, raw));
		return;

	case VE_SYS_AC_OUT1_A_P_SYS:
	case VE_SYS_AC_OUT1_B_P_SYS:
	case VE_SYS_AC_OUT1_C_P_SYS:
		if (vebusSystem.power.flags & VE_POWER_VALID)
			veVariantSn32(var, vebusSystem.power.output[phase - 1]);
		else
			veVariantSn32(var, (sn32) (acOutputVoltage(dev, raw) * acOutputCurrent(dev, raw)));
		return;

	case VE_SYS_AC_OUT1_A_APPARENT_P_SYS:
	case VE_SYS_AC_OUT1_B_APPARENT_P_SYS:
	case VE_SYS_AC_OUT1_C_APPARENT_P_SYS:
		veVariantSn32(var, (sn32) (acOutputVoltage(dev, raw) * acOutputCurrent(dev, raw)));
		break;

	case VE_SYS_AC_OUT1_A_FREQUENCY_SYS:
	case VE_SYS_AC_OUT1_B_FREQUENCY_SYS:
	case VE_SYS_AC_OUT1_C_FREQUENCY_SYS:
	{
		float ret = mk2UnpackRamFloat(vebusSystem.dc.period, dev, VE_VAR_INVERTER_PERIOD_RAMVAR);
		if (ret)
			ret = 10.0F / ret;
		veVariantFloat(var, ret);
		return;
	}

	default:
		var->type.tp = VE_UNKNOWN;
	}
}


/**
 * Up till here, the settings / ramvars / flags where treated seperately as
 * defined in the protocol. From here on they are treated the same. Note that
 * "VebusValue nr" indicates any values and "un8 id" a speicifc one.
 */
veBool vebusSupportedVariable(struct VebusDeviceS* dev, VebusValue nr)
{
	typedef veBool(*F)(struct VebusDeviceS* dev, un8 nr);
	VebusValueType tp = vebusGetValueType(nr);
	F const f[] = {
		vebusSupportedSetting,
		vebusSupportedFlag,
		vebusSupportedRamVar,
		vebusSupportedSystemVar
	};
	return (tp < ARRAY_LENGTH(f) ? f[tp](dev, (un8) nr) : veFalse);
}

/*
 * Returns if the value is "known". It does not mean it is supported /
 * supported by the device. But at least the nr is within valid bounds.
 * Here they are all glued together and identified by the VebusValue nr
 * instead of the byte id. The high byte of the nr indicates if it is a
 * settings / flag etc. The low byte the id. When calling specific
 * routines the nr can casted to an un8 to become the id.
 */
veBool vebusKnownVariable(struct VebusDeviceS* dev, VebusValue nr)
{
#define X(a,b) b,
	un8 const count[] = {
		VEBUS_VALUE_TYPES
	};
#undef X

	VebusValueType tp = vebusGetValueType(nr);
	if (tp == VEBUS_TP_INVALID)
		return veFalse;

	if (tp != VEBUS_TP_SYSTEM && !dev)
		return veFalse;

	return (un8) nr < count[tp];
}

/** Returns any vebus value as indicated by nr */
void vebusGetValue(VebusDevice *dev, VebusValue nr, VeVariant *var, VeUnitSet units)
{
	switch (vebusGetValueType(nr))
	{
	case VEBUS_TP_SETTING:
		vebusGetSettingValue(dev, nr, var, units);
		return;

	case VEBUS_TP_FLAG:
		vebusGetFlag(dev, var, 1);
		fflush(stdout);
		return;

	case VEBUS_TP_RAMVAR:
		vebusGetRamValue(dev, nr, var, units);
		return;

	case VEBUS_TP_SYSTEM:
		vebusGetSystemValue(dev, nr, var, units);
		return;

	default:
		var->type.tp = VE_UNKNOWN;
	}
}

float vebusGetSettingMinimum(struct VebusDeviceS *dev, un8 id, VeUnitSet units)
{
	VebusSettingInfo *settingInfo = vebusGetSettingInfoOfDevice(dev);

	if (id >= VEBUS_SETTINGS_COUNT || !settingInfo)
		return 0;

	return mk2UnpackSettingFloat(settingInfo[id].minimum, dev, id, units);
}

float vebusGetSettingMaximum(struct VebusDeviceS *dev, un8 id, VeUnitSet units)
{
	VebusSettingInfo* settingInfo = vebusGetSettingInfoOfDevice(dev);

	if (id >= VEBUS_SETTINGS_COUNT || !settingInfo)
		return 0;

	return mk2UnpackSettingFloat(settingInfo[id].maximum, dev, id, units);
}

float vebusGetMinimum(struct VebusDeviceS *dev, VebusValue nr, VeUnitSet units)
{
	if (vebusGetValueType(nr) == VEBUS_TP_SETTING)
		return vebusGetSettingMinimum(dev, (un8) nr, units);


	switch (nr)
	{
	case VE_SYS_AC_IN_LIM:
		return (float) vebusSystem.acInConfig.min / 10;

	case VE_SYS_AC_IN1_LIM:
		return (float) vebusSystem.acInConfigPerAcIn[0].min / 10;

	case VE_SYS_AC_IN2_LIM:
		return (float) vebusSystem.acInConfigPerAcIn[1].min / 10;

	default:
		return -1;
	}
}

float vebusGetMaximum(struct VebusDeviceS *dev, VebusValue nr, VeUnitSet units)
{
	if (vebusGetValueType(nr) == VEBUS_TP_SETTING)
		return vebusGetSettingMaximum(dev, (un8) nr, units);

	switch (nr)
	{
	case VE_SYS_AC_IN_LIM:
		return (float) vebusSystem.acInConfig.max / 10;

	case VE_SYS_AC_IN1_LIM:
		return (float) vebusSystem.acInConfigPerAcIn[0].max / 10;

	case VE_SYS_AC_IN2_LIM:
		return (float) vebusSystem.acInConfigPerAcIn[1].max / 10;

	default:
		return -1;
	}
}
