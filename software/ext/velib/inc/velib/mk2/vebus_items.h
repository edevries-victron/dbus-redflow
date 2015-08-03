#ifndef _VELIB_MK2_VEBUS_ITEMS_H_
#define _VELIB_MK2_VEBUS_ITEMS_H_

#include <velib/types/ve_item.h>
#include <velib/types/ve_item_def.h>
#include <velib/mk2/mk2_actions.h>

/* These should not be invalidated since they are stored */
#define VE_SYSTEM_STORED_DEVICE_VERSIONS									\
	X(VE_SYS_DMC_VERSION,					"Devices/DMC/Version")			\
	X(VE_SYS_BMS_VERSION,					"Devices/BMS/Version")			\

#define VE_SYSTEM_GENERAL													\
	X(VE_SYS_MK2_VERSION,					"Interfaces/Mk2/Version")		\
	X(VE_SYS_VEBUS_VERSION,					"Devices/0/Version")			\
	X(VE_SYS_PRODUCT_NAME,					"ProductName")					\

#define VE_SYSTEM_DC_VARS													\
	X(VE_SYS_UBAT,							"Dc/V")							\
	X(VE_SYS_IBAT,							"Dc/I")							\
	X(VE_SYS_DC_POWER,						"Dc/P")							\

#define VE_SYSTEM_AC_VARS													\
	X(VE_SYS_PHASES,						"Ac/NumberOfPhases")			\
	X(VE_SYS_AC_POWER_MEASUREMENT,			"Ac/PowerMeasurementType")		\
	X(VE_SYS_QUIRKS,						"Quirks")						\

#define VE_SYSTEM_AC_VARS_ACIN												\
	X(VE_SYS_AC_IN_ACTIVE,					"Ac/ActiveIn/ActiveInput")		\
	X(VE_SYS_AC_IN_P,						"Ac/ActiveIn/P")				\
	X(VE_SYS_AC_IN_APPARENT_P,				"Ac/ActiveIn/S")				\
																			\
	X(VE_SYS_AC_IN_A_V,						"Ac/ActiveIn/L1/V")				\
	X(VE_SYS_AC_IN_A_I,						"Ac/ActiveIn/L1/I")				\
	X(VE_SYS_AC_IN_A_P,						"Ac/ActiveIn/L1/P")				\
	X(VE_SYS_AC_IN_A_FREQUENCY,				"Ac/ActiveIn/L1/F")				\
	X(VE_SYS_AC_IN_A_APPARENT_P,			"Ac/ActiveIn/L1/S")				\
																			\
	X(VE_SYS_AC_IN_B_V,						"Ac/ActiveIn/L2/V")				\
	X(VE_SYS_AC_IN_B_I,						"Ac/ActiveIn/L2/I")				\
	X(VE_SYS_AC_IN_B_P,						"Ac/ActiveIn/L2/P")				\
	X(VE_SYS_AC_IN_B_FREQUENCY,				"Ac/ActiveIn/L2/F")				\
	X(VE_SYS_AC_IN_B_APPARENT_P,			"Ac/ActiveIn/L2/S")				\
																			\
	X(VE_SYS_AC_IN_C_V,						"Ac/ActiveIn/L3/V")				\
	X(VE_SYS_AC_IN_C_I,						"Ac/ActiveIn/L3/I")				\
	X(VE_SYS_AC_IN_C_P,						"Ac/ActiveIn/L3/P")				\
	X(VE_SYS_AC_IN_C_FREQUENCY,				"Ac/ActiveIn/L3/F")				\
	X(VE_SYS_AC_IN_C_APPARENT_P,			"Ac/ActiveIn/L3/S")				\

