#ifndef _VELIB_CANHW_CANHW_TYPES_H_
#define _VELIB_CANHW_CANHW_TYPES_H_

/** 
 * @addtogroup VELIB_CANHW
 * @brief 
 *	Definition of a CAN-bus frame.
 * @details
 *	Common calls to the CANHW only works if there is a single message format. 
 *	This message format is defined in canhw_types.h.
 * @{
 */

#include <velib/velib_config.h>
#include <velib/base/types.h>

#if CFG_CANHW_STAMP
#include <velib/types/ve_stamp.h>
#endif

/// Message is a remote request
#define 	VE_CAN_RTR 				0x0001U
/// Message has a standard ID (11 bit)
#define		VE_CAN_STD				0x0002U
/// Message has an extended ID (29 bit)
#define		VE_CAN_EXT				0x0004U
/// Message looped back (error of tx ack). When the @ref VE_CAN_BOUNCE flag is set
/// the message will be returned after it has been send by @ref veCanRead with this
/// flag set. An error might be returned only if @ref VE_CAN_SHOOT is set (normally
/// the message will be retransmitted till success).
#define 	VE_CAN_IS_LOOP			0x0008U

/// Request for loopback after sending.
#define 	VE_CAN_DO_LOOP			0x0010U
/// Single shot mode.
#define		VE_CAN_SHOOT			0x0020U
/// Error notification. Only applicable in single shot + loop.
#define		VE_CAN_TX_ERR			0x0040U
/// Some messages where lost during reception in hardware.
#define		VE_CAN_RX_OVERRUN		0x0080U
/// 
#define		VE_CAN_ERROR_PASSIVE	0x0100U
///
#define		VE_CAN_TX_BUS_OFF		0x0200U
///
#define		VE_CAN_RX_BUS_OFF		0x0400U


/// Flags assigned to a CANbus messages.
typedef un16 VeCanFlags;
/// The CAN identifier (either 11 or 29 bits)
typedef un32 VeCanId;	
/// The length of the data in the message (dlc).
typedef un16 VeCanLength;

#if CFG_CANHW_STAMP

typedef struct {
	VeStamp time;
} VeCanMsgExtra;

#endif


#if CFG_CANHW_J1939_COMPATIBLE

/** 
 * Message definition compatible with the J1939 stack, allowing message 
 * to be casted instead of having to copy them, reducing code, mem and cpu.
 */
typedef struct
{
	VeCanId		canId;				///< the 11 or 29 bits CAN-identifier of the message
	VeCanLength length;				///< DLC
	VeCanFlags	flags;				///< control and status flags
	un8			notused1;			///< (type)
	void*		notused2;			///< (next)
#if CFG_CAN_MSG_EXTRA
	VeCanMsgExtra extra;			///< additional fields which can be added to a msg
#endif
	un8			mdata[8];			///< data
} VeRawCanMsg;

#else

/** 
 * @brief
 *	Definition of a raw CANbus message.
 * @details
 *	The VeRawCanMsg is defined to allow running different protocols on the 
 *	CANbus. E.g. the EMM uses the same interface but uses several different 
 *	kinds of higher level protocols. Direct support for J1939 way of 
 *	interpetation is added though, since it will be the majority of cases. 
 *	Simply casting from on to the order prevents having unnecessary copies.
 * @note 
 *	Only when CFG_CANHW_J1939_COMPATIBLE is defined, space is reserved to 
 *	cast it to a J1939Sf. 
 */
typedef struct
{
	/// The 11 or 29 bits CAN-identifier of the message
	VeCanId		canId;
	VeCanLength length;
	VeCanFlags	flags;
	un8			mdata[8];	/**< The data itself, data is reserved by KEIL */
} VeRawCanMsg;

#endif

/// @}

#endif
