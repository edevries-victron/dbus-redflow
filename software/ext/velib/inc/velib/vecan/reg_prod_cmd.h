#ifndef _VELIB_VECAN_VE_REG_PROD_CMD_H_
#define _VELIB_VECAN_VE_REG_PROD_CMD_H_

#include <velib/vecan/reg.h>
#include <velib/vecan/prod_types.h>
#include <velib/types/types.h>
#include <velib/utils/ve_events.h>
#include <velib/types/variant.h>

/**
 * @ingroup VELIB_VECAN
 * @defgroup VELIB_VECAN_REG_PROD_CMD VReg Command Handler (SF)
 */
/// @{

/// Span definitions of VRegs, to be defined by the application
VE_EXT	VeProdSgnSpanDef const * veRegProdCmdGetSpan(VeRegNr regNr);

VE_DCL void veRegProdCmdHandler(VeEvent ev);
VE_DCL VeRegAckCode veRegProdCmdCallback(void *data, VeVariant *variant);

/// @}

#endif
