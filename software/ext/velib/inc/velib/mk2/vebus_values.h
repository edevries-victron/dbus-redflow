#ifndef _VELIB_MK2_UNITS_H_
#define _VELIB_MK2_UNITS_H_

struct VebusVersion;

/*
 * Currently there are two units systems.
 * The values as unpacked by the downloaded values (UNITS_MULTI)
 * and the units used in the vsc file (which is how VeConfigure shows
 * the values to the user.
 */
typedef enum {
	VE_UNITS_MULTI,
	VE_UNITS_VSC
} VeUnitSet;

typedef enum {
	VE_U_NR,
	VE_U_V_DC,
	VE_U_A_DC,
	VE_U_V_AC,
	VE_U_A_AC,
	VE_U_W,
	VE_U_VA,
	VE_U_HZ,
	VE_U_MSEC,
	VE_U_DSEC,
	VE_U_SEC,
	VE_U_MIN,
	VE_U_PERC,
	VE_U_AH,
	VE_U_J16,		// energy = (value << 16) J = (value * 65536) J / = value * 65536 / 3600 / 1000 kWh
	VE_U_KWH,
	VE_U_STR,
	VE_U_FLAG,
	VE_U_DFLAG,
	VE_U_FLAGS,
	VE_U_PERC_FRAC,
	VE_U_SELECT,
	VE_U_INVALID = 0x7FFF
} VebusUnit;

#define VE_U_MASK 0x00FF

/* bits 15-14 are used to identify the phase a a value belongs to */
#define VE_A		0x4000
#define VE_B		0x8000
#define VE_C		0xC000

/* bits 13-11 define the connection it belongs to */
typedef enum {
	VE_ACIN1 = 1,
	VE_ACIN2,
	VE_ACIN3,
	VE_ACIN4,
	VE_ACIN,	/* active */
	VE_ACOUT
} VeAcConn;

#define VE_ACIN1_F		( VE_ACIN1 << 11)
#define VE_ACIN2_F		( VE_ACIN2 << 11)
#define VE_ACIN3_F		( VE_ACIN3 << 11)
#define VE_ACIN4_F		( VE_ACIN4 << 11)
#define VE_ACIN_F		( VE_ACIN << 11)
#define VE_ACOUT_F		( VE_ACOUT << 11)

#define VEBUS_FLAGS_OFFSET					0x100
#define VEBUS_FLAGS_ALLOCATED				64
#define VEBUS_RAMVAR_OFFSET					0x200
#define VEBUS_SYS_OFFSET					0x300
#define VEBUS_ID_INVALID					0xFF

/// Ram ID table as defined in the vebus documentation
#define VEBUS_RAMVARS																		\
	X(VE_VAR_UINPUT_RMS,					"AC Input Voltage",				VE_U_V_AC)		\
	X(VE_VAR_IINPUT_RMS,					"AC Input Current",				VE_U_A_AC)		\
	X(VE_VAR_UINVERTER_RMS,					"AC Output Voltage",			VE_U_V_AC)		\
	X(VE_VAR_IINVERTER_RMS,					"AC Output Current",			VE_U_A_AC)		\
	X(VE_VAR_UBAT,							"DC Voltage",					VE_U_V_DC)		\
	X(VE_VAR_IBAT,							"DC Current",					VE_U_A_DC)		\
	X(VE_VAR_UBAT_RMS,						"DC Rimple",					VE_U_V_DC)		\
	X(VE_VAR_INVERTER_PERIOD,				"Output period",				VE_U_DSEC)		\
	X(VE_VAR_AC_INPUT_PERIOD,				"Input period",					VE_U_DSEC)		\
	X(VE_VAR_SIGNED_ILOAD,					"AC Output Current (signed)",	VE_U_A_AC)		\
	X(VE_VAR_VIRTUAL_SWITCH_POSITION,		"Virtual switch",				VE_U_NR)		\
	X(VE_VAR_IGNORE_AC_INPUT_STATE,			"Ignore ac input",				VE_U_NR)		\
	X(VE_VAR_MULTI_FUNCTIONAL_RELAY_STATE,	"Functional relay state",		VE_U_NR)		\
	X(VE_VAR_SOC,							"State of charge",				VE_U_PERC_FRAC) \
	X(VE_VAR_PINVERTER,						"Inverter Power",				VE_U_W)			\
	X(VE_VAR_PINPUT,						"Input Power",					VE_U_W)			\
	X(VE_VAR_POUTPUT,						"Output Power",					VE_U_W  /* v3.xx only */)	\
	X(VE_VAR_PINVERTER_INSTANT,				"Instantaneous Inverter Power",	VE_U_W  /* v3.xx only */)	\
	X(VE_VAR_PINPUT_INSTANT,				"Instantaneous Input Power",	VE_U_W  /* v3.xx only */)	\
	X(VE_VAR_POUTPUT_INSTANT,				"Instantaneous Output Power",	VE_U_W  /* v3.xx only */)

/*
 * Changes to the following settings only become active after a reset:
 *  - VE_CURRENT_FOR_SWITCHING_TO_AES
 *  - VE_HYST_FOR_AES_CURRENT
 *  - VE_BATTERY_CAPACITY
 *  - VE_BATTERY_CHARGE_PERCENTAGE
 */
