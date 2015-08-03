#ifndef _VELIB_VECAN_REG_RX_H_
#define _VELIB_VECAN_REG_RX_H_

#include <velib/vecan/reg.h>
#include <velib/utils/ve_events.h>

/**
 * @ingroup VELIB_VECAN
 * @defgroup VELIB_VECAN_REG_RX Handle incoming VRegs
 */
/// @{

/** 
 * Lookup table, which must be defined by the application. 
 * Table must end with hndl set to NULL.
 */
extern VeEventHndlr const veRegRxHndlrDef[];

void veRegRxInit(void);
void veRegRxData(void);
void veRegRxUpdate(void);

/// The register being received, ACKed or requested.
extern VeRegId			veRegRxId;

/// Contains the ACK code when ev equals VE_REG_ACK.
extern VeRegAckCode		veRegAckCode;

/// Contains the request mask when ev equals VE_EV_VREG_RX_REQ.
extern VeRegId 			veRegReqMask;

/// Flag which must be sets by the function handling the register.
/// If the flag was not set a error will be returned to the sender.
extern veBool			veRegRxHandled;

/// @}

#endif