#define VE_SYSTEM_AC_VARS_ACIN1												\
	X(VE_SYS_AC_IN1_A_V,					"Ac/In1/L1/V")					\
	X(VE_SYS_AC_IN1_A_I,					"Ac/In1/L1/I")					\
	X(VE_SYS_AC_IN1_A_P,					"Ac/In1/L1/P")					\
	X(VE_SYS_AC_IN1_A_FREQUENCY,			"Ac/In1/L1/F")					\
	X(VE_SYS_AC_IN1_A_APPARENT_P,			"Ac/In1/L1/S")					\
																			\
	X(VE_SYS_AC_IN1_B_V,					"Ac/In1/L2/V")					\
	X(VE_SYS_AC_IN1_B_I,					"Ac/In1/L2/I")					\
	X(VE_SYS_AC_IN1_B_P,					"Ac/In1/L2/P")					\
	X(VE_SYS_AC_IN1_B_FREQUENCY,			"Ac/In1/L2/F")					\
	X(VE_SYS_AC_IN1_B_APPARENT_P,			"Ac/In1/L2/S")					\
																			\
	X(VE_SYS_AC_IN1_C_V,					"Ac/In1/L3/V")					\
	X(VE_SYS_AC_IN1_C_I,					"Ac/In1/L3/I")					\
	X(VE_SYS_AC_IN1_C_P,					"Ac/In1/L3/P")					\
	X(VE_SYS_AC_IN1_C_FREQUENCY,			"Ac/In1/L3/F")					\
	X(VE_SYS_AC_IN1_C_APPARENT_P,			"Ac/In1/L3/S")					\


#define VE_SYSTEM_AC_VARS_ACIN2												\
	X(VE_SYS_AC_IN2_A_V,					"Ac/In2/L1/V")					\
	X(VE_SYS_AC_IN2_A_I,					"Ac/In2/L1/I")					\
	X(VE_SYS_AC_IN2_A_P,					"Ac/In2/L1/P")					\
	X(VE_SYS_AC_IN2_A_FREQUENCY,			"Ac/In2/L1/F")					\
	X(VE_SYS_AC_IN2_A_APPARENT_P,			"Ac/In2/L1/S")					\
																			\
	X(VE_SYS_AC_IN2_B_V,					"Ac/In2/L2/V")					\
	X(VE_SYS_AC_IN2_B_I,					"Ac/In2/L2/I")					\
	X(VE_SYS_AC_IN2_B_P,					"Ac/In2/L2/P")					\
	X(VE_SYS_AC_IN2_B_FREQUENCY,			"Ac/In2/L2/F")					\
	X(VE_SYS_AC_IN2_B_APPARENT_P,			"Ac/In2/L2/S")					\
																			\
	X(VE_SYS_AC_IN2_C_V,					"Ac/In2/L3/V")					\
	X(VE_SYS_AC_IN2_C_I,					"Ac/In2/L3/I")					\
	X(VE_SYS_AC_IN2_C_P,					"Ac/In2/L3/P")					\
	X(VE_SYS_AC_IN2_C_FREQUENCY,			"Ac/In2/L3/F")					\
	X(VE_SYS_AC_IN2_C_APPARENT_P,			"Ac/In2/L3/S")					\

#define VE_SYSTEM_AC_VARS_ACOUT1											\
	X(VE_SYS_AC_OUT1_P,						"Ac/Out/P")						\
	X(VE_SYS_AC_OUT1_APPARENT_P,			"Ac/Out/S")						\
	X(VE_SYS_AC_OUT1_A_V,					"Ac/Out/L1/V")					\
	X(VE_SYS_AC_OUT1_A_I,					"Ac/Out/L1/I")					\
	X(VE_SYS_AC_OUT1_A_P,					"Ac/Out/L1/P")					\
	X(VE_SYS_AC_OUT1_A_FREQUENCY,			"Ac/Out/L1/F")					\
	X(VE_SYS_AC_OUT1_A_APPARENT_P,			"Ac/Out/L1/S")					\
																			\
	X(VE_SYS_AC_OUT1_B_V,					"Ac/Out/L2/V")					\
	X(VE_SYS_AC_OUT1_B_I,					"Ac/Out/L2/I")					\
	X(VE_SYS_AC_OUT1_B_P,					"Ac/Out/L2/P")					\
	X(VE_SYS_AC_OUT1_B_FREQUENCY,			"Ac/Out/L2/F")					\
	X(VE_SYS_AC_OUT1_B_APPARENT_P,			"Ac/Out/L2/S")					\
																			\
	X(VE_SYS_AC_OUT1_C_V,					"Ac/Out/L3/V")					\
	X(VE_SYS_AC_OUT1_C_I,					"Ac/Out/L3/I")					\
	X(VE_SYS_AC_OUT1_C_P,					"Ac/Out/L3/P")					\
	X(VE_SYS_AC_OUT1_C_FREQUENCY,			"Ac/Out/L3/F")					\
	X(VE_SYS_AC_OUT1_C_APPARENT_P,			"Ac/Out/L3/S")					\

