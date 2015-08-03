#include <velib/velib_inc_mk2.h>
#include <velib/mk2/values_float.h>
#include <velib/utils/ve_logger.h>

/**
 * Since the common code attempt to prevent to hardcode where data is
 * stored and in which format, some callbacks are needed when the common
 * code needs to use a value. This provides some defaults for the
 * values_float version. Typically this file is project specific.
 * (and since that is the case, also the debug stubs are placed here..)
 */

#if !CFG_MULTIPLE_VEBUS_SYSTEMS
struct VebusSystemS vebusSystem;
struct Mk2S mk2;
#else

VebusSystem*		aVebusSystem;
Mk2*				aMk2;

void vebusSystemSetActive(VebusSystem* system)
{
	aVebusSystem = system;
	aMk2 = &system->mk2Interface;
}

#endif

void mk2FhRawFrameReceivedEvent(Mk2MsgRx* mk2msg)
{
	mk2Dump("<", mk2msg);
}

void mk2FhFrameSendEvent(Mk2MsgTx* mk2msg)
{
	VE_UNUSED(mk2msg);
}

void mk2FhFrameReceivedEvent(Mk2MsgRx* mk2msg)
{
	VE_UNUSED(mk2msg);
}

/* Number of cells */
un8 mk2GetNoc(struct VebusDeviceS* dev)
{
	float val = mk2UnpackSettingFloat(dev->settingInfo[VE_U_BAT_ABSORPTION].defaultValue,
														dev, VE_U_BAT_ABSORPTION, vebusSystem.units);
	if (val > 40)
		return 24;

	if (val > 20)
		return 12;

	return 6;
}

/* Todo */
veBool mk2IsUS(struct VebusDeviceS* dev)
{
	VE_UNUSED(dev);

	return veFalse;
}

veBool mk2HasAcIn(VebusDevice* dev, un8 n)
{
	VE_UNUSED(dev);

	switch (n)
	{
	case 0:
	case 1:
		return veTrue;
	case 2:
		if (!vebusSystem.devices || !vebusSystem.devices[0].validScales)
			return veFalse;
		if (vebusSystem.devices->settingInfo[VE_I_MAINS_LIMIT_AC2].scale != 0)
			return veTrue;
		return veFalse;
	default:
		return veFalse;
	}
}

#if CFG_WITH_CONTROL

un16 veGetCurrentLimitCb(void)
{
	un8 input = vebusActiveIn(&vebusSystem.acInConfig);
	return (un16) (vebusSystem.acInConfigPerAcIn[input].limit);
}

VebusSwitchPos veGetIntendedState(void)
{
	return SP_SWITCH_ON;
}

#endif

VebusSettingInfo* vebusGetSettingInfoOfDevice(struct VebusDeviceS* dev)
{
	return dev->settingInfo;
}

VebusRamInfo* vebusGetRamInfoOfDevice(struct VebusDeviceS* dev)
{
	return dev->ramInfo;
}
