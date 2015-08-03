#ifndef _VELIB_MK2_MK2_INFO_H_

#include <velib/mk2/vebus_device.h>

void mk2InfoRamStore(un32 version, VebusRamInfo* info, un16 numVars);
veBool mk2InfoRamLoad(un32 version,  VebusRamInfo* info, un16 numVars);
void mk2InfoSettingsStore(un32 version, VebusSettingInfo* info, un16 numSettings);
veBool mk2InfoSettingsLoad(un32 version, VebusSettingInfo* info, un16 numSettings);

#endif