#define VE_SYSTEM_SOC														\
	X(VE_VAR_SOC,							"Soc")

#define VE_SYSTEM_LEDS														\
	X(VE_SYS_LED_MAINS,						"Leds/Mains")					\
	X(VE_SYS_LED_ABSORPTION,				"Leds/Absorption")				\
	X(VE_SYS_LED_BULK,						"Leds/Bulk")					\
	X(VE_SYS_LED_FLOAT,						"Leds/Float")					\
	X(VE_SYS_LED_INVERTER,					"Leds/Inverter")				\
	X(VE_SYS_LED_OVERLOAD,					"Leds/Overload")				\
	X(VE_SYS_LED_LOW_BATTERY,				"Leds/LowBattery")				\
	X(VE_SYS_LED_TEMPERATURE,				"Leds/Temperature")				\

#define VE_SYSTEM_KWH_COUNTERS												\
	X(VE_SYS_ENERGY_AC_IN1_TO_INVERTER,		"Energy/AcIn1ToInverter")		\
	X(VE_SYS_ENERGY_AC_IN2_TO_INVERTER,		"Energy/AcIn2ToInverter")		\
	X(VE_SYS_ENERGY_AC_IN1_TO_AC_OUT,		"Energy/AcIn1ToAcOut")			\
	X(VE_SYS_ENERGY_AC_IN2_TO_AC_OUT,		"Energy/AcIn2ToAcOut")			\
	X(VE_SYS_ENERGY_INVERTER_TO_AC_IN1,		"Energy/InverterToAcIn1")		\
	X(VE_SYS_ENERGY_INVERTER_TO_AC_IN2,		"Energy/InverterToAcIn2")		\
	X(VE_SYS_ENERGY_AC_OUT_TO_AC_IN1,		"Energy/AcOutToAcIn1")			\
	X(VE_SYS_ENERGY_AC_OUT_TO_AC_IN2,		"Energy/AcOutToAcIn2")			\
	X(VE_SYS_ENERGY_INVERTER_TO_AC_OUT,		"Energy/InverterToAcOut")		\
	X(VE_SYS_ENERGY_OUT_TO_INVERTER,		"Energy/OutToInverter")

#define VE_SYSTEM_STATE \
	X(VE_SYS_VEBUS_ERROR,					"VebusError")					\
	X(VE_SYS_SHORT_IDS,						"ShortIds")						\

#define VE_SYSTEM_CONV_VARS_VEBUS											\
	X(VE_SYS_MAINSTATE,						"VebusMainState")					\
	X(VE_SYS_SUBSTATE,						"VebusSubstate")

