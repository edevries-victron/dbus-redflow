#ifndef _VELIB_CANHW_CANHW_H_
#define _VELIB_CANHW_CANHW_H_

/**
 * @ingroup VELIB
 * @defgroup VELIB_CANHW CANHW
 * @brief
 *	Common prototypes to interface the CAN-bus.
 *
 * @details 
 *	The minimum prototypes for interfacing a CAN-bus are defined here.
 *	Standardizing these calls, allows all code to simply send CAN messages, 
 *	without having to bother about the actual device / platform, whether there 
 *	are multiple CAN-interfaces etc. The code always interact with the active 
 *	bus. It is not defined here how to switch between busses, since most devices
 *	will only have one bus in the first place.
 *
 *	Some of the commonly used CAN-bus driver are included in velib. If they 
 *	are used macro can be created to directly call them by setting 
 *  CFG_CANHW_FUNCTIONS_ARE_MACROS and adding the macro's to vecan_inc_canhw.h.
 *  PC tools can use the canhw_driver.c which support multiple CAN drivers.
 *  
 * @{
 */

#include <velib/velib_config.h>
#include <velib/base/base.h>
#include <velib/types/types.h>
#include <velib/canhw/canhw_types.h>

#ifndef BUS_STAT_COUNT_USE_UN32
#define BUS_STAT_COUNT_USE_UN32				1
#endif

/*
 * The main reason for the counters is to see if errors occur at all.
 * If a 16 bit might be atomic, faster it can be used as well.
 */
#if BUS_STAT_COUNT_USE_UN32
#define BUS_STAT_COUNT_TYPE VE_UN32
typedef un32 veCanCount;
#elif BUS_STAT_COUNT_USE_UN16
#define BUS_STAT_COUNT_TYPE VE_UN16
typedef un16 veCanCount;
#else
#error unknown BUS_STAT_COUNT_TYPE
#endif

/*
 * The canbus statistics of most interest are the errors, not how many
 * messages are received / transmissed, nor a detailed reason of the
 * cause since not all CAN controllers.
 */
typedef struct
{
	/*
	 * Number of messages overwritten in the CAN controller itself.
	 * In this case the mcu should just read more ofter.
	 */
	veCanCount hwRxOverflow;
	/* 
	 * Number of times the controller gets into an error passive state.
	 * For the general vreg there is no distinction if the REC of TEC
	 * caused the error passive state since not all controllers might
	 * no the exact cause. 
	 */
	veCanCount errorPassive;
	/*
	 * Number of times the driver was forced off the bus.
	 */
	veCanCount errorBusOff;
	/*
	 * Software overruns. This counter should be updated in the application.
	 */
	veCanCount sfRxOverflow;
} VeCanBusStats;

extern VeCanBusStats veCanBusStats;

#if !CFG_CANHW_FUNCTIONS_ARE_MACROS

/// Initialises the CANbus driver
VE_DCL void veCanInit(void);

/// De initialises the CANbus driver
VE_DCL void veCanDeInit(void);

/**
 * Sends a rawCanMsg to the bus. 
 *
 * This call must be implemented or forwarded to a already
 * implemented support.
 *
 * @param	rawCanMsg the message to be send
 * @return	whether the message was accepted.
*/
VE_DCL veBool veCanSend(VeRawCanMsg *rawCanMsg);

/** 
 * Retrieves an incoming message from the CAN-bus.
 * Although this function is not strictly necessary, since
 * the message-router is responsible for the incoming message, it is
 * recommend. This allows easy exchange of code between different 
 * projects.
 * * @param	rawCanMsg the message to tbe read.
 * @return	whether a new message is returned
 */
VE_DCL veBool veCanRead(VeRawCanMsg *rawCanMsg);

/// Enables the CAN bus
VE_DCL veBool veCanBusOn(void);

/// Not all bitrates are per definition supported!
VE_DCL veBool veCanSetBitRate(un16 kbit);

#else
/**
 * Note: This file is expected to be part of the application. It allows to
 * #define veCanRead / veCanWrite to existing functions, to call it directly.
 * (The implementations have different names to allow several in tools etc).
 */
#include <velib/velib_inc_canhw_app.h>
#endif

/* @retval If the message is read 
 * Some hardware would need to clear interrupt flags or will continuesly interrupt otherwise.
 */
VE_DCL veBool veCanRxEvent(void);

/** Should return if there is space left in the CANhw layer for sending */
VE_DCL veBool veCanSendPossible(void);

VE_DCL void veCanTxEvent(void);

VE_DCL void veCanErrorEvent(VeCanFlags flags);

#if CFG_CAN_LOCK
#include <velib/platform/plt.h>
# define veCanLockClaim()		do {pltInterruptSuspend();} while (0)
# define veCanLockRelease()		do {pltInterruptResume();} while (0)
#else
# define veCanLockClaim()
# define veCanLockRelease()
#endif

/// @}

#endif
