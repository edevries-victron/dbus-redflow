#ifndef _VELIB_MK2_VEBUS_DEVICE_H_
#define _VELIB_MK2_VEBUS_DEVICE_H_

#include <velib/base/types.h>
#include <velib/mk2/frame_handler.h>
#include <velib/mk2/vebus_values.h>

// device states
#define VE_STATE_DOWN						0x00
#define VE_STATE_STARTUP					0x01
#define VE_STATE_OFF						0x02
#define VE_STATE_SLAVE						0x03
#define VE_STATE_INVERT_FULL				0x04
#define VE_STATE_INVERT_HALF				0x05
#define VE_STATE_INVERT_AES					0x06
#define VE_STATE_POWER_ASSIST				0x07
#define VE_STATE_BYPASS						0x08
#define VE_STATE_CHARGE						0x09
#define VE_STATE_UNKNOWN					0x0A

#define VE_SUBSTATE_CHRG_INIT				0x00
#define VE_SUBSTATE_CHRG_BULK				0x01
#define VE_SUBSTATE_CHRG_ABSORPTION			0x02
#define VE_SUBSTATE_CHRG_FLOAT				0x03
#define VE_SUBSTATE_CHRG_STORAGE			0x04
#define VE_SUBSTATE_CHRG_ABSORPTION_REPEAT	0x05
#define VE_SUBSTATE_CHRG_ABSORPTION_FORCE	0x06
#define VE_SUBSTATE_CHRG_EQUALISE			0x07
#define VE_SUBSTATE_CHRG_BULK_STOPPED		0x08
#define VE_SUBSTATE_CHRG_UNKNOWN			0x09

/* These are sent */
#define VE_ALARM_FLAG_TEMPERATURE_PRE		0x01
#define VE_ALARM_FLAG_LOW_BATTERY_PRE		0x02
#define VE_ALARM_FLAG_OVERLOAD_PRE			0x04
#define VE_ALARM_FLAG_U_BAT_RIPPLE_PRE		0x08
#define VE_ALARM_FLAG_TEMPERATURE			0x10
#define VE_ALARM_FLAG_LOW_BATTERY			0x20
#define VE_ALARM_FLAG_OVERLOAD				0x40
#define VE_ALARM_FLAG_U_BAT_RIPPLE			0x80

/* Derived from led flags */
#define VE_ALARM_FLAG_VEBUS_ERROR			0x0100
#define VE_ALARM_FLAG_TEMP_SENSOR			0x0200
#define VE_ALARM_FLAG_VOLTAGE_SENSOR		0x0400

/* State derived from above flags */
typedef enum {
	VEBUS_ALARM_OK,
	VEBUS_ALARM_WARNING,
	VEBUS_ALARM_ACTIVE
} VebusAlarmState;

// Definitions of each bit in the led status messages for multi's/inverters
#define VE_LED_MAINS						0x01
#define VE_LED_ABSORPTION					0x02
#define VE_LED_BULK							0x04
#define VE_LED_FLOAT						0x08
#define VE_LED_INVERTER						0x10
#define VE_LED_OVERLOAD						0x20
#define VE_LED_LOW_BATTERY					0x40
#define VE_LED_TEMPERATURE					0x80

#define VE_LED_ON(i, a)			(((i)->leds & (a)) == (a) && (((i)->ledsBlink & (a)) == 0))
#define VE_LED_BLINK(i, a)		(((i)->leds & (a)) == 0 && (((i)->ledsBlink & (a)) == (a)))
#define VE_LED_ANTI(i,a)		(((i)->leds & (a)) == (a) && (((i)->ledsBlink & (a)) == (a)))

// Winmon commands
#define	WM_CMD_SEND_LED_STATUS				0x02
#define	WM_CMD_SWITCH						0x03
#define	WM_CMD_SHORE_CURRENT_SETPOINT		0x04
#define	WM_CMD_SEND_SOFTWARE_VERSION_PART0	0x05
#define	WM_CMD_SEND_SOFTWARE_VERSION_PART1	0x06

#define	WM_CMD_SWITCH_AND_SHORE_AND_LEDS	0x0C
#define	WM_CMD_GET_SWITCH_AND_SHORE			0x0D
#define	WM_CMD_GET_SET_DEVICE_STATE			0x0E