#define VE_SYSTEM_AC_SENSORS		\
	X(VE_SYS_AC_SENSOR_COUNT,				"AcSensor/Count")				\
																			\
	X(VE_SYS_AC_SENSOR0_VOLTAGE,			"AcSensor/0/Voltage")			\
	X(VE_SYS_AC_SENSOR0_CURRENT,			"AcSensor/0/Current")			\
	X(VE_SYS_AC_SENSOR0_POWER,				"AcSensor/0/Power")				\
	X(VE_SYS_AC_SENSOR0_ENERGY,				"AcSensor/0/Energy")			\
	X(VE_SYS_AC_SENSOR0_POSITION,			"AcSensor/0/Location")			\
	X(VE_SYS_AC_SENSOR0_PHASE,				"AcSensor/0/Phase")				\
																			\
	X(VE_SYS_AC_SENSOR1_VOLTAGE,			"AcSensor/1/Voltage")			\
	X(VE_SYS_AC_SENSOR1_CURRENT,			"AcSensor/1/Current")			\
	X(VE_SYS_AC_SENSOR1_POWER,				"AcSensor/1/Power")				\
	X(VE_SYS_AC_SENSOR1_ENERGY,				"AcSensor/1/Energy")			\
	X(VE_SYS_AC_SENSOR1_POSITION,			"AcSensor/1/Location")			\
	X(VE_SYS_AC_SENSOR1_PHASE,				"AcSensor/1/Phase")				\
																			\
	X(VE_SYS_AC_SENSOR2_VOLTAGE,			"AcSensor/2/Voltage")			\
	X(VE_SYS_AC_SENSOR2_CURRENT,			"AcSensor/2/Current")			\
	X(VE_SYS_AC_SENSOR2_POWER,				"AcSensor/2/Power")				\
	X(VE_SYS_AC_SENSOR2_ENERGY,				"AcSensor/2/Energy")			\
	X(VE_SYS_AC_SENSOR2_POSITION,			"AcSensor/2/Location")			\
	X(VE_SYS_AC_SENSOR2_PHASE,				"AcSensor/2/Phase")				\
																			\
	X(VE_SYS_AC_SENSOR3_VOLTAGE,			"AcSensor/3/Voltage")			\
	X(VE_SYS_AC_SENSOR3_CURRENT,			"AcSensor/3/Current")			\
	X(VE_SYS_AC_SENSOR3_POWER,				"AcSensor/3/Power")				\
	X(VE_SYS_AC_SENSOR3_ENERGY,				"AcSensor/3/Energy")			\
	X(VE_SYS_AC_SENSOR3_POSITION,			"AcSensor/3/Location")			\
	X(VE_SYS_AC_SENSOR3_PHASE,				"AcSensor/3/Phase")				\

#define VE_SYSTEM_ALARMS													\
	X(VE_SYS_ALARM_LOW_BATTERY,				"Alarms/LowBattery")			\
	X(VE_SYS_ALARM_TEMPERATURE,				"Alarms/HighTemperature")		\
	X(VE_SYS_ALARM_OVERLOAD,				"Alarms/Overload")				\
	X(VE_SYS_ALARM_U_BAT_RIPPLE,			"Alarms/Ripple")				\
	X(VE_SYS_ALARM_TEMP_SENSOR,				"Alarms/TemperatureSensor")		\
	X(VE_SYS_ALARM_VOLTAGE_SENSOR,			"Alarms/VoltageSensor")			\