/* settings  as defined in the vebus documentation */
#define VEBUS_SETTINGS																		\
	X(VE_FLAGS_0,							"flags0",						VE_U_FLAGS)		\
	X(VE_FLAGS_1,							"flags1",						VE_U_FLAGS)		\
	X(VE_U_BAT_ABSORPTION,					"Absorption voltage",			VE_U_V_DC)		\
	X(VE_U_BAT_FLOAT,						"Float voltage",				VE_U_V_DC)		\
	X(VE_I_BAT_BULK,						"Charge current",				VE_U_A_DC)		\
	X(VE_U_INV_SETPOINT,					"Inverter output voltage",		VE_U_V_AC)		\
	X(VE_I_MAINS_LIMIT_AC1,					"AC current limit",				VE_U_A_AC)		\
	X(VE_REPEATED_ABSORPTION_TIME,			"Repeated absorption time",		VE_U_MIN)		\
	X(VE_REPEATED_ABSORPTION_INTERVAL,		"Repeated absorption interval", VE_U_MIN)		\
	X(VE_MAXIMUM_ABSORPTION_DURATION,		"Maximum absorption time",		VE_U_MIN)		\
	X(VE_CHARGE_CHARACTERISTIC,				"Charge Characteristics",		VE_U_SELECT)	\
	X(VE_U_BAT_LOW_LIMIT_FOR_INVERTER,		"Inverter DC shutdown voltage", VE_U_V_DC)		\
	X(VE_U_BAT_LOW_HYSTERESIS_FOR_INVERTER,	"Inverter DC restart voltage",	VE_U_V_DC)		\
	X(VE_NUMBER_OF_SLAVES_CONNECTED,		"Number of slaves",				VE_U_NR)		\
	X(VE_SPECIAL_THREE_PHASE_SETTING,		"Special three phase",			VE_U_SELECT)	\
																							\
	X(VE_VS_USAGE,							"Virtual switch usage",			VE_U_SELECT)	\
	X(VE_VS_ON_I_INV_HIGH,					"VS on load higher then",		VE_U_W)			\
	X(VE_VS_ON_U_BAT_HIGH,					"VS on when udc lower then",	VE_U_V_DC)		\
	X(VE_VS_ON_U_BAT_LOW,					"VS on when udc higher then",	VE_U_V_DC)		\
	X(VE_VS_TON_I_INV_HIGH,					"VS time on load high",			VE_U_SEC)		\
	X(VE_VS_TON_U_BAT_HIGH,					"VS time on DC voltage high",	VE_U_SEC)		\
	X(VE_VS_TON_U_BAT_LOW,					"VS time on DC voltage low",	VE_U_SEC)		\
	X(VE_VS_TON_NOT_CHARGING,				"VS time on when not charging",	VE_U_SEC)		\
	X(VE_VS_TON_FAN_ON,						"VS time on fan enabled",		VE_U_SEC)		\
	X(VE_VS_TON_TEMPERATURE_ALARM,			"VS time on temperature alarm", VE_U_SEC)		\
	X(VE_VS_TON_LOW_BATTERY_ALARM,			"VS time on low battery alarm", VE_U_SEC)		\
	X(VE_VS_TON_OVARLOAD_ALARM,				"VS time on overload alarm",	VE_U_SEC)		\
	X(VE_VS_TON_U_BATT_RIPPLE_ALARM,		"VS time on ripple alarm",		VE_U_SEC)		\
	X(VE_VS_OFF_I_INV_LOW,					"VS off load lower then",		VE_U_W)			\
	X(VE_VS_OFF_U_BAT_HIGH,					"VS off when udc lower then",	VE_U_V_DC)		\
	X(VE_VS_OFF_U_BAT_LOW,					"VS off when udc higher then",	VE_U_V_DC)		\
	X(VE_VS_TOFF_I_INV_LOW,					"VS time off load low",			VE_U_SEC)		\
	X(VE_VS_TOFF_U_BATT_HIGH,				"VS time off DC voltage high",  VE_U_SEC)		\
	X(VE_VS_TOFF_U_BATT_LOW,				"VS time off DC voltage low",	VE_U_SEC)		\
	X(VE_VS_TOFF_CHARGING,					"VS off when charging for",		VE_U_SEC)		\
	X(VE_VS_TOFF_FAN_OFF,					"VS time off fan disabled",		VE_U_SEC)		\
	X(VE_VS_TOFF_CHARGE_BULK_FINISHED,		"VS off bulk finished for",		VE_U_MIN)		\
	X(VE_VS_TOFF_NO_VSON_CONDITION,			"VS off no ON condition for",	VE_U_MIN)		\
	X(VE_VS_TOFF_NO_AC_INPUT,				"VS off when no AC Input for",	VE_U_SEC)		\
	X(VE_VS_TOFF_TEMPERATURE_ALARM,			"VS off no temperature alarm",	VE_U_SEC)		\
	X(VE_VS_TOFF_LOW_BATTERY_ALARM,			"VS off no battery alarm",		VE_U_SEC)		\
	X(VE_VS_TOFF_OVERLOAD_ALARM,			"VS off no overload alarm",		VE_U_SEC)		\
	X(VE_VS_TOFF_U_BATT_RIPPLE_ALARM,		"VS off no ripple alarm",		VE_U_SEC)		\
	X(VE_VS_MINIMUM_ON_TIME,				"minimum on time",				VE_U_MIN)		\
																							\
	X(VE_LOWEST_ACCEPTABLE_U_MAINS,			"AC low switch input off",		VE_U_V_AC)		\
	X(VE_HYST_FOR_LOWEST_ACCEPTED_U_MAINS,	"AC low switch input on",		VE_U_V_AC)		\
	X(VE_HIGHEST_ACCEPTABLE_U_MAINS,		"AC high switch input on",		VE_U_V_AC)		\
	X(VE_HYST_FOR_HIGHEST_ACCEPTED_U_MAINS, "AC high switch input off",		VE_U_V_AC)		\
	X(VE_ASSIST_CURRENT_BOOST_FACTOR,		"Assist current boost factor",	VE_U_NR)		\
	X(VE_I_MAINS_LIMIT_AC2,					"Second input current limit",	VE_U_A_AC)		\
	X(VE_CURRENT_FOR_SWITCHING_TO_AES,		"Load for starting AES mode",	VE_U_W)			\
	X(VE_HYST_FOR_AES_CURRENT,				"Offset for ending AES mode",	VE_U_W)			\
																							\
	X(VE_VS_2_ON_I_LOAD_HIGH_LEVEL,			"VS2 on load higher then",		VE_U_W)			\
	X(VE_VS_2_ON_I_LOAD_HIGH_TIME,			"VS2 time on load high",		VE_U_SEC)		\
	X(VE_VS_2_ON_U_BAT_LOW_LEVEL,			"VS2 on lower DC voltage then",	VE_U_V_DC)		\
	X(VE_VS_2_ON_U_BAT_LOW_TIME,			"VS2 on low DC voltage for",	VE_U_SEC)		\
	X(VE_VS_2_OFF_LOAD_LOW_LEVEL,			"VS2 off load lower then",		VE_U_W)			\
	X(VE_VS_2_OFF_LOAD_LOW_TIME,			"VS2 off load lower for",		VE_U_MIN)		\
	X(VE_VS_2_OFF_U_BAT_HIGH_LEVEL,			"VS2 off DC voltage high",		VE_U_V_DC)		\
	X(VE_VS_2_OFF_U_BAT_HIGH_TIME,			"VS2 off DC voltage for",		VE_U_MIN)		\
																							\
	X(VE_FLAGS_2,							"flags2",						VE_U_FLAGS)		\
	X(VE_FLAGS_3,							"flags3",						VE_U_FLAGS)		\
																							\
	X(VE_VS_INVERTER_PERIOD_TIME,			"System period",				VE_U_MSEC)		\
																							\
	X(VE_U_BAT_LOW_PRE_ALARM_OFFSET,		"Low DC alarm level",			VE_U_V_DC)		\
	X(VE_BATTERY_CAPACITY,					"Battery capacity",				VE_U_AH)		\
	X(VE_BATTERY_CHARGE_PERCENTAGE,			"SOC when Bulk finished",		VE_U_PERC)		\
	X(VE_SHIFT_UBAT_START,					"Frequency shift ubat start",	VE_U_V_DC)		\
	X(VE_SHIFT_UBAT_START_DELAY,			"Frequency shift start delay",	VE_U_SEC)		\
	X(VE_SHIFT_UBAT_STOP,					"Frequency shift ubat stop",	VE_U_V_DC)		\
	X(VE_SHIFT_UBAT_STOP_DELAY,				"Frequency shift stop delay",	VE_U_SEC)		\
	X(VE_VS2_ON_SOC,						"VS2 on soc level",				VE_U_PERC)