#define	WM_CMD_READ_RAMVAR					0x30
#define	WM_CMD_READ_SETTING					0x31
#define	WM_CMD_WRITE_RAMVAR					0x32
#define	WM_CMD_WRITE_SETTING				0x33
#define	WM_CMD_WRITE_DATA					0x34
#define	WM_CMD_GET_SETTING_INFO				0x35
#define	WM_CMD_GET_RAMVAR_INFO				0x36
#define	WM_CMD_WRITE_VIA_ID					0x37

#define WM_CMD_ACCESS_PASSWORD				0x3A
#define WM_CMD_GET_SERIAL_NUMBER			0x3B
#define WM_CMD_GET_EXTRA_FLAG_ACCESS_INFO	0x3C

// flags of cmd write via id
#define	WM_FLAG_WRITE_RAMVAR				0x00
#define	WM_FLAG_WRITE_SETTINGS				0x01

// winmon responses
#define	WM_RSP_UNKNOWN_COMMAND				0x80
#define	WM_RSP_SEND_LED_STATUS				0x81
#define	WM_RSP_SEND_SOFTWARE_VERSION_PART0	0x82
#define	WM_RSP_SEND_SOFTWARE_VERSION_PART1	0x83
#define	WM_RSP_GET_SWITCH_AND_SHORE			0x92
#define	WM_RSP_GET_DEVICE_STATE				0x94
#define	WM_RSP_GET_KWH						0x98
#define	WM_RSP_SNAPSHOT						0x99

#define	WM_RSP_READ_RAM_OK					0x85
#define	WM_RSP_VAR_NOT_SUPPORTED			0x90
#define	WM_RSP_READ_SETTING_OK				0x86
#define	WM_RSP_SETTING_NOT_SUPPORTED		0x91
#define	WM_RSP_WRITE_RAMVAR_OK				0x87
#define	WM_RSP_WRITE_SETTING_OK				0x88

#define	WM_RSP_SET_SCALE					0x89
#define	WM_RSP_SET_OFFSET					0x8A
#define	WM_RSP_SET_DEFAULT					0x8B
#define	WM_RSP_SET_MINIMUM					0x8C
#define	WM_RSP_SET_MAXIMUM					0x8D

#define	WM_RSP_VAR_SCALE					0x8E
#define	WM_RSP_VAR_OFFSET					0x8F

#define WM_RSP_ACCESS_LEVEL_REQUIRED		0x9B
#define WM_RSP_ACCESS_PASSWORD_ACCEPTED		0x9C /* Passwords are valid fo 30 seconds */
#define WM_RSP_SERIAL_NUMBER				0x9D
#define WM_RSP_EXTRA_FLAG_ACCESS_INFO_LO	0x9E
#define WM_RSP_EXTRA_FLAG_ACCESS_INFO_HI	0x9F

/*
 * VebusSetting access level bit definitions
 * access levels where introduced for grid compliance. Multifirmware > 308
 */
#define VEBUS_ACCESS_LEVEL_GRID_CODE					0x01
#define VEBUS_ACCESS_LEVEL_RESET_REQUIRED				0x80 /* Change of this setting requires device reset */

/// Error codes of a VEBus device
typedef enum {
	VEBUS_ERROR_NONE = 0,
	VEBUS_ERROR_PHASE_MISSING = 1,
	VEBUS_ERROR_INCORRECT_DEV_NUMBER = 3,
	VEBUS_ERROR_ONLY_ONE_DEV = 4,
	VEBUS_ERROR_AC_OUT_OVERVOLTAGE = 5,
	VEBUS_ERROR_SYNC = 10,
	VEBUS_ERROR_SENDING = 14,
	VEBUS_ERROR_NO_DONGLE = 16,
	VEBUS_ERROR_NEW_MASTER = 17,
	VEBUS_ERROR_OVERVOLTAGE = 18,
	VEBUS_ERROR_ACT_AS_SLAVE = 22,
	VEBUS_ERROR_SWITCH_OVER = 24,
	VEBUS_ERROR_FIRMWARE_INCOMPATIBLE = 25,
	VEBUS_ERROR_INTERNAL_ERROR = 26
} VebusErrorCode;

/* Raw info needed to unpack info values */
typedef struct {
	sn16 scale;
	sn16 offset;
} VebusRamInfo;

/* Information structure about settings variables. Raw values */
typedef struct {
	sn16 scale;
	sn16 offset;
	un16 defaultValue;
	un16 minimum;
	un16 maximum;
	un8  accessLevel;
} VebusSettingInfo;

typedef struct {
	veBool valid;
	un8 flagWordId;
	un8 accessLevel[16];		/* Access levels for flags 0...15 */
} VebusExtraFlagAccessInfo;