/* Note: virtual switch settings are only supported on 1xx multi's */
#define VE_DEVICE_SETTINGS_VS \
	X(VE_VS_USAGE,							"VirtualSwitchUsage")			\
	X(VE_VS_ON_I_INV_HIGH,					"VsOnLoadHigherThen")			\
	X(VE_VS_ON_U_BAT_HIGH,					"VsOnWhenUdcLowerThen")			\
	X(VE_VS_ON_U_BAT_LOW,					"VsOnWhenUdcHigherThen")		\
	X(VE_VS_TON_I_INV_HIGH,					"VsTimeOnLoadHigh")				\
	X(VE_VS_TON_U_BAT_HIGH,					"VsTimeOnDcVoltageHigh")		\
	X(VE_VS_TON_U_BAT_LOW,					"VsTimeOnDcVoltageLow")			\
	X(VE_VS_TON_NOT_CHARGING,				"VsTimeOnWhenNotCharging")		\
	X(VE_VS_TON_FAN_ON,						"VsTimeOnFanEnabled")			\
	X(VE_VS_TON_TEMPERATURE_ALARM,			"VsTimeOnTemperatureAlarm")		\
	X(VE_VS_TON_LOW_BATTERY_ALARM,			"VsTimeOnLowBatteryAlarm")		\
	X(VE_VS_TON_OVARLOAD_ALARM,				"VsTimeOnOverloadAlarm")		\
	X(VE_VS_TON_U_BATT_RIPPLE_ALARM,		"VsTimeOnRippleAlarm")			\
	X(VE_VS_OFF_I_INV_LOW,					"VsOffLoadLowerThen")			\
	X(VE_VS_OFF_U_BAT_HIGH,					"VsOffWhenUDcLowerThen")		\
	X(VE_VS_OFF_U_BAT_LOW,					"VsOffWhenUDcHigherThen")		\
	X(VE_VS_TOFF_I_INV_LOW,					"VsTimeOffLoadLow")				\
	X(VE_VS_TOFF_U_BATT_HIGH,				"VsTimeOffDcVoltageHigh")		\
	X(VE_VS_TOFF_U_BATT_LOW,				"VsTimeOffDcVoltageLow")		\
	X(VE_VS_TOFF_CHARGING,					"VsOffWhenChargingFor")			\
	X(VE_VS_TOFF_FAN_OFF,					"VsTimeOffFanDisabled")			\
	X(VE_VS_TOFF_CHARGE_BULK_FINISHED,		"VsOffBulkFinishedFor")			\
	X(VE_VS_TOFF_NO_VSON_CONDITION,			"VsOffNoOnConditionFor")		\
	X(VE_VS_TOFF_NO_AC_INPUT,				"VsOffWhenNoAcInputFor")		\
	X(VE_VS_TOFF_TEMPERATURE_ALARM,			"VsOffNoTemperatureAlarm")		\
	X(VE_VS_TOFF_LOW_BATTERY_ALARM,			"VsOffNoBatteryAlarm")			\
	X(VE_VS_TOFF_OVERLOAD_ALARM,			"VsOffNoOverloadAlarm")			\
	X(VE_VS_TOFF_U_BATT_RIPPLE_ALARM,		"VsOffNoRippleAlarm")			\
	X(VE_VS_MINIMUM_ON_TIME,				"VsMinimumOnTime")				\
																			\
	X(VE_VS_2_ON_I_LOAD_HIGH_LEVEL,			"Vs2OnLoadhigherThen")			\
	X(VE_VS_2_ON_I_LOAD_HIGH_TIME,			"Vs2TimeOnloadHigh")			\
	X(VE_VS_2_ON_U_BAT_LOW_LEVEL,			"Vs2OnLowerDcVoltageThen")		\
	X(VE_VS_2_ON_U_BAT_LOW_TIME,			"Vs2OnLowDcVoltageFor")			\
	X(VE_VS_2_OFF_LOAD_LOW_LEVEL,			"Vs2OffLoadLowerThen")			\
	X(VE_VS_2_OFF_LOAD_LOW_TIME,			"Vs2OffLoadLowerFor")			\
	X(VE_VS_2_OFF_U_BAT_HIGH_LEVEL,			"Vs2OffDcVoltageHigh")			\
	X(VE_VS_2_OFF_U_BAT_HIGH_TIME,			"Vs2OffDcVoltageFor")			\
	X(VE_VS_INVERTER_PERIOD_TIME,			"SystemPeriod")					\
	X(VE_VS2_ON_SOC,						"Vs2OnSocLevel")				\

/*
 * Changes to the following settings only become active after a reset:
 *  - VE_CURRENT_FOR_SWITCHING_TO_AES
 *  - VE_HYST_FOR_AES_CURRENT
 *  - VE_BATTERY_CAPACITY
 *  - VE_BATTERY_CHARGE_PERCENTAGE
 */