/* Flags as defined in the vebus documentation */
#define VEBUS_FLAGS																			\
	X(VE_MULTI_PHASE_SYSTEM,				"multi phase system",			VE_U_FLAG)		\
	X(VE_MULTI_PHASE_LEADER,				"multi phase leader",			VE_U_FLAG)		\
	X(VE_60HZ,								"60Hz",							VE_U_FLAG)		\
	X(VE_DISABLE_WAVE_CHECK,				"disable wave check",			VE_U_FLAG)		\
	X(VE_DONT_STOP_AFTER_10H_BULK,			"don't stop after 10h bulk",	VE_U_FLAG)		\
	X(VE_ASSIST_ENABLED,					"power assist enabled",			VE_U_FLAG)		\
	X(VE_DISABLE_CHARGE,					"disabled charger",				VE_U_FLAG)		\
	X(VE_DISABLE_WAVE_CHECK_INV,			"disable wave checked (inv)",	VE_U_FLAG)		\
	X(VE_DISABLE_AES,						"disable AES",					VE_U_FLAG)		\
	X(VE_NOT_PROMOTED_1,					"don't change me!",				VE_U_DFLAG)		\
	X(VE_NOT_PROMOTED_2,					"don't change me!",				VE_U_DFLAG)		\
	X(VE_ENABLE_REDUCED_FLOAT,				"enable reduced float",			VE_U_FLAG)		\
	X(VE_NOT_PROMOTED_3,					"don't change me!",				VE_U_DFLAG)		\
	X(VE_DISABLE_GROUND_RELAY,				"disable ground relay",			VE_U_FLAG)		\
	X(VE_WEAK_AC_INPUT,						"weak AC input",				VE_U_FLAG)		\
	X(VE_REMOTE_OVERRULES_AC2,				"remote overrules AC2",			VE_U_FLAG)		\
																							\
	X(VE_VS_ON_BULK_PROTECTION,				"VS on bulk protection",		VE_U_FLAG)		\
	X(VE_VS_ON_TEMP_PRE_ALARM,				"VS on temperature warning",	VE_U_FLAG)		\
	X(VE_VS_ON_LOW_BAT_PRE_ALARM,			"VS on battery warning",		VE_U_FLAG)		\
	X(VE_VS_ON_OVERLOAD_PRE_ALARM,			"VS on overload warning",		VE_U_FLAG)		\
	X(VE_VS_ON_RIPPLE_PRE_ALARM,			"VS on ripple warning",			VE_U_FLAG)		\
	X(VE_VS_OFF_TEMP_PRE_ALARM,				"VS on temperature warning",	VE_U_FLAG)		\
	X(VE_VS_OFF_LOW_BAT_PRE_ALARM,			"VS on battery warning",		VE_U_FLAG)		\
	X(VE_VS_OFF_OVERLOAD_PRE_ALARM,			"VS on overload warning",		VE_U_FLAG)		\
	X(VE_VS_OFF_RIPPLE_PRE_ALARM,			"VS on ripple warning",			VE_U_FLAG)		\
	X(VE_VS_ON_WHEN_FAILURE,				"VS on when failure present",	VE_U_FLAG)		\
	X(VE_VS_INVERT,							"invert VS behaviour",			VE_U_FLAG)		\
																							\
	X(VE_WIDE_INPUT_FREQ,					"accept wide input frequency",	VE_U_FLAG)		\
	X(VE_DYNAMIC_CURRENT_LIMIT,				"dynamic current limit",		VE_U_FLAG)		\
	X(VE_USE_TUBULAR_PLATE_CURVE,			"tabular plate traction curve",	VE_U_FLAG)		\
	X(VE_REMOTE_OVERRULES_AC1,				"remote overrules AC1",			VE_U_FLAG)		\
	X(VE_LOW_POWER_SHUTDOWN_IN_AES,			"low power shutdown in AES",	VE_U_FLAG)		\
																							\
	X(VE_VS2_OFF_WHEN_AC1_PRESENT,			"VS2 off when AC1 available",	VE_U_FLAG)		\
	X(VE_VS2_INVERT,						"invert vs2 behaviour",			VE_U_FLAG)		\
	X(VE_INVERTER_PERIOD_BY_VS,				"Frequency changes by VS",		VE_U_FLAG)		\
	X(VE_INVERTER_PERIOD_BY_UBAT,			"Frequency changes by UBat",	VE_U_FLAG)

