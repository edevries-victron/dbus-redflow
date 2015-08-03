#ifndef _VELIB_VECAN_PROD_TYPES_H_
#define _VELIB_VECAN_PROD_TYPES_H_

#include <velib/base/base.h>
#include <velib/J1939/J1939_types.h>
#include <velib/types/types.h>
#include <velib/vecan/reg_prod_sf.h>

/// @addtogroup VELIB_VECAN_PROD_DATA
/// @{

/// The signal can be changed from the bus.
#define S_WRITE		0x01
/// Flags signals with act as instances, when this field differs the message
/// report the value for a different object.
#define S_INSTANCE	0x02
/// shorter name
#define S_INST		S_INSTANCE

/// Typedef for the message id. (must be signed!)
typedef sn16		VeMsgNr;
/// Reserved value to indicate no message.
#define VE_MSG_NR_NONE	-1

/// Index in the array of used scales.
typedef	un8  VeScaleId;

/// Constant part of a message definition.
typedef struct
{
	un8				type;		///< SF / FP.
	un8				dp;			///< Datapage.
	un16			pgn;		///< The pgn of this message.
	un16			signal;		///< The index of the first signal.
	un8				number;		///< Number of signals in this message.
} VeProdMsgDefConst;

/// Volatile part of a message definition.
typedef struct
{
	un8		state;			///< The state of this message.
	un8		timer;			///< Timer for periodic transmission.
} VeProdMsgDefVolatile;

/// The (normally) constant part of a signal definition, but could
/// be altered and stored.
typedef struct
{
	un8		interval;		///< Interval to send the message.
	un8		priority;		///< Priority of the message.
} VeProdMsgDefInvolatile;

/// Definition of a signal (constant)
typedef struct 
{
	VeDataBasicType	type;		///< Bus format.
	VeScaleId		trans;		///< Index in scale array for optional unit conversion.
	un8				flags;		///< Indicates if writable / instance etc.
	void*			value;		///< Pointer to actual value.
	un8				size;		///< Length of the actual value in bytes.
} VeProdSgnDef;

typedef struct
{
	sn32			min;		///< Minimum allowed value to set.
	sn32			max;		///< Maximum allowed value to set.
//	un32			step;		///< Single step increment (not used at the moment).
} VeProdSgnSpanDef;

/// Several Device Funtions can use the same code to send messages.
/// Updates and request need to be called in their own context. This defines
/// the context for the producer per device function.
typedef struct
{
	VeMsgNr			msgFirst;	///< First message belonging to the Device Function
	VeMsgNr			msgLast;	///< Last message belonging to the Device Function
} VeProdDevDef;

/// @}

#endif
