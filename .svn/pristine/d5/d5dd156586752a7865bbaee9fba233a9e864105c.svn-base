#ifndef _CANHW_KVASER_H_
#define _CANHW_KVASER_H_

#include <velib/base/compiler.h>
#include "canhw_types.h"

/**
 * @ingroup VELIB_CANHW
 * @defgroup VELIB_CANHW_KVASER Kvaser CAN-lib
 * @brief 
 *	Standardized calls for Kvaser API (see http://www.kvaser.com/).
 * @{
 */

/// Cheating a bit; this is actually the Kvaser handle but typedefed here
/// again to stop depending on canlib.h in public headers.
typedef sn32		VeKvCanHndl;

///@name Common
///@{
VE_DCL	void		veKvCanInit(void);
VE_DCL	veBool		veKvCanSend(VeRawCanMsg *rawCanMsg);
VE_DCL	veBool		veKvCanRead(VeRawCanMsg *rawCanMsg); 

// additional functions set bus parameters
VE_DCL	veBool		veKvCanBusOn(void);
VE_DCL	veBool		veKvCanSetBitRate(un16 kbit);
///@}

///@name Specific
///@{
VE_DCL	VeKvCanHndl	veKvCanOpen(int channel);

// functions to select the active bus
VE_DCL	void 		veKvCanSetHandle(VeKvCanHndl handle);
VE_DCL	VeKvCanHndl	veKvCanGetHandle(void);

///@}

/// @}

#endif