/// Vebus system values (defined here, and can be freely added)
#define VEBUS_SYSTEM																		\
	X(VE_SYS_MK2_VERSION,					"Mk2 version",					VE_U_NR)		\
	X(VE_SYS_VEBUS_VERSION,					"VE.Bus version",				VE_U_NR)		\
	X(VE_SYS_PRODUCT_NAME,					"Product Name",					VE_U_STR)		\
																							\
	X(VE_SYS_CURRENT_LIMIT_IS_ADJUSTABLE,	"Current limit is adjustable",	VE_U_NR)		\
	X(VE_SYS_MODE_IS_ADJUSTABLE,			"Mode is adjustable",			VE_U_NR)		\
	X(VE_SYS_DMC_VERSION,					"Multi control version",		VE_U_NR)		\
	X(VE_SYS_BMS_VERSION,					"BMS version",					VE_U_NR)		\
																							\
	X(VE_SYS_LEDS,							"Leds",							VE_U_NR)		\
	X(VE_SYS_LEDS_BLINK,					"Leds blink",					VE_U_NR)		\
																							\
	X(VE_SYS_LED_MAINS,						"Led Mains",					VE_U_NR)		\
	X(VE_SYS_LED_ABSORPTION,				"Led Absorption",				VE_U_NR)		\
	X(VE_SYS_LED_BULK,						"Led Bulk",						VE_U_NR)		\
	X(VE_SYS_LED_FLOAT,						"Led Float",					VE_U_NR)		\
	X(VE_SYS_LED_INVERTER,					"Led Inverter",					VE_U_NR)		\
	X(VE_SYS_LED_OVERLOAD,					"Led Overload",					VE_U_NR)		\
	X(VE_SYS_LED_LOW_BATTERY,				"Led Low Battery",				VE_U_NR)		\
	X(VE_SYS_LED_TEMPERATURE,				"Led Temperature",				VE_U_NR)		\
																							\
	X(VE_SYS_ALARMS,						"Alarms",						VE_U_FLAGS)		\
	X(VE_SYS_ALARM_LOW_BATTERY,				"Low battery alarm",			VE_U_NR)		\
	X(VE_SYS_ALARM_TEMPERATURE,				"Temperature alarm",			VE_U_NR)		\
	X(VE_SYS_ALARM_OVERLOAD,				"Overload alarm",				VE_U_NR)		\
	X(VE_SYS_ALARM_U_BAT_RIPPLE,			"Battery ripple alarm",			VE_U_NR)		\
	X(VE_SYS_ALARM_TEMP_SENSOR,				"Temperature sensor alarm",		VE_U_NR)		\
	X(VE_SYS_ALARM_VOLTAGE_SENSOR,			"Voltage sensor alarm",			VE_U_NR)		\
	X(VE_SYS_VEBUS_ERROR,					"Vebus error",					VE_U_NR)		\
	X(VE_SYS_SHORT_IDS,						"Short ids",					VE_U_NR)		\
																							\
	X(VE_SYS_MAINSTATE,						"Mainstate",					VE_U_NR)		\
	X(VE_SYS_SUBSTATE,						"Substate",						VE_U_NR)		\
	X(VE_SYS_STATE,							"State",						VE_U_NR)		\
	X(VE_SYS_MODE,							"Mode",							VE_U_NR)		\
																							\
	X(VE_SYS_UBAT,							"DC Voltage",					VE_U_V_DC)		\
	X(VE_SYS_IBAT,							"DC Current",					VE_U_A_DC)		\
	X(VE_SYS_INVERTER_PERIOD,				"Inverter period",				VE_U_DSEC)		\
	X(VE_SYS_INVERTER_FREQUENCY,			"Inverter frequency",			VE_U_HZ)		\
	X(VE_SYS_PHASES,						"Number of phases",				VE_U_NR)		\
	X(VE_SYS_BACKFEED_FACTOR,				"Backfeed factor",				VE_U_NR)		\
	X(VE_SYS_INVERTER_FACTOR,				"Inverter factor",				VE_U_NR)		\
																							\
	X(VE_SYS_AC_IN_ACTIVE,					"Used AC Input",				VE_U_NR)		\
																												\
	X(VE_SYS_AC_IN_LIM,						"ACIN current limit",			VE_U_A_AC	| VE_ACIN_F)			\
	X(VE_SYS_AC_IN_SET,						"ACIN current setpoint",		VE_U_A_AC	| VE_ACIN_F)			\
																												\
	X(VE_SYS_AC_IN1_LIM,					"ACIN1 current limit",			VE_U_A_AC	| VE_ACIN1_F)			\
	X(VE_SYS_AC_IN1_SET,					"ACIN1 current setpoint",		VE_U_A_AC	| VE_ACIN1_F)			\
																												\
	X(VE_SYS_AC_IN2_LIM,					"ACIN2 current limit (read)",	VE_U_A_AC	| VE_ACIN2_F)			\
	X(VE_SYS_AC_IN2_SET,					"ACIN2 current setpoint",		VE_U_A_AC	| VE_ACIN2_F)			\
																												\
	/* note: keep the AC In phases the same */																	\
	X(VE_SYS_AC_IN_A_V,						"ACIN A Voltage",				VE_U_V_AC	| VE_ACIN_F | VE_A)		\
	X(VE_SYS_AC_IN_A_I,						"ACIN A Current",				VE_U_A_AC	| VE_ACIN_F | VE_A)		\
	X(VE_SYS_AC_IN_A_P,						"ACIN A power",					VE_U_W		| VE_ACIN_F | VE_A)		\
	X(VE_SYS_AC_IN_A_PERIOD,				"ACIN A period",				VE_U_DSEC	| VE_ACIN_F | VE_A)		\
	X(VE_SYS_AC_IN_A_FREQUENCY,				"ACIN A frequency",				VE_U_HZ		| VE_ACIN_F | VE_A)		\
	X(VE_SYS_AC_IN_A_APPARENT_P,			"ACIN A power",					VE_U_VA		| VE_ACIN_F | VE_A)		\
																												\
	X(VE_SYS_AC_IN_B_V,						"ACIN B Voltage",				VE_U_V_AC	| VE_ACIN_F | VE_B)		\
	X(VE_SYS_AC_IN_B_I,						"ACIN B Current",				VE_U_A_AC	| VE_ACIN_F | VE_B)		\
	X(VE_SYS_AC_IN_B_P,						"ACIN B power",					VE_U_W		| VE_ACIN_F | VE_B)		\
	X(VE_SYS_AC_IN_B_PERIOD,				"ACIN B period",				VE_U_DSEC	| VE_ACIN_F | VE_B)		\
	X(VE_SYS_AC_IN_B_FREQUENCY,				"ACIN B frequency",				VE_U_HZ		| VE_ACIN_F | VE_B)		\
	X(VE_SYS_AC_IN_B_APPARENT_P,			"ACIN A power",					VE_U_VA		| VE_ACIN_F | VE_B)		\
	\
	X(VE_SYS_AC_IN_C_V,						"ACIN C Voltage",				VE_U_V_AC	| VE_ACIN_F | VE_C)		\
	X(VE_SYS_AC_IN_C_I,						"ACIN C Current",				VE_U_A_AC	| VE_ACIN_F | VE_C)		\
	X(VE_SYS_AC_IN_C_P,						"ACIN C power",					VE_U_W		| VE_ACIN_F | VE_C)		\
	X(VE_SYS_AC_IN_C_PERIOD,				"ACIN C period",				VE_U_DSEC	| VE_ACIN_F | VE_C)		\
	X(VE_SYS_AC_IN_C_FREQUENCY,				"ACIN C frequency",				VE_U_HZ		| VE_ACIN_F | VE_C)		\
	X(VE_SYS_AC_IN_C_APPARENT_P,			"ACIN A power",					VE_U_VA		| VE_ACIN_F | VE_C)		\
	\
	X(VE_SYS_AC_IN1_A_V,					"ACIN1 A Voltage",				VE_U_V_AC	| VE_ACIN1_F | VE_A)		\
	X(VE_SYS_AC_IN1_A_I,					"ACIN1 A Current",				VE_U_A_AC	| VE_ACIN1_F | VE_A)		\
	X(VE_SYS_AC_IN1_A_P,					"ACIN1 A power",				VE_U_W		| VE_ACIN1_F | VE_A)		\
	X(VE_SYS_AC_IN1_A_PERIOD,				"ACIN1 A period",				VE_U_DSEC	| VE_ACIN1_F | VE_A)		\
	X(VE_SYS_AC_IN1_A_FREQUENCY,			"ACIN1 A frequency",			VE_U_HZ		| VE_ACIN1_F | VE_A)		\
	X(VE_SYS_AC_IN1_A_APPARENT_P,			"ACIN1 A power",				VE_U_VA		| VE_ACIN1_F | VE_A)		\
	\
	X(VE_SYS_AC_IN1_B_V,					"ACIN1 B Voltage",				VE_U_V_AC	| VE_ACIN1_F | VE_B)		\
	X(VE_SYS_AC_IN1_B_I,					"ACIN1 B Current",				VE_U_A_AC	| VE_ACIN1_F | VE_B)		\
	X(VE_SYS_AC_IN1_B_P,					"ACIN1 B power",				VE_U_W		| VE_ACIN1_F | VE_B)		\
	X(VE_SYS_AC_IN1_B_PERIOD,				"ACIN1 B period",				VE_U_DSEC	| VE_ACIN1_F | VE_B)		\
	X(VE_SYS_AC_IN1_B_FREQUENCY,			"ACIN1 B frequency",			VE_U_HZ		| VE_ACIN1_F | VE_B)		\
	X(VE_SYS_AC_IN1_B_APPARENT_P,			"ACIN1 B power",				VE_U_VA		| VE_ACIN1_F | VE_B)		\
																													\
	X(VE_SYS_AC_IN1_C_V,					"ACIN1 C Voltage",				VE_U_V_AC	| VE_ACIN1_F | VE_C)		\
	X(VE_SYS_AC_IN1_C_I,					"ACIN1 C Current",				VE_U_A_AC	| VE_ACIN1_F | VE_C)		\
	X(VE_SYS_AC_IN1_C_P,					"ACIN1 C power",				VE_U_W		| VE_ACIN1_F | VE_C)		\
	X(VE_SYS_AC_IN1_C_PERIOD,				"ACIN1 C period",				VE_U_DSEC	| VE_ACIN1_F | VE_C)		\
	X(VE_SYS_AC_IN1_C_FREQUENCY,			"ACIN1 C frequency",			VE_U_HZ		| VE_ACIN1_F | VE_C)		\
	X(VE_SYS_AC_IN1_C_APPARENT_P,			"ACIN1 C power",				VE_U_VA		| VE_ACIN1_F | VE_C)		\
																													\
	X(VE_SYS_AC_IN2_A_V,					"ACIN2 A Voltage",				VE_U_V_AC	| VE_ACIN2_F | VE_A)		\
	X(VE_SYS_AC_IN2_A_I,					"ACIN2 A Current",				VE_U_A_AC	| VE_ACIN2_F | VE_A)		\
	X(VE_SYS_AC_IN2_A_P,					"ACIN2 A power",				VE_U_W		| VE_ACIN2_F | VE_A)		\
	X(VE_SYS_AC_IN2_A_PERIOD,				"ACIN2 A period",				VE_U_DSEC	| VE_ACIN2_F | VE_A)		\
	X(VE_SYS_AC_IN2_A_FREQUENCY,			"ACIN2 A frequency",			VE_U_HZ		| VE_ACIN2_F | VE_A)		\
	X(VE_SYS_AC_IN2_A_APPARENT_P,			"ACIN2 A power",				VE_U_VA		| VE_ACIN2_F | VE_A)		\
																													\
	X(VE_SYS_AC_IN2_B_V,					"ACIN2 B Voltage",				VE_U_V_AC	| VE_ACIN2_F | VE_B)		\
	X(VE_SYS_AC_IN2_B_I,					"ACIN2 B Current",				VE_U_A_AC	| VE_ACIN2_F | VE_B)		\
	X(VE_SYS_AC_IN2_B_P,					"ACIN2 B power",				VE_U_W		| VE_ACIN2_F | VE_B)		\
	X(VE_SYS_AC_IN2_B_PERIOD,				"ACIN2 B period",				VE_U_DSEC	| VE_ACIN2_F | VE_B)		\
	X(VE_SYS_AC_IN2_B_FREQUENCY,			"ACIN2 B frequency",			VE_U_HZ		| VE_ACIN2_F | VE_B)		\
	X(VE_SYS_AC_IN2_B_APPARENT_P,			"ACIN2 B power",				VE_U_VA		| VE_ACIN2_F | VE_B)		\
																													\
	X(VE_SYS_AC_IN2_C_V,					"ACIN2 C Voltage",				VE_U_V_AC	| VE_ACIN2_F | VE_C)		\
	X(VE_SYS_AC_IN2_C_I,					"ACIN2 C Current",				VE_U_A_AC	| VE_ACIN2_F | VE_C)		\
	X(VE_SYS_AC_IN2_C_P,					"ACIN2 C Power",				VE_U_W		| VE_ACIN2_F | VE_C)		\
	X(VE_SYS_AC_IN2_C_PERIOD,				"ACIN2 C Period",				VE_U_DSEC	| VE_ACIN2_F | VE_C)		\
	X(VE_SYS_AC_IN2_C_FREQUENCY,			"ACIN2 C Frequency",			VE_U_HZ		| VE_ACIN2_F | VE_C)		\
	X(VE_SYS_AC_IN2_C_APPARENT_P,			"ACIN2 C power",				VE_U_VA		| VE_ACIN2_F | VE_C)		\
																													\
	X(VE_SYS_AC_OUT1_A_V,					"ACOUT A Voltage",				VE_U_V_AC	| VE_ACOUT_F | VE_A)		\
	X(VE_SYS_AC_OUT1_A_I,					"ACOUT A Current",				VE_U_A_AC	| VE_ACOUT_F | VE_A)		\
	X(VE_SYS_AC_OUT1_A_P,					"ACOUT A Power",				VE_U_W		| VE_ACOUT_F | VE_A)		\
	X(VE_SYS_AC_OUT1_A_FREQUENCY,			"ACOUT A Frequency",			VE_U_HZ		| VE_ACOUT_F | VE_A)		\
	X(VE_SYS_AC_OUT1_A_APPARENT_P,			"ACOUT A power",				VE_U_VA		| VE_ACOUT_F | VE_A)		\
																													\
	X(VE_SYS_AC_OUT1_B_V,					"ACOUT B Voltage",				VE_U_V_AC	| VE_ACOUT_F | VE_B)		\
	X(VE_SYS_AC_OUT1_B_I,					"ACOUT B Current",				VE_U_A_AC	| VE_ACOUT_F | VE_B)		\
	X(VE_SYS_AC_OUT1_B_P,					"ACOUT B Power",				VE_U_W		| VE_ACOUT_F | VE_B)		\
	X(VE_SYS_AC_OUT1_B_FREQUENCY,			"ACOUT B Frequency",			VE_U_HZ		| VE_ACOUT_F | VE_B)		\
	X(VE_SYS_AC_OUT1_B_APPARENT_P,			"ACOUT B power",				VE_U_VA		| VE_ACOUT_F | VE_B)		\
																													\
	X(VE_SYS_AC_OUT1_C_V,					"ACOUT C Voltage",				VE_U_V_AC	| VE_ACOUT_F | VE_C)		\
	X(VE_SYS_AC_OUT1_C_I,					"ACOUT C Current",				VE_U_A_AC	| VE_ACOUT_F | VE_C)		\
	X(VE_SYS_AC_OUT1_C_P,					"ACOUT C Power",				VE_U_W		| VE_ACOUT_F | VE_C)		\
	X(VE_SYS_AC_OUT1_C_FREQUENCY,			"ACOUT C Frequency",			VE_U_HZ		| VE_ACOUT_F | VE_C)		\
	X(VE_SYS_AC_OUT1_C_APPARENT_P,			"ACOUT C power",				VE_U_VA		| VE_ACOUT_F | VE_C)		\
																												\
	X(VE_SYS_AC_IN_P,						"AC In Power",					VE_U_W)								\
	X(VE_SYS_AC_IN_APPARENT_P,				"AC In Apparent power",			VE_U_VA)							\
	X(VE_SYS_AC_OUT1_P,						"AC Out Power",					VE_U_W)								\
	X(VE_SYS_AC_OUT1_APPARENT_P,			"AC Out Apparent power",		VE_U_VA)							\
	X(VE_SYS_DC_POWER,						"DC Power",						VE_U_W)								\
																												\
	X(VE_SYS_ENERGY_AC_IN1_TO_INVERTER,		"Energy ACIN1 to inverter",		VE_U_KWH)							\
	X(VE_SYS_ENERGY_AC_IN2_TO_INVERTER,		"Energy ACIN2 to Inverter",		VE_U_KWH)							\
	X(VE_SYS_ENERGY_AC_IN1_TO_AC_OUT,		"Energy ACIN1 to ACOUT",		VE_U_KWH)							\
	X(VE_SYS_ENERGY_AC_IN2_TO_AC_OUT,		"Energy ACIN2 to ACOUT",		VE_U_KWH)							\
	X(VE_SYS_ENERGY_INVERTER_TO_AC_IN1,		"Energy Inverter to ACIN1",		VE_U_KWH)							\
	X(VE_SYS_ENERGY_INVERTER_TO_AC_IN2,		"Energy Inverter to ACIN2",		VE_U_KWH)							\
	X(VE_SYS_ENERGY_AC_OUT_TO_AC_IN1,		"Energy ACOUT to ACIN1",		VE_U_KWH)							\
	X(VE_SYS_ENERGY_AC_OUT_TO_AC_IN2,		"Energy AC0UT to ACIN2",		VE_U_KWH)							\
	X(VE_SYS_ENERGY_INVERTER_TO_AC_OUT,		"Energy Inverter to ACOUT",		VE_U_KWH)							\
	X(VE_SYS_ENERGY_OUT_TO_INVERTER,		"Energy ACOUT to inverter",		VE_U_KWH)							\
																												\
	X(VE_SYS_AC_SENSOR_COUNT,				"Ac sensors found",				VE_U_NR)							\
																												\
	X(VE_SYS_AC_SENSOR0_VOLTAGE,			"Ac sensor 0 voltage",			VE_U_V_AC)							\
	X(VE_SYS_AC_SENSOR0_CURRENT,			"Ac sensor 0 current",			VE_U_A_AC)							\
	X(VE_SYS_AC_SENSOR0_POWER,				"Ac sensor 0 power",			VE_U_W)								\
	X(VE_SYS_AC_SENSOR0_ENERGY,				"Ac sensor 0 energy",			VE_U_KWH)							\
	X(VE_SYS_AC_SENSOR0_POSITION,			"Ac sensor 0 position",			VE_U_NR)							\
	X(VE_SYS_AC_SENSOR0_PHASE,				"Ac sensor 0 phase",			VE_U_NR)							\
																												\
	X(VE_SYS_AC_SENSOR1_VOLTAGE,			"Ac sensor 1 voltage",			VE_U_V_AC)							\
	X(VE_SYS_AC_SENSOR1_CURRENT,			"Ac sensor 1 current",			VE_U_A_AC)							\
	X(VE_SYS_AC_SENSOR1_POWER,				"Ac sensor 1 power",			VE_U_W)								\
	X(VE_SYS_AC_SENSOR1_ENERGY,				"Ac sensor 1 energy",			VE_U_KWH)							\
	X(VE_SYS_AC_SENSOR1_POSITION,			"Ac sensor 1 position",			VE_U_NR)							\
	X(VE_SYS_AC_SENSOR1_PHASE,				"Ac sensor 1 phase",			VE_U_NR)							\
																												\
	X(VE_SYS_AC_SENSOR2_VOLTAGE,			"Ac sensor 2 voltage",			VE_U_V_AC)							\
	X(VE_SYS_AC_SENSOR2_CURRENT,			"Ac sensor 2 current",			VE_U_A_AC)							\
	X(VE_SYS_AC_SENSOR2_POWER,				"Ac sensor 2 power",			VE_U_W)								\
	X(VE_SYS_AC_SENSOR2_ENERGY,				"Ac sensor 2 energy",			VE_U_KWH)							\
	X(VE_SYS_AC_SENSOR2_POSITION,			"Ac sensor 2 position",			VE_U_NR)							\
	X(VE_SYS_AC_SENSOR2_PHASE,				"Ac sensor 2 phase",			VE_U_NR)							\
																												\
	X(VE_SYS_AC_SENSOR3_VOLTAGE,			"Ac sensor 3 voltage",			VE_U_V_AC)							\
	X(VE_SYS_AC_SENSOR3_CURRENT,			"Ac sensor 3 current",			VE_U_A_AC)							\
	X(VE_SYS_AC_SENSOR3_POWER,				"Ac sensor 3 power",			VE_U_W)								\
	X(VE_SYS_AC_SENSOR3_ENERGY,				"Ac sensor 3 energy",			VE_U_KWH)							\
	X(VE_SYS_AC_SENSOR3_POSITION,			"Ac sensor 3 position",			VE_U_NR)							\
	X(VE_SYS_AC_SENSOR3_PHASE,				"Ac sensor 3 phase",			VE_U_NR)							\
																												\
	X(VE_SYS_AC_POWER_MEASUREMENT,			"AC Power measurement type",	VE_U_NR)							\
	X(VE_SYS_QUIRKS,						"Quirks",						VE_U_NR)							\


