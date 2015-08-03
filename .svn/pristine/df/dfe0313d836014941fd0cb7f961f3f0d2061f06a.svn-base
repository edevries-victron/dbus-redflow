#ifndef _VELIB_VECAN_PROD_DATA_CM_H_
#define _VELIB_VECAN_PROD_DATA_CM_H_

#include <velib/base/base.h>
#include <velib/nmea2k/n2k_complex.h>
#include <velib/vecan/prod_types.h>
#include <velib/types/variant.h>

/**
 * @ingroup VELIB_VECAN_PROD_DATA
 * @defgroup VELIB_VECAN_PROD_DATA_CM Complex Group Support
 */
/// @{

veBool			veProdDataMsg(void);
void			veProdDataCmSet(VeProdSgnDef const* signal, VeVariant* variant);

// Callbacks

/**
 * @brief
 *	Callback to indicate that a request to update signal to the value in variant
 *	has arrived. 
 * @details
 *	The values is aleady checked for sanity and min / max. The 
 *	application must perform the actual update or return an error code. The error
 *	code will be send back to the source of the command.
  */
N2kCmParamCode	veProdDataChangeEvent(VeProdSgnDef const* signal, VeVariant* variant);

/**
 * @brief
 *	Callback to indicate that the priority should change.
 */
N2kCmPriCode	veProdDataPriorityChangeEvent(VeMsgNr msgNr, N2kCmPriCode priority);

/**
 * @brief
 *	Callback to indicate the start of the 
 */
void			veProdDataChangeBeginEvent(VeMsgNr msgNr);

/**
 * @brief
 *	Callback to indicate that the interval should change.
 */
void			veProdDataIntervalChangeEvent(VeMsgNr msgNr, un32 interval);


/**
 * Callback to indicate that at least one signal is changed and the processing of
 * the command is done.
 */
void			veProdDataStoreEvent(void);


/// @}

#endif
