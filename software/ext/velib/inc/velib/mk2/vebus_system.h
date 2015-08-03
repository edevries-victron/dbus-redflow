#ifndef _VELIB_MK2_VEBUS_SYSTEM_H_
#define _VELIB_MK2_VEBUS_SYSTEM_H_

#include <velib/mk2/frame_handler.h>
#include <velib/mk2/mk2.h>
#include <velib/mk2/vebus_device.h>

typedef void (*VebusControlCallback)(veBool success);

#define VE_REMOTE_AC1_IN_LIMIT			0x01

#include <velib/base/types.h>

#define VEBUS_MAX_PHASES 3

/// System values for an AC phase a received from vebus (unscaled)
typedef struct
{
	un8 backfeedFactor;		///< Multiplier for AC current in.
	un8 inverterFactor;		///< Multiplier for inverter current out.
	/**
	 * Indicates what phase the received data subsribes and in case of L1, how many
	 * phases are present in the system (1 -> 1 phase).
	 */
	un8 phaseCount;
	un16 inputV;
	un16 inputI;
	un8 inputPeriod;
	un16 inverterV;
	sn16 inverterI;
} Mk2RawAcPhase;

/* Only newer multi firmware reports the power, so flag if supported */
#define VE_POWER_VALID				0x01
#define VE_SNAPSHOT_NOT_SUPPORTED	0x02

/* Real power, only new firmware, state reports if valid */
typedef struct {
	sn32 input[3];
	sn32 inverter[3];
	sn32 output[3];
	un8 flags;
	VeAcPowerMeasurement measurementType;
} VebusPower;

typedef struct
{
	un16 V;
	un32 inverterI;
	un32 chargerI;
	un8 period;
} Mk2RawDc;

typedef struct
{
	un16 min;
	un16 max;
	un16 limit;
	un8 acInputConfig;
} VebusAcInConfig;

typedef enum
{
	SP_UNKNOWN,
	SP_CHARGER_ONLY,
	SP_FIRST = SP_CHARGER_ONLY,
	SP_INVERTER_ONLY,
	SP_SWITCH_ON,
	SP_SWITCH_OFF,
	SP_LAST = SP_SWITCH_OFF
} VebusSwitchPos;

typedef enum
{
	VEBUS_PANEL_DISABLED,
	VEBUS_PANEL_INIT,
	VEBUS_PANEL_SYNC,
	VEBUS_PANEL_TAKE_OVER,
	VEBUS_PANEL_CONTROL
} VebusControlState;

typedef struct
{
	VebusControlState state;
	/*
	 * Temp storage to park the new mode while trying to take over control of the multi.
	 * The actual mode lives in mk2.state.mode.
	 */
	VebusSwitchPos intendedMode;
	un8 timer;
	un8 panelFrameReceived;
	un8 vebusPowerTimer;
	VebusControlCallback controlCb;
	un8 panelTimer;
} VebusControl;

typedef enum {
	VE_KWH_IDLE,
	VE_KWH_BUSY,
	VE_KWH_CHECK_NEXT_DEVICE,
	VE_KWH_NOT_SUPPORTED
} VeKwhState;

/* variables related to the current sensor assistant */
typedef struct
{
	/* device the sensor is connected to */
	struct VebusDeviceS *dev;
	un8 ramVarOffset;
	/* energy counters in 2^16 Joule */
	un16 kwhCounter;	///< looping counter
	un32 kwhTotal;		///< total
	un16 current;
	un16 voltage;
	un16 infoFlags;
} VeCurrentSensor;

#define VEBUS_MAX_CURRENT_SENSORS	4
typedef struct
{
	VeCurrentSensor sensors[VEBUS_MAX_CURRENT_SENSORS];
	Mk2Callback *callback;
	un16 tmpCounter;
	un8 count; /* number of current sensor found */
	un8 pollSensor;
} VeCurrentSensorVars;

/*
 * Variables related to the hub1 assistant.
 * Note: the voltage setpoint is in vebusSystem itself, anticipating that
 * there will be more then one assistant forwarding voltage setpoints.
 */
typedef struct
{
	/* device the sensor is connected to */
	struct VebusDeviceS *dev;
	un8 ramVarOffset; /* note: 0 is not a hub1 */
} VeHub1Vars;

typedef enum {
	VE_KWH_COUNTERS_NUM = 10
} VeKwhCounter;

typedef struct {
	/* looping kwh counter in 2^16 J of this device */
	un8 counters[32][VE_KWH_COUNTERS_NUM];
	/* looping kwh counter in 2^16 J of this device */
	un32 totals[VE_KWH_COUNTERS_NUM];
	/* whether all initial counter values are obtained */
	veBool initialized;
	/* update state */
	VeKwhState state;
	/* address currently being updated */
	un8 address;
} VeKwhVars;

typedef struct {
	struct VebusDeviceS *pollDev;
	/* RAM location to scan */
	un8 scanId;
	un16 identifier;
} VeAssistantVars;

#define VEBUS_QUIRK_A_SLAVE_REPORTS_INVALID_POWER	0x0001

veBool vebusCanBeControlled(void);
veBool vebusAssistantsPoll(Mk2Callback *callback);
veBool vebusCurrentSensorsMoreTodo(void);
void vebusGetAcPhase(un8 phase, Mk2RawAcPhase* out, Mk2Callback* callback);
veBool vebusGetCurrentSensors(Mk2Callback *callback);
void vebusGetDcValues(Mk2RawDc* dc, Mk2Callback* callback);
void vebusGetAcInConfig(VebusAcInConfig *info, Mk2Callback* callback);
veBool vebusKwhMoreTodo(void);
veBool vebusKwhUpdate(Mk2Callback* callback);
void vebusSendPanelFrame(Mk2Callback* callback);
void vebusControlInit(void);
void vebusControlUpdate(void);
void vebusControlTick(void);
void vebusControlDevice(VebusControlCallback cb, VebusSwitchPos mode);
un16 vebusAlarmsFromState(VebusState* info);
un8 vebusActiveIn(VebusAcInConfig* config);
veBool vebusActiveCurrentLimitChangable(VebusAcInConfig* config);
void vebusSetModeVariable(VebusSwitchPos mode);
veBool vebusGetPower(Mk2Callback *callback);
void vebusGetShortIds(Mk2Callback *callback);

/* Note on rx should call this function is control is wanted */
Mk2EventReply vebusControlHandler(Mk2Event mk2event, Mk2MsgRx *msg);

/* These should be implemented, since these depend on the settings */
un16 veGetCurrentLimitCb(void);
void vebusModeChanged(VebusSwitchPos mode);
void vebusCurrentLimitChanged(un8 acin, un16 limit);
veBool mk2HasAcIn(struct VebusDeviceS* dev, un8 n);
void vebusQueuePanelFrameCallback(void);

void vebusAssistantFoundCallback(struct VebusDeviceS *dev, un8 assistentId, un8 ramVarOffset, un8 ramvarCount);

#endif