typedef enum {
	VE_SYS_AC_POWER_APPARENT_MASTER,		/* no support of power at all, using apparent power */
	VE_SYS_AC_POWER_REAL_SUMMED_MASTERS,	/* real power, but only from the phase masters, but at different points in time */
	VE_SYS_AC_POWER_REAL_SUMMED,			/* real power, from all device, but at different points in time */
	VE_SYS_AC_POWER_REAL_SNAPSHOT_MASTERS,	/* real power, from the same moment, but from phase masters only */
	VE_SYS_AC_POWER_REAL_SNAPSHOT			/* real power, from all devices and at the same moment */
} VeAcPowerMeasurement;

#define VE_SYS_ENERGY_COUNTER_FIRST_SYS		VE_SYS_ENERGY_AC_IN1_TO_INVERTER_SYS
#define VE_SYS_ENERGY_COUNTER_LAST_SYS		VE_SYS_ENERGY_OUT_TO_INVERTER_SYS

#define VE_SYS_AC_SENSOR_FIRST_SYS			VE_SYS_AC_SENSOR0_VOLTAGE_SYS
#define VE_SYS_AC_SENSOR_LAST_SYS			VE_SYS_AC_SENSOR3_PHASE_SYS
#define VE_SYS_AC_SENSOR_REG_COUNT			((VE_SYS_AC_SENSOR0_PHASE - VE_SYS_AC_SENSOR0_VOLTAGE) + 1)

