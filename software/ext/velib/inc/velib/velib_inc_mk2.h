#ifndef _VELIB_VELIB_MK2_INC_H_
#define _VELIB_VELIB_MK2_INC_H_

/*
 * This file provides a definition for all information which
 * can be collected about a vebus device / system. If memory is
 * of any concern the structs can be reduced by defining the
 * structs in the application.
 */

#include <velib/velib_config_app.h>

#if CFG_MK2_INC_APP
#include <velib/velib_inc_mk2_app.h>
#else

#include <velib/mk2/frame_handler.h>
#include <velib/mk2/mk2.h>
#include <velib/mk2/mk2_actions.h>
#include <velib/mk2/values_float.h>
#include <velib/mk2/vebus_device.h>
#include <velib/mk2/vebus_system.h>
#include <velib/mk2/vsc.h>
#include <velib/platform/serial.h>

typedef struct Mk2S
{
	/* version of the mk2 itself */
	un32 version;
	/* state of the mk2 itself */
	Mk2State state;
	/* variables for the mk2 frame handler */
	Mk2FhVars fh;
	/* last address the mk2 is going to set to */
	un8 intendedAddress;
	/* device the mk2 is set to talk to */
	un8 actualAddress;
	/* last command send to support XYZ looping */
	un8 cmd;
} Mk2;

typedef struct VebusDeviceS
{
	/* version of the vebus device */
	VebusVersion version;
	/* vebus short address used by this device */
	un8 addr;
	/* Unique id of the vebus device */
	un32 uniqueNumber;
	un8 type;
	/* Scale information of this device */
	VebusSettingInfo settingInfo[VEBUS_SETTINGS_COUNT];
	VebusRamInfo ramInfo[VEBUS_RAMVAR_COUNT];
	veBool validScales;
	veBool resetRequired;
	un8 accessLevelRequired;
	/* Readings of the device */
	un16 settings[VEBUS_SETTINGS_COUNT];
	un8 settingValidFlags[MK2_ACT_SETTINGS_FLAG_BYTES];
	un16 vars[VEBUS_RAMVAR_COUNT];
	/* Queued actions */
	Mk2DevActVars act;
	/*
	 * If settings are requested / written the normal loop is temporarily ignored.
	 * The lastAction stores where to resume the loop.
	 */
	Mk2Actions nextAction;
	/* Linked list */
	struct VebusDeviceS* next;
} VebusDevice;

typedef struct VebusSystemS
{
	/* Units used for the values */
	VeUnitSet units;
	/* State of the whole system */
	VebusState state;
	/* low bat etc */
	un16 alarms;
	/* Charge and / or inverter state */
	VebusConverterState convState;
	/* DC values, raw and floats */
	Mk2RawDc dc;
	/* Current limit as used by the whole system */
	VebusAcInConfig acInConfig;
	/* Copy of the above to keep the min/max available if it is not the active input */
	VebusAcInConfig acInConfigPerAcIn[2];
	/* AC values */
	Mk2RawAcPhase ac[3];
	/* Variables needed for quering scales of a VEbus device */
	VebusInitVars init;
	/* pending actions */
	Mk2SystemActVars act;
	/* One mk2 to interface a VEBus system */
	struct Mk2S mk2Interface;
	/* Port the mk2 is connected to */
	VeSerialPort port;
	/* One vebus system can consist out of multi devices */
	struct VebusDeviceS* devices;
	/* kwh related variables */
	VeKwhVars kwh;
	/* Workaround / features which might need to be considered */
	un16 quirks;
	/* index of the vebus system */
	int handle;

	/* Variables needed for controlling on / off and current limit */
	VebusControl ctrl;

	/* Set to true when all values are initialised */
	veBool validRuntimeValues;
	/*
	 * The vebus system can optionally contain assistants sending
	 * out the voltage setpoint. This is forwardingen to the CAN-bus in
	 * for other chargers to follow.
	 */
	un16 voltageSetpoint;

	VeCurrentSensorVars iSense;
	VeHub1Vars hub1;
	VeAssistantVars assistant;

	/* Power as obtained from the multi */
	VebusPower power;

	/* AUX devices verions (Digital Multi Control and Battery Management System) */
	un32 bmsVersion;
	un32 dmcVersion;
} VebusSystem;

#if CFG_MULTIPLE_VEBUS_SYSTEMS

#define vebusSystem				(*aVebusSystem)
#define mk2						(*aMk2)

extern struct VebusSystemS*		aVebusSystem;
extern struct Mk2S*				aMk2;

void vebusSystemSetActive(struct VebusSystemS* system);

#else

extern struct VebusSystemS vebusSystem;
extern struct Mk2S mk2;

#endif

#endif
#endif
