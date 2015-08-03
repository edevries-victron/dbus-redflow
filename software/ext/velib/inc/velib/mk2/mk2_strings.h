#ifndef _VELIB_MK2_STRINGS_H_
#define _VELIB_MK2_STRINGS_H_

#include <velib/mk2/vebus_values.h>

char const* vebusGetName(VebusValue nr);
char const* vebusGetSettingName(un8 id);
char const* vebusGetFlagName(un8 nr);
char const* vebusGetRamVarName(un8 id);

#endif
