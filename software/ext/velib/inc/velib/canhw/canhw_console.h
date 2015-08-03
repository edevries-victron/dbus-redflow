#ifndef _VELIB_CANHW_CANHW_CONSOLE_H_
#define _VELIB_CANHW_CANHW_CONSOLE_H_

#include <velib/base/base.h>
#include <velib/canhw/canhw.h>

/**
 * @ingroup VELIB_CANHW
 * @defgroup VELIB_CANHW_CONSOLE Console
 * @brief 
 *	Stub to (optional) print send and received messages.
 * @{
*/

VE_DCL	veBool	veCanShowTrace(veBool enableTrace);
VE_DCL	void	veCanShow(VeRawCanMsg const *canMsg, char *module, char *str);
VE_DCL	veBool	veCanShowRead(VeRawCanMsg *rawCanMsg);
VE_DCL	veBool	veCanShowSend(VeRawCanMsg *rawCanMsg);

/// @}

#endif