#define VE_DEVICE_SETTINGS													\
	X(VE_U_BAT_ABSORPTION,					"AbsorptionVoltage")			\
	X(VE_U_BAT_FLOAT,						"FloatVoltage")					\
	X(VE_I_BAT_BULK,						"ChargeCurrent")				\
	X(VE_U_INV_SETPOINT,					"InverterOutputVoltage")		\
	X(VE_I_MAINS_LIMIT_AC1,					"AcCurrentLimit")				\
	X(VE_REPEATED_ABSORPTION_TIME,			"RepeatedAbsorptionTime")		\
	X(VE_REPEATED_ABSORPTION_INTERVAL,		"RepeatedAbsorptionInterval")	\
	X(VE_MAXIMUM_ABSORPTION_DURATION,		"MaximumAbsorptionTime")		\
	X(VE_CHARGE_CHARACTERISTIC,				"ChargeCharacteristic")			\
	X(VE_U_BAT_LOW_LIMIT_FOR_INVERTER,		"InverterDcShutdownVoltage")	\
	X(VE_U_BAT_LOW_HYSTERESIS_FOR_INVERTER,	"InverterDcRestartVoltage")		\
																			\
	X(VE_LOWEST_ACCEPTABLE_U_MAINS,			"AcLowSwitchInputOff")			\
	X(VE_HYST_FOR_LOWEST_ACCEPTED_U_MAINS,	"AcLowSwitchInputOn")			\
	X(VE_HIGHEST_ACCEPTABLE_U_MAINS,		"AcHighSwitchInputOn")			\
	X(VE_HYST_FOR_HIGHEST_ACCEPTED_U_MAINS, "AcHighSwitchInputOff")			\
	X(VE_ASSIST_CURRENT_BOOST_FACTOR,		"AssistCurrentBoostFactor")		\
	X(VE_I_MAINS_LIMIT_AC2,					"SecondInputCurrentLimit")		\
	X(VE_CURRENT_FOR_SWITCHING_TO_AES,		"LoadForStartingAesMode")		\
	X(VE_HYST_FOR_AES_CURRENT,				"OffsetForEndingAesMode")		\
																			\
	X(VE_U_BAT_LOW_PRE_ALARM_OFFSET,		"LowDcAlarmLevel")				\
	X(VE_BATTERY_CAPACITY,					"BatteryCapacity")				\
	X(VE_BATTERY_CHARGE_PERCENTAGE,			"SocWhenBulkfinished")			\
	X(VE_SHIFT_UBAT_START,					"FrequencyShiftUBatStart")		\
	X(VE_SHIFT_UBAT_START_DELAY,			"FrequencyShiftStartDelay")		\
	X(VE_SHIFT_UBAT_STOP,					"FrequencyShiftUBatStop")		\
	X(VE_SHIFT_UBAT_STOP_DELAY,				"FrequencyShiftStopDelay")		\

/* Note: virtual switch settings are only supported on 1xx multi's */
#define VE_DEVICE_FLAGS_VS \
	X(VE_VS_ON_BULK_PROTECTION,				"VsOnBulkProtection")			\
	X(VE_VS_ON_TEMP_PRE_ALARM,				"VsOnTemperatureWarning")		\
	X(VE_VS_ON_LOW_BAT_PRE_ALARM,			"VsOnBatteryWarning")			\
	X(VE_VS_ON_OVERLOAD_PRE_ALARM,			"VsOnOverloadWarning")			\
	X(VE_VS_ON_RIPPLE_PRE_ALARM,			"VsOnRippleWarning")			\
	X(VE_VS_OFF_TEMP_PRE_ALARM,				"VsOnTemperatureWarning")		\
	X(VE_VS_OFF_LOW_BAT_PRE_ALARM,			"VsOnBatteryWarning")			\
	X(VE_VS_OFF_OVERLOAD_PRE_ALARM,			"VsOnOverloadWarning")			\
	X(VE_VS_OFF_RIPPLE_PRE_ALARM,			"VsOnRippleWarning")			\
	X(VE_VS_ON_WHEN_FAILURE,				"VsOnWhenFailurePresent")		\
	X(VE_VS_INVERT,							"InvertVsBehaviour")			\
	X(VE_VS2_OFF_WHEN_AC1_PRESENT,			"Vs2OffWhenAc1Available")		\
	X(VE_VS2_INVERT,						"InvertVs2Behaviour")			\
	X(VE_INVERTER_PERIOD_BY_VS,				"FrequencyChangesByVs")			\
	X(VE_INVERTER_PERIOD_BY_UBAT,			"FrequencyChangesByUBat")


