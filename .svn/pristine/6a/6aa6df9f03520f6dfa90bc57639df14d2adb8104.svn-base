#ifndef _VELIB_NMEA2K_N2K_FP_H_
#define _VELIB_NMEA2K_N2K_FP_H_

/**
 * @ingroup VELIB_NMEA2K
 * @defgroup VELIB_NMEA2K_FP Fast Packet
 */
 /// @{

#include <velib/velib_config.h>
#include <velib/base/base.h>
#include <velib/J1939/J1939_types.h>
#include <velib/J1939/J1939_sf.h>

/** 
 * A fast packet message may be considered incomplete if a frame is not received
 * within 750 milliseconds 
 */
#ifndef CFG_N2K_FP_RX_FRAME_TIMEOUT
#define CFG_N2K_FP_RX_FRAME_TIMEOUT		(15+1)	// * default in steps of 50 ms
#endif

/// @note this should be in agreement with the J1939 message types, see j1939_types.h
#define N2K_FP	0x03

#define N2K_FP_TX	(N2K_FP | J1939_TX)
#define N2K_FP_RX	(N2K_FP | J1939_RX)

/// Mask for the counter in the first byte of a Fast Packet data frame.
#define N2K_FP_PDU_COUNTER		0x1F
/// Checks if this Fast Packet data frame is the first of a new FP transmission.
#define N2K_FP_PDU_BEGIN(msg)	(((msg).mdata[0] & 0x1F) == 0)
/// Grabs the manufacturer field from the first data frame to allow early dropping.
#define N2K_FP_PDU_MAN(msg)		(((msg).mdata[3] << 8) | (msg).mdata[2])

/**
 * A NMEA 2000 Fast Packet message.
 * 
 * @extends J1939Msg
 */
typedef struct
{
	J1939Id			msgId; 						///< Can identifier.
	VeCanLength		length;						///< Length of the data.
	VeCanFlags		flags;						///< flags for special purposes.
	J1939MsgType	type;						///< must be N2K_FP
	J1939Msg*		next;						///< @protected next message in list
#if CFG_CAN_MSG_EXTRA
	VeCanMsgExtra extra;			///< additional fields which can be added to a msg
#endif
	un8				mdata[CFG_N2K_FP_MSG_SIZE];	///< actual data
	un8				sequence;					///< @protected counter for transport
	/// @protected current index for bytes being send / received
	un8				bytes;
	/// @protected 750 millisecond timeout counter
	un16			timeout;
} N2kFp;

/// Per stack variables needed for fp support
typedef struct
{
	N2kFp*		txFree;			///< top of free FP msg queue
	N2kFp*		rxFree;			///< top of free FP msg queue
	N2kFp*		rxInProgress;	///< fp messages being partly received	
} N2kFpVars;

#ifdef CFG_N2K_FP_REPLY_USES_RX
// Fast packet replies must be queued and cannot be postponed. For that reason
// a define allows using the rx messages for that purpose, since the rx queue
// needs to be of a reasonable size anyway. The tx memory can then be limited
// (even to a single message). The code freeing the tx message must however take
// into account that rx message must be handled as well!.
#define n2kFpReplyAlloc()		n2kFpRxAlloc()
#define n2kFpReplyFree(msg)		n2kFpRxFree(msg)

#else

// Replies use the tx messages. Note that if many frames are queued for output,
// incoming commands cannot be properly dealt with.
#define n2kFpReplyAlloc()		n2kFpTxAlloc()
#define n2kFpReplyFree(msg)		n2kFpTxFree(msg)

#endif

VE_DCL	void 	n2kFpInit(void);
VE_DCL	void 	n2kFpQueueRxInit(void);
VE_DCL	void 	n2kFpQueueTxInit(void);
VE_DCL	N2kFp* 	n2kFpRxAlloc(void);
VE_DCL	N2kFp* 	n2kFpTxAlloc(void);
VE_DCL	void 	n2kFpRxFree(N2kFp *msg);
VE_DCL	void 	n2kFpTxFree(N2kFp *msg);
VE_DCL	void 	n2kFpSequence(N2kFp *msg, un8 *sequence);
VE_API	veBool 	n2kFpSend(N2kFp *fullMsg);
VE_API	N2kFp* 	n2kFpData(J1939Sf *frame);
VE_DCL	void 	n2kFpTick(void);

// Helper to get to now if a pgn is defined as a FastPacket
VE_DCL	veBool	n2kIsFp(J1939Id const* id);

/// @}

#endif
