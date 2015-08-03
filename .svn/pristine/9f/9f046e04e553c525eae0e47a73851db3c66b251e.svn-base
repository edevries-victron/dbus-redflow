#include <velib/velib_inc_mk2.h>
#include <velib/mk2/mk2_actions.h>
#include <velib/mk2/vebus_device.h>
#include <velib/mk2/vebus_system.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>

/*
 * To limit the number of frames needed to guarantee communication, only one
 * action is executed at a time. Flags are used to queue actions, so all
 * possible actions can be marked as pending while not requiring a lot of
 * resources.
 */

void mk2ActInit(void)
{
	vebusSystem.act.state = MK2_ACT_IDLE;
}

static void setFlag(un8* flags, un16 flag)
{
	flags[flag / 8] |= 1 << (flag & 0x07);
}

static void clearFlag(un8* flags, un16 flag)
{
	flags[flag / 8] &= ~(1 << (flag & 0x07));
}

static un8 isFlagSet(un8* flags, un16 nflags)
{
	un8 mask = 1;
	un8 n = 0;

	do {
		if (*flags & mask)
			return n;

		mask <<= 1;
		if (mask == 0) {
			mask = 1;
			flags++;
		}
	} while (++n <= nflags);

	return MK2_ACT_FLAG_INVALID;
}

static void currentActionDone(void)
{
	Mk2Actions done = vebusSystem.act.state;
	vebusSystem.act.state = MK2_ACT_IDLE;
	clearFlag(vebusSystem.act.actions, done);
	mk2ActDoneCb(vebusSystem.act.dev, done);
}

#ifdef DEBUG

/* debug functions to provide breakpoints */

#if CFG_MK2_KWH_COUNTERS
static void getKwhFromDeviceDone(void)
{
	veAssert(vebusSystem.act.state == MK2_ACT_GET_KWH_COUNTERS);
	currentActionDone();
}
#endif

#if CFG_WITH_CONTROL
static void sendPanelFrameDone(void)
{
	veAssert(vebusSystem.act.state == MK2_ACT_SET_PANEL_STATE);
	currentActionDone();
}
#endif

static void getDcDone(void)
{
	veAssert(vebusSystem.act.state == MK2_ACT_DC_VALUES);
	currentActionDone();
}

static void getVebusStateDone(void)
{
	veAssert(vebusSystem.act.state == MK2_ACT_STATUS);
	currentActionDone();
}

static void getConvStateDone(void)
{
	veAssert(vebusSystem.act.state == MK2_ACT_CONV_STATE);
	currentActionDone();
}

#if CFG_MK2_AC_SENSOR
static void getAssistantValuesDone(void)
{
	veAssert(vebusSystem.act.state == MK2_ACT_GET_ASSISTANT_VALUES);
	currentActionDone();
}
#endif

#else

#define sendPanelFrameDone			currentActionDone
#define getDcDone					currentActionDone
#define getVebusStateDone			currentActionDone
#define getConvStateDone			currentActionDone
#define getKwhFromDeviceDone		currentActionDone
#define getAssistantValuesDone		currentActionDone
#endif

/* Get next phase if any or start over */
static void getAcDone(void)
{
	if ((vebusSystem.act.varId + 1) >= vebusSystem.ac[0].phaseCount)
	{
		vebusSystem.act.varId = 0;
		vebusSystem.act.state = MK2_ACT_IDLE;
		clearFlag(vebusSystem.act.actions, MK2_ACT_AC_VALUES);
		mk2ActDoneCb(vebusSystem.act.dev, MK2_ACT_AC_VALUES);
		return;
	}

	vebusSystem.act.varId++;
	vebusGetAcPhase(vebusSystem.act.varId, &vebusSystem.ac[vebusSystem.act.varId], getAcDone);
}

static void reqAcInConfigDone(void)
{
	clearFlag(vebusSystem.act.actions, MK2_ACT_AC_CONFIG);
	vebusSystem.act.state = MK2_ACT_IDLE;
	mk2ActDoneCb(vebusSystem.act.dev, MK2_ACT_AC_CONFIG);
}