#define VE_SYS_ACIN_PHASE_FIRST_SYS			VE_SYS_AC_IN_A_V_SYS
#define VE_SYS_ACIN_PHASE_LAST_SYS			VE_SYS_AC_IN2_C_APPARENT_P_SYS
#define VE_SYS_ACIN_PHASE_FIELDS			6

/* note, these are all values / settings combined */
#define X(a,b,c) a,
typedef enum
{
	VEBUS_SETTINGS
#ifndef CFG_VEBUS_SETTINGS_COUNT
	VEBUS_SETTINGS_COUNT,
#else
	VEBUS_SETTINGS_COUNT = CFG_VEBUS_SETTINGS_COUNT,
#endif
	VEBUS_LAST_SETTING = VEBUS_FLAGS_OFFSET - 1,
	VEBUS_FLAGS
	VEBUS_LAST_FLAG = VEBUS_RAMVAR_OFFSET - 1,
	VEBUS_RAMVARS
	VEBUS_RAMVARS_LAST = VEBUS_SYS_OFFSET - 1,
	VEBUS_SYSTEM
} VebusValue;
#undef X

/* Seperate flags */
#define X(a,b,c) a##_FLAG,
typedef enum {
	VEBUS_FLAGS
	VEBUS_FLAGS_COUNT
} VebusFlagsEnum;
#undef X

