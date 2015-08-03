#ifndef _VELIB_TYPES_VE_BIT_PAIR_H
#define _VELIB_TYPES_VE_BIT_PAIR_H

#include <velib/velib_config.h>
#include <velib/base/base.h>
#include <velib/types/types.h>

 /**
  * @ingroup VELIB_TYPES
  * @defgroup VELIB_TYPES_BIT_PAIR Bit Pair Operations
  */
/// @{

#define VE_BIT_PAIR_MASK			(3)

/* Binary Switch Bank Status */
typedef enum
{
	VE_BIN_SW_BANK_STAT_NO			= 0,
	VE_BIN_SW_BANK_STAT_OFF			= VE_BIN_SW_BANK_STAT_NO,
	VE_BIN_SW_BANK_STAT_DISABLED	= VE_BIN_SW_BANK_STAT_NO,
	VE_BIN_SW_BANK_STAT_RESET		= VE_BIN_SW_BANK_STAT_NO,
	VE_BIN_SW_BANK_STAT_ZERO		= VE_BIN_SW_BANK_STAT_NO,
	
	VE_BIN_SW_BANK_STAT_YES			= 1,
	VE_BIN_SW_BANK_STAT_ON			= VE_BIN_SW_BANK_STAT_YES,
	VE_BIN_SW_BANK_STAT_ENABLED		= VE_BIN_SW_BANK_STAT_YES,
	VE_BIN_SW_BANK_STAT_SET			= VE_BIN_SW_BANK_STAT_YES,
	VE_BIN_SW_BANK_STAT_ONE			= VE_BIN_SW_BANK_STAT_YES,
	
	VE_BIN_SW_BANK_STAT_ERROR		= 2,
	
	VE_BIN_SW_BANK_STAT_UNAVAILABLE	= 3,
	VE_BIN_SW_BANK_STAT_UNKNOWN		= VE_BIN_SW_BANK_STAT_UNAVAILABLE,
} VeBinarySwitchBankStatusBit;

/* Switch Bank Control */
typedef enum
{	
	VE_SW_BANK_CTRL_TURN_OFF		= 0,
	VE_SW_BANK_CTRL_DISABLE			= VE_SW_BANK_CTRL_TURN_OFF,
	VE_SW_BANK_CTRL_RESET			= VE_SW_BANK_CTRL_TURN_OFF,
	VE_SW_BANK_CTRL_ZERO			= VE_SW_BANK_CTRL_TURN_OFF,
	
	VE_SW_BANK_CTRL_TURN_ON			= 1,
	VE_SW_BANK_CTRL_ENABLE			= VE_SW_BANK_CTRL_TURN_ON,
	VE_SW_BANK_CTRL_MAKE_ONE		= VE_SW_BANK_CTRL_TURN_ON,
	
	VE_SW_BANK_CTRL_RESERVED		= 2,
	
	VE_SW_BANK_CTRL_NO_ACTION		= 3
} VeSwitchBankControlBit;

typedef un8 VeBitPair;

VE_DCL	void		veBitPairSet(un8 *container, un8 bitNr, VeBitPair value);
VE_DCL	void		veBitPairSetRange(un8 *container, un8 bitNr, un8 count, VeBitPair value);
VE_DCL	VeBitPair	veBitPairGet(un8 *container, un8 bitNr);

/// @}

#endif