/** handle incoming settings */
static void getSettingDone(void)
{
	un8 varId = vebusSystem.act.varId;
	un16 varValue = vebusSystem.act.varValue;

	logI("mk2 action", "raw setting: %d %d", varId, varValue);
	clearFlag(vebusSystem.act.dev->act.settingReadFlags, varId);
	vebusSystem.act.state = MK2_ACT_IDLE;

	/* Check if all settings are obtained */
	if (isFlagSet(vebusSystem.act.dev->act.settingReadFlags, VEBUS_SETTINGS_COUNT) == MK2_ACT_FLAG_INVALID) {
		logI("mk2 action", "all settings obtained");
		clearFlag(vebusSystem.act.dev->act.actions, MK2_ACT_DEV_GET_SETTING);
	}

	mk2ActObtainedRawSettingCb(vebusSystem.act.dev, varId, varValue);
}

#if CFG_MK2_ASSISTANT_VARS_COUNT
/** handle acknowledgement of settings */
static void setRamAssistantVarDone(void)
{
	un8 varId = vebusSystem.act.varId;
	un16 varValue = vebusSystem.act.varValue;

	logI("mk2 action", "value set: %d %d", varId, varValue);
	clearFlag(vebusSystem.act.dev->act.assistantWriteFlags, varId - 0x80);

	if (isFlagSet(vebusSystem.act.dev->act.assistantWriteFlags, CFG_MK2_ASSISTANT_VARS_COUNT) == MK2_ACT_FLAG_INVALID) {
		logI("mk2 action", "all settings written");
		clearFlag(vebusSystem.act.dev->act.actions, MK2_ACT_DEV_SET_ASSISTANT_VAR);
	}

	vebusSystem.act.state = MK2_ACT_IDLE;
	mk2ActSetVarDoneCb(vebusSystem.act.dev, varId);
}
#endif

/** handle acknowledgement of settings */
static void setSettingDone(void)
{
	un8 varId = vebusSystem.act.varId;
	un16 varValue = vebusSystem.act.varValue;
	VE_UNUSED(varValue);

	logI("mk2 action", "value set: %d %d", varId, varValue);
	clearFlag(vebusSystem.act.dev->act.settingWriteFlags, varId);

	if (isFlagSet(vebusSystem.act.dev->act.settingWriteFlags, VEBUS_SETTINGS_COUNT) == MK2_ACT_FLAG_INVALID) {
		logI("mk2 action", "all settings written");
		clearFlag(vebusSystem.act.dev->act.actions, MK2_ACT_DEV_SET_SETTING);
	}

	vebusSystem.act.state = MK2_ACT_IDLE;
	mk2ActSetSettingDoneCb(vebusSystem.act.dev, varId);
}

/* handle incoming variables */
static void getVarDone(void)
{
	un8 varId = vebusSystem.act.varId;
	un16 varValue = vebusSystem.act.varValue;

	logI("mk2 action", "var: %d %d", varId, varValue);
	clearFlag(vebusSystem.act.dev->act.ramReadFlags, varId);
	vebusSystem.act.state = MK2_ACT_IDLE;

	/* Check if all ram vars are obtained */
	if (isFlagSet(vebusSystem.act.dev->act.ramReadFlags, VEBUS_RAMVAR_COUNT) == MK2_ACT_FLAG_INVALID) {
		logI("mk2 action", "all ramvars obtained");
		clearFlag(vebusSystem.act.dev->act.actions, MK2_ACT_DEV_GET_RAM_VAR);
	}

	mk2ActObtainedRawVarCb(vebusSystem.act.dev, varId, varValue);
}

/**
 * Continue to execute pending requests for the current device.
 * The functions returns veTrue if all requests for this device
 * are done. This can be used to switch to the next device.
 */