/*
 * This creates an enum for the RAMVARS starting at 0,
 * see the VEBUS_RAMVARS define, e.g. VE_VAR_UINPUT_RMS_RAMVAR.
 * These values are device specific, not system values.
 */
#define X(a,b,c) a##_RAMVAR,
typedef enum
{
	VEBUS_RAMVARS
	VEBUS_RAMVAR_COUNT
} VebusRamVarEnum;
#undef X

/* Seperate ramvars */
#define X(a,b,c) a##_SYS,
typedef enum
{
	VEBUS_SYSTEM
	VEBUS_SYSTEM_COUNT
} VebusSysEnum;
#undef X

#define VEBUS_VALUE_TYPES									\
	X(VEBUS_TP_SETTING,			VEBUS_SETTINGS_COUNT)		\
	X(VEBUS_TP_FLAG,			VEBUS_FLAGS_COUNT)			\
	X(VEBUS_TP_RAMVAR,			VEBUS_RAMVAR_COUNT)			\
	X(VEBUS_TP_SYSTEM,			VEBUS_SYSTEM_COUNT)

#define X(a,b) a,
typedef enum
{
	VEBUS_VALUE_TYPES
	VEBUS_TP_INVALID
} VebusValueType;
#undef X

un8 vebusGetAcIn(VebusValue nr);
VebusUnit vebusGetFlagUnit(un8 id);
un8 vebusGetPhase(VebusValue nr);
VebusUnit vebusGetUnit(VebusValue nr, VeUnitSet units);
VebusValueType vebusGetValueType(un16 nr);
un8 vebusGetSettingIdFromFlag(un8 id);
un32 mk2VersionToBcd(un32 version);
un32 mk2VersionToProductBcd(un32 version);
veBool vebusSettingChangeRequiresReset(struct VebusVersion version, un8 id);

#endif
