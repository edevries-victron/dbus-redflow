#ifndef _VELIB_VECAN_TESTER_H_
#define _VELIB_VECAN_TESTER_H_

#include <velib/vecan/reg.h>
#include <velib/vecan/prod_types.h>
#include <velib/types/types.h>
#include <velib/utils/ve_events.h>
#include <velib/types/variant.h>

/**
 * @ingroup VELIB_VECAN
 * @defgroup VELIB_VECAN_TESTER Test Mode Handler
 */
/// @{

#define VE_TESTMODE_PASSWORD			"VTST"
#define VE_TESTMODE_PASSWORD_SIZE		(sizeof(VE_TESTMODE_PASSWORD)-1)

VE_DCL void 	veTestModeHandler(VeEvent ev);
VE_DCL veBool 	veTestModeIsEnabled(void);
VE_EXT void 	veTestModeCallback(void);

/// @}

#endif