veBool mk2ActUpdate(struct VebusDeviceS* dev)
{
	if (vebusSystem.act.state != MK2_ACT_IDLE)
		return veFalse;

	if (vebusSystem.act.dev != NULL && vebusSystem.act.dev != dev) {
		logI("actions", "no valid device...");
		return veFalse;
	}

	vebusSystem.act.dev = dev;

	/* system actions */
	switch (isFlagSet(vebusSystem.act.actions, MK2_ACT_COUNT))
	{
#if CFG_WITH_CONTROL
	case MK2_ACT_SET_PANEL_STATE:
		vebusSystem.act.state = MK2_ACT_SET_PANEL_STATE;
		vebusSendPanelFrame(sendPanelFrameDone);
		return veFalse;
#endif

	case MK2_ACT_DC_VALUES:
		vebusSystem.act.state = MK2_ACT_DC_VALUES;
		vebusGetDcValues(&vebusSystem.dc, getDcDone);
		return veFalse;

	case MK2_ACT_STATUS:
		mk2GetVebusState(&vebusSystem.state, getVebusStateDone);
		vebusSystem.act.state = MK2_ACT_STATUS;
		return veFalse;

	case MK2_ACT_CONV_STATE:
		vebusSystem.act.state = MK2_ACT_CONV_STATE;
		vebusGetConverterState(dev, getConvStateDone, &vebusSystem.convState);
		return veFalse;

	case MK2_ACT_AC_VALUES:
		vebusSystem.act.state = MK2_ACT_AC_VALUES;
		vebusSystem.act.varId = 0; // phase
		vebusGetAcPhase(vebusSystem.act.varId, &vebusSystem.ac[0], getAcDone);
		return veFalse;

	case MK2_ACT_AC_CONFIG:
		vebusSystem.act.state = MK2_ACT_AC_CONFIG;
		vebusGetAcInConfig(&vebusSystem.acInConfig, reqAcInConfigDone);
		return veFalse;

#if CFG_MK2_KWH_COUNTERS
	case MK2_ACT_GET_KWH_COUNTERS:
		vebusSystem.act.state = MK2_ACT_GET_KWH_COUNTERS;
		if (!vebusKwhUpdate(getKwhFromDeviceDone))
			getKwhFromDeviceDone();
		return veFalse;
#endif

#if CFG_MK2_AC_SENSOR
	case MK2_ACT_GET_ASSISTANT_VALUES:
		vebusSystem.act.state = MK2_ACT_GET_ASSISTANT_VALUES;
		if (!vebusGetCurrentSensors(getAssistantValuesDone))
			getAssistantValuesDone();
		return veFalse;
#endif
	}

	/* per device actions */
	switch (isFlagSet(dev->act.actions, MK2_ACT_COUNT))
	{
#if CFG_MK2_ASSISTANT_VARS_COUNT
	case MK2_ACT_DEV_SET_ASSISTANT_VAR:
		/* Change a setting in the device, cb to get the new raw value */
		vebusSystem.act.varId = 0x80 + isFlagSet(dev->act.assistantWriteFlags, CFG_MK2_ASSISTANT_VARS_COUNT);
		vebusSystem.act.varValue = mk2ActSetVarCb(vebusSystem.act.dev, vebusSystem.act.varId);
		vebusSystem.act.state = MK2_ACT_DEV_SET_ASSISTANT_VAR;
		vebusSetVar(dev, vebusSystem.act.varId, setRamAssistantVarDone, vebusSystem.act.varValue);
		break;
#endif

	case MK2_ACT_DEV_SET_SETTING:
		/* Change a setting in the device, cb to get the new raw value */
		vebusSystem.act.varId = isFlagSet(dev->act.settingWriteFlags, VEBUS_SETTINGS_COUNT);
		vebusSystem.act.varValue = mk2ActSetRawSettingCb(vebusSystem.act.dev, vebusSystem.act.varId);
		vebusSystem.act.state = MK2_ACT_DEV_SET_SETTING;
		if (!vebusSetSetting(dev, vebusSystem.act.varId, setSettingDone, vebusSystem.act.varValue)) {
			/*
			 * Things are quite wrong when ended up here. It was attempted to
			 * write an invalid setting to the device. Ignore this settings..
			 */
			logE("mk2 action", "attempted to write wrong settings %d %d", (int) vebusSystem.act.varId, (int) vebusSystem.act.varValue);
			setSettingDone();
		}

		break;

	case MK2_ACT_DEV_GET_SETTING:
		vebusSystem.act.varId = isFlagSet(dev->act.settingReadFlags, VEBUS_SETTINGS_COUNT);
		veAssert(vebusSystem.act.varId != MK2_ACT_FLAG_INVALID);
		vebusSystem.act.state = MK2_ACT_DEV_GET_SETTING;
		vebusGetSetting(dev, vebusSystem.act.varId, getSettingDone, &vebusSystem.act.varValue);
		break;

	case MK2_ACT_DEV_GET_RAM_VAR:
		vebusSystem.act.varId = isFlagSet(dev->act.ramReadFlags, VEBUS_RAMVAR_COUNT);
		veAssert(vebusSystem.act.varId != MK2_ACT_FLAG_INVALID);
		vebusSystem.act.state = MK2_ACT_DEV_GET_RAM_VAR;
		vebusGetVar(dev, vebusSystem.act.varId, getVarDone, &vebusSystem.act.varValue);
		break;

	default:
		vebusSystem.act.dev = NULL;
		return veTrue;
	}
	return veFalse;
}

