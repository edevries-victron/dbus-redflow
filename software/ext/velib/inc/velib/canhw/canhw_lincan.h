#ifndef _VELIB_CANHW_LINCAN_H_
#define _VELIB_CANHW_LINCAN_H_

/** 
* @ingroup VELIB_CANHW
* @defgroup VELIB_CANHW_LINCAN LinCAN
* @{
* @brief 
*	Standardized calls for LinCan (see http://freshmeat.net/projects/lincan/).
*/

#include <velib/base/compiler.h>
#include <velib/canhw/canhw_types.h>

void    veLcInit(void);
veBool  veLcOpen(char const * const can_dev_name);
veBool  veLcBusOn(void);
veBool  veLcBusOff(void);
void    veLcDeInit(void);

veBool	veLcSetBusParams(
                sn8 freq,
                un8 tseg1,
                un8 tseg2,
                un8 sjw,
                un8 noSamp,
                un8 syncmode
            );

veBool  veLcCanRequest(VeCanId canId);
veBool  veLcCanSend(VeRawCanMsg* rawCanMsg);
veBool  veLcCanRead(VeRawCanMsg* rawCanMsg);
void    veLcSetHandle(int newfd);
int     veLcGetHandle (void);

/// @}
#endif
