#ifndef _VELIB_MK2_VALUES_FLOAT_H_
#define _VELIB_MK2_VALUES_FLOAT_H_

#include <velib/velib_config.h>
#include <velib/base/types.h>
#include <velib/mk2/vebus_device.h>
#include <velib/mk2/vebus_system.h>
#include <velib/types/variant.h>

// System values for an AC phase
typedef struct
{
	float V;
	float I;
	float period;
	float frequency;
} Mk2AcInPhaseF;

// System values for an AC phase
typedef struct
{
	float V;
	float I;
	float period;
	float frequency;
} Mk2AcOutPhaseF;

typedef struct
{
	float V;
	float I;
	float period;
	float frequency;
} Mk2DcF;

typedef struct
{
	float min;
	float max;
	float limit;
	float setLimit;
} VebusAcInConfigF;


veBool vebusLocalSetFlag(struct VebusDeviceS* dev, un8 id, veBool value);
veBool vebusLocalSetSetting(struct VebusDeviceS * dev, un8 id, float value);

size_t vebusFormatValue(VebusUnit unit, VeVariant *value, char* buf, size_t len);
void vebusFormatRawSettingValue(struct VebusDeviceS* dev, un8 id, un16 value, char* buf, size_t len);
void vebusFormatRawVariableValue(struct VebusDeviceS* dev, un8 id, un16 value, char* buf, size_t len);
void vebusGetSettingValue(struct VebusDeviceS* dev, un8 id, VeVariant *var, VeUnitSet units);
void vebusGetRamValue(struct VebusDeviceS* dev, un8 id, VeVariant *var, VeUnitSet units);
void vebusGetSystemValue(struct VebusDeviceS* dev, un8 id, VeVariant *var, VeUnitSet units);
void vebusGetValue(struct VebusDeviceS* dev, VebusValue nr, VeVariant* value, VeUnitSet units);
void vebusGetFlag(struct VebusDeviceS* dev, VeVariant *var, un8 id);
float vebusGetSettingMaximum(struct VebusDeviceS* dev, un8 id, VeUnitSet units);
float vebusGetSettingMinimum(struct VebusDeviceS* dev, un8 id, VeUnitSet units);

void mk2UnpackAcInConfig(VebusAcInConfig const* raw, VebusAcInConfigF* out);
void mk2UnpackAcPhase(struct VebusDeviceS* dev, Mk2RawAcPhase const* raw,
					  Mk2AcInPhaseF* acIn, Mk2AcOutPhaseF* acOut);
void mk2UnpackDc(struct VebusDeviceS* dev, Mk2RawDc const* raw, Mk2DcF* out);
float mk2UnpackRamFloat(sn32 raw, struct VebusDeviceS* dev, un8 id);
float mk2UnpackSettingFloat(sn32 raw, struct VebusDeviceS* dev, un8 id, VeUnitSet units);
un16 mk2PackSettingFloat(float value, struct VebusDeviceS* dev, un8 id, VeUnitSet units);

float vebusSettingScale(struct VebusDeviceS* dev, un8 id, VeUnitSet units);
sn16 vebusSettingOffset(struct VebusDeviceS* dev, un8 id, VeUnitSet units);
size_t vebusStateText(int state, char* buf, size_t len);
size_t vebusNmea2000StateText(int state, char* buf, size_t len);

#endif