#define VE_DEVICE_FLAGS \
	X(VE_60HZ,								"Is60Hz")						\
	X(VE_DISABLE_WAVE_CHECK,				"DisableWaveCheck")				\
	X(VE_DONT_STOP_AFTER_10H_BULK,			"DontStopAfter10hBulk")			\
	X(VE_ASSIST_ENABLED,					"PowerAssistEnabled")			\
	X(VE_DISABLE_CHARGE,					"DisableCharger")				\
	X(VE_DISABLE_AES,						"DisableAes")					\
	X(VE_ENABLE_REDUCED_FLOAT,				"EnableReducedFloat")			\
	X(VE_DISABLE_GROUND_RELAY,				"DisableGroundRelay")			\
	X(VE_WEAK_AC_INPUT,						"WeakAcInput")					\
	X(VE_REMOTE_OVERRULES_AC2,				"RemoteOverrulesAc2")			\
	X(VE_WIDE_INPUT_FREQ,					"AcceptWideInputFrequency")		\
	X(VE_DYNAMIC_CURRENT_LIMIT,				"DynamicCurrentLimit")			\
	X(VE_USE_TUBULAR_PLATE_CURVE,			"TabularPlateTractionCurve")	\
	X(VE_REMOTE_OVERRULES_AC1,				"RemoteOverrulesAc1")			\
	X(VE_LOW_POWER_SHUTDOWN_IN_AES,			"LowPowerShutdownInAes")		\


#define VE_AC_CONFIG_VALUES													\
	X(VE_SYS_CURRENT_LIMIT_IS_ADJUSTABLE,	"Ac/ActiveIn/CurrentLimitIsAdjustable") \


#define VE_APP_SPECIFIC_VALUES												\
	X(VE_SYS_STATE,							"State")						\
	X(VE_SYS_MODE,							"Mode")							\
	X(VE_SYS_MODE_IS_ADJUSTABLE,			"ModeIsAdjustable")			\
	X(VE_SYS_AC_IN_LIM,						"Ac/ActiveIn/CurrentLimit")		\
	X(VE_I_BAT_BULK,						"Dc/MaxChargeCurrent")			\


/*
 * VE_SYSTEM_AC_VARS_ACIN1				\
 * VE_SYSTEM_AC_VARS_ACIN2				\
 */

#define VE_SYSTEM_AC_ITEMS				\
	VE_SYSTEM_AC_VARS					\
	VE_SYSTEM_AC_VARS_ACIN				\
	VE_SYSTEM_AC_VARS_ACOUT1			\

#define VE_SYSTEM_INVALIDATE_ITEMS		\
	VE_SYSTEM_AC_ITEMS					\
	VE_SYSTEM_DC_VARS					\
	VE_SYSTEM_KWH_COUNTERS				\
	VE_SYSTEM_STATE						\
	VE_SYSTEM_AC_SENSORS				\
	VE_SYSTEM_ALARMS					\
	VE_SYSTEM_GENERAL					\
	VE_SYSTEM_LEDS						\
	VE_SYSTEM_SOC						\
	VE_SYSTEM_CONV_VARS_VEBUS			\
	VE_AC_CONFIG_VALUES					\

#define VE_SYSTEM_CREATE_ITEMS			\
	VE_SYSTEM_STORED_DEVICE_VERSIONS	\
	VE_SYSTEM_INVALIDATE_ITEMS			\
	VE_APP_SPECIFIC_VALUES				\

#define X(a,b)	VeItem a;

#ifdef CFG_VEBUS_DEVICE_ITEMS
typedef struct {
	VeItem settings[VEBUS_SETTINGS_COUNT];
	VeItem flags[VEBUS_FLAGS_COUNT];
} VeDeviceItems;
#endif

typedef struct VeBusItemsS
{
	struct VeBusItemsS* next;

	/* Automagically create a VeItem for all items as the define name */
	VE_SYSTEM_CREATE_ITEMS

#ifdef CFG_VEBUS_DEVICE_ITEMS
	VeDeviceItems devices[32];
#endif
} VeBusItems;

#undef X

void vebusItemsActionDone(VeBusItems *sys, struct VebusDeviceS *dev, Mk2Actions done);
void vebusItemsCreateSystem(struct VeItem *root, VeBusItems *sys);
void vebusItemsInitDone(VeBusItems *sys, struct VeItem *root, struct VebusDeviceS *masterDevice);
void vebusItemsInvalidate(VeBusItems *sys, struct VeItem *root);
void vebusItemsRamVarObtained(VeBusItems *sys, struct VebusDeviceS *dev, un8 id);
void vebusItemsSettingChanged(VeBusItems *sys, struct VebusDeviceS *dev, un8 id);
#endif