/**
 * Get the setting with the passed id from a device.
 *
 * Once the variable is obtained @ see mk2ActObtainedRawSettingCb will be
 * invoked with the value.
 */
void mk2ActGetRawSetting(struct VebusDeviceS* dev, un8 id)
{
	setFlag(dev->act.settingReadFlags, id);
	setFlag(dev->act.actions, MK2_ACT_DEV_GET_SETTING);
}

/**
 * Helper to get a flag by id. This will actually request the
 * setting containing the flag.
 */
void mk2ActGetRawFlag(struct VebusDeviceS* dev, un8 id)
{
	un8 sid = vebusGetSettingIdFromFlag(id);

	if (sid  == VEBUS_ID_INVALID)
		return;
	mk2ActGetRawSetting(dev, sid);
}

/**
 * Mark the setting id for writing. When this action is performed
 * @see mk2ActSetRawSettingCb will be invoked to get the value to be
 * written. mk2ActSetSettingDoneCb will be invoked after the setting
 * has been updated.
 */
void mk2ActSetRawSetting(struct VebusDeviceS* dev, un8 id)
{
	if (!vebusSupportedSetting(dev, id))
		return;
	setFlag(dev->act.settingWriteFlags, id);
	setFlag(dev->act.actions, MK2_ACT_DEV_SET_SETTING);
}

void mk2ActSetFlag(struct VebusDeviceS* dev, un8 id)
{
	un8 sid = vebusGetSettingIdFromFlag(id);

	if (sid == VEBUS_ID_INVALID)
		return;

	mk2ActSetRawSetting(dev, sid);
}

/**
 * Get the ram variable with the passed id.
 *
 * Once the variable is obtained @ see mk2ActObtainedRawVarCb will be
 * invoked with the value.
 */
void mk2ActGetVar(struct VebusDeviceS* dev, un8 id)
{
	setFlag(dev->act.ramReadFlags, id);
	setFlag(dev->act.actions, MK2_ACT_DEV_GET_RAM_VAR);
}

#if CFG_MK2_ASSISTANT_VARS_COUNT
/**
 * Mark the assistant ramvar id for writing. When this action is
 * performed @see mk2ActSetVarCb will be invoked to get the value to be
 * written. mk2ActSetVarDoneCb will be invoked after the value
 * has been updated.
 */
void mk2ActSetAssistantVar(struct VebusDeviceS* dev, un8 id)
{
	veAssert(id >= 0x80 && id < 0x80 + CFG_MK2_ASSISTANT_VARS_COUNT);
	setFlag(dev->act.assistantWriteFlags, id - 0x80);
	setFlag(dev->act.actions, MK2_ACT_DEV_SET_ASSISTANT_VAR);
}
#endif

/** helper which returns if there is something left todo for this device */
veBool mk2ActAreActionsQueued(struct VebusDeviceS* dev)
{
	logI("mk2", "queued %X %X", dev->act.actions[0], dev->act.actions[1]);
	return isFlagSet(dev->act.actions, MK2_ACT_COUNT) != MK2_ACT_FLAG_INVALID;
}

/**
 * Queue the request of a vebus system message.
 *
 * @note: only for system actions!
 */
void mk2ActMark(Mk2Actions action)
{
	setFlag(vebusSystem.act.actions, action);
}

veBool mk2IsActionsQueued(struct VebusDeviceS* dev, Mk2Actions action)
{
	return dev->act.actions[action / 8] & (1 << (action & 0x7));
}