// left to app which information is exactly of interest..
struct VebusDeviceS;

typedef struct
{
	un8 id;
	Mk2Callback* cb;
	struct VebusDeviceS* dev;
} VebusInitVars;

typedef struct {
	un8 state;
	un8 substate;
} VebusConverterState;

typedef struct {
	/*
	 * Bytes 2,3,4,5 contain the address map of configured devices
	 * Byte 7 bit 4 = VLL code not used
	 * Byte 8 contains the short address of the device
	 */
	un8 raw[9];
} VebusConfig;

/* version frame remainder */
#define VEBUS_VERSION_REMAINDER_LEN			15
#define VEBUS_VERION_F_COMBINED_FRAME		0x01
typedef struct VebusVersion {
	un32 number;
	un8 features;

#if CFG_MK2_VSC_SUPPORT
	/* information needed for generation vsc files */
	un8 remainderLength;
	un8 remainder[VEBUS_VERSION_REMAINDER_LEN];
#endif
} VebusVersion;


/* Get a vebusDevice */
void vebusDeviceAllocInit(void);
struct VebusDeviceS* vebusDeviceAlloc(void);
void vebusDeviceFree(struct VebusDeviceS*);

/** Called when a new vebus device is found */
void mk2NewDeviceFound(struct VebusDeviceS* dev);

/* Communication with the vebus device */
void vebusGetConfig(struct VebusDeviceS* dev, Mk2Callback* callback);
Mk2MsgTx *vebusGetConfigMsg(un8 addr);
void vebusGetConverterState(struct VebusDeviceS* dev, Mk2Callback* callback, VebusConverterState* info);
void vebusGetTargetVersion(struct VebusDeviceS* dev, Mk2Callback* callback, VebusVersion *version);
void vebusGetSettingInfo(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, VebusSettingInfo* ctx);
void vebusGetRamInfo(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, VebusRamInfo* dst);
void vebusGetVar(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, un16* out);
veBool vebusSetVar(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, un16 value);
void vebusGetSetting(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, un16* out);
veBool vebusSetSetting(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, un16 value);
void vebusQueryDevice(struct VebusDeviceS* dev, VeUnitSet units, Mk2Callback* callback);
veBool mk2ReqSystemVar(un8 id);
void vebusResetDevice(struct VebusDeviceS* dev, Mk2Callback* callback);
void vebusResetDevices(Mk2Callback* callback);
void vebusSendAccessPassword(struct VebusDeviceS *dev, un8 *passwd, un8 len, Mk2Callback *callback);
void vebusGetExtraFlagAccessInfo(struct VebusDeviceS *dev, un8 id, Mk2Callback *cb, VebusExtraFlagAccessInfo *ctx);
void vebusGetSerialNumber(struct VebusDeviceS *dev, Mk2Callback *cb, un8 *serialNumber);

/* Operation on the obtained information */
veBool vebusSupportedSetting(struct VebusDeviceS* dev, un8 id);
veBool vebusSupportedFlag(struct VebusDeviceS* dev, un8 id);
veBool vebusSupportedRamVar(struct VebusDeviceS* dev, un8 id);
veBool vebusSupportedSystemVar(struct VebusDeviceS* dev, un8 id);
veBool vebusSupportedVariable(struct VebusDeviceS* dev, VebusValue nr);

veBool vebusValidRawSetting(struct VebusDeviceS* dev, un8 id, un16 value);

un16 vebusGetSettingRawDefault(struct VebusDeviceS* dev, un8 id);
un16 vebusGetSettingRawMinimum(struct VebusDeviceS* dev, un8 id);
un16 vebusGetSettingRawMaximum(struct VebusDeviceS* dev, un8 id);
un16 vebusGetFlagRawDefault(struct VebusDeviceS* dev, un8 id);
sn32 vebusUnpackRam(struct VebusDeviceS* dev, sn32 raw, un8 id, un16 mul);

/* static information, if the variable exists at all (but might not be supported by this device) */
veBool vebusKnownVariable(struct VebusDeviceS* dev, VebusValue id);
VebusValueType vebusGetValueType(un16 nr);
VebusSettingInfo* vebusGetSettingInfoOfDevice(struct VebusDeviceS* dev);
VebusRamInfo* vebusGetRamInfoOfDevice(struct VebusDeviceS* dev);

un8 mk2GetNoc(struct VebusDeviceS* dev);
veBool mk2IsUS(struct VebusDeviceS* dev);

#endif
