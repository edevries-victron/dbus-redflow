#ifndef _VELIB_MK2_MK2_ACTIONS_H_
#define _VELIB_MK2_MK2_ACTIONS_H_

#include <velib/base/types.h>
#include <velib/mk2/vebus_device.h>

#define MK2_ACT_FLAG_INVALID			0xFF

#define MK2_ACT_VAR_FLAG_BYTES			((VEBUS_RAMVAR_COUNT - 1) / 8 + 1)
#define MK2_ACT_SETTINGS_FLAG_BYTES		((VEBUS_SETTINGS_COUNT - 1) / 8 + 1)
#define MK2_ACT_FLAG_BYTES				((MK2_ACT_COUNT - 1) / 8 + 1)

#if CFG_MK2_ASSISTANT_VARS_COUNT
#define MK2_ACT_ASSISTANTS_BYTES		((CFG_MK2_ASSISTANT_VARS_COUNT - 1) / 8 + 1)
#endif

typedef struct Mk2ActIdleS {
	Mk2Callback *cb;
	struct Mk2ActIdleS *next;
} Mk2ActIdle;

typedef enum
{
	MK2_ACT_IDLE = -1,

	/* System actions / more then 1 device involved */
	MK2_ACT_SET_PANEL_STATE,
	MK2_ACT_AC_CONFIG,
	MK2_ACT_STATUS,
	MK2_ACT_CONV_STATE,
	MK2_ACT_DC_VALUES,
	MK2_ACT_AC_VALUES,
	MK2_ACT_GET_KWH_COUNTERS,
	MK2_ACT_GET_ASSISTANT_VALUES,

	/* Device specific actions */
	MK2_ACT_DEV_SET_ASSISTANT_VAR,
	MK2_ACT_DEV_SET_SETTING,
	MK2_ACT_DEV_GET_RAM_VAR,
	MK2_ACT_DEV_GET_SETTING,

	MK2_ACT_COUNT
} Mk2Actions;

typedef struct {
	un8 ramReadFlags[MK2_ACT_VAR_FLAG_BYTES];
#if CFG_MK2_ASSISTANT_VARS_COUNT
	un8 assistantWriteFlags[MK2_ACT_ASSISTANTS_BYTES];
#endif
	un8 settingWriteFlags[MK2_ACT_SETTINGS_FLAG_BYTES];
	un8 settingReadFlags[MK2_ACT_SETTINGS_FLAG_BYTES];
	un8 actions[MK2_ACT_FLAG_BYTES];
} Mk2DevActVars;

typedef struct
{
	Mk2Actions state;
	un8 varId;
	un16 varValue;
	struct VebusDeviceS* dev;
	un8 actions[MK2_ACT_FLAG_BYTES];
} Mk2SystemActVars;

void mk2ActInit(void);
veBool mk2ActUpdate(struct VebusDeviceS* dev);
void mk2ActGetRawSetting(struct VebusDeviceS* dev, un8 id);
void mk2ActGetRawFlag(struct VebusDeviceS* dev, un8 id);
void mk2ActGetVar(struct VebusDeviceS* dev, un8 id);
void mk2ActSetAssistantVar(struct VebusDeviceS* dev, un8 id);
void mk2ActSetRawSetting(struct VebusDeviceS* dev, un8 id);
void mk2ActSetFlag(struct VebusDeviceS* dev, un8 id);
void mk2ActMark(Mk2Actions action);
veBool mk2ActAreActionsQueued(struct VebusDeviceS* dev);
veBool mk2IsActionsQueued(struct VebusDeviceS* dev, Mk2Actions action);

void mk2ActObtainedRawSettingCb(struct VebusDeviceS* dev, un8 id, un16 value);
void mk2ActObtainedRawVarCb(struct VebusDeviceS* dev, un8 id, un16 value);
void mk2ActDoneCb(struct VebusDeviceS* dev, Mk2Actions action);
un16 mk2ActSetVarCb(struct VebusDeviceS* dev, un8 id);
void mk2ActSetVarDoneCb(struct VebusDeviceS* dev, un8 id);
un16 mk2ActSetRawSettingCb(struct VebusDeviceS* dev, un8 id);
void mk2ActSetSettingDoneCb(struct VebusDeviceS* dev, un8 id);

#endif
