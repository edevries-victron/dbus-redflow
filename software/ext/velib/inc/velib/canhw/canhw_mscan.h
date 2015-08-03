#ifndef _VELIB_CANHW_MSCAN_H_
#define _VELIB_CANHW_MSCAN_H_

/**
 * @ingroup VELIB_CANHW
 * @defgroup VELIB_CANHW_MSCAN	FreeScale MSCAN
 * @brief 
 *  Standardized calls MSCAN, FreeScale CAN support for (part of) the
 *  HC08 / HC12 / S12X / MPC5200 families. 
 * @{
 */

#include <velib/canhw/canhw_types.h>

#define veMsCanTxIsEmpty() (CANTFLG==0x07)

veBool		veMsCanRead(VeRawCanMsg * const rawCanMsg);
veBool 		veMsCanReadTx(VeRawCanMsg * const frame);
veBool		veMsCanSend(VeRawCanMsg const * const frame);
void		veMsCanRxInterrupt(void);
void		veMsCanTxInterrupt(void);

/// @}

#endif
