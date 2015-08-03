#ifndef _CANHW_CANHW_GENERAL_H_
#define _CANHW_CANHW_GENERAL_H_

/**
* @ingroup VELIB_CANHW
* @defgroup VELIB_CANHW_GENERAL User selectable
* @{
* @brief 
*	General CAN-interface.
* @details
*	Selecting a active CANbus interface / port is handled from a configuration 
*	string. Interfaces can be assigned to numbers determined by the application.
*	The busId must be less then @ref CFG_CANHW_MAX_BUSSES.
*/

#include <velib/velib_config.h>
#include <velib/base/base.h>
#include <velib/canhw/canhw_types.h>
#include <velib/canhw/canhw_stats.h>

extern VeBusStats veBusStats[CFG_CANHW_MAX_BUSSES];

typedef un8 VeBusId; // bus number assigned to a CAN-bus connection

///@name Common
///@{
// standard calls
VE_DCL	veBool	veGeCanSend(VeRawCanMsg *rawCanMsg);
VE_DCL	veBool	veGeCanRead(VeRawCanMsg *rawCanMsg);

// Apply bus settings
VE_DCL	void 	veGeCanInit(void);
VE_DCL	veBool	veGeCanSetBitRate(un16 kbit);
VE_DCL	veBool	veGeCanBusOn(void);
///@}

///@name Specific
///@{
VE_DCL	veBool	veGeCanOpen(VeBusId busId, char const* driver);
VE_DCL	veBool	veGeCanBusClose(void);

// Set the active bus
VE_DCL	VeBusId	veGeCanGetActiveBus(void);
VE_DCL	void	veGeCanSetActiveBus(VeBusId busId);

///@}

///@}

#endif
