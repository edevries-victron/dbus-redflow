#ifndef _VELIB_J1939_J1939_TP_H_
#define _VELIB_J1939_J1939_TP_H_

/**
 * @ingroup VELIB_J1939
 * @defgroup VELIB_J1939_TRANSPORT Transport
 * @brief 
 *	Support for the J1939 CTS/RTS and BAM transport protocol.
 * @{
 */

#include <velib/velib_config.h>
#include <velib/J1939/J1939_types.h>
#include <velib/J1939/J1939_acl.h>

/// 50ms minimum (max 200ms) delay between packets of a BAM message, appendix E, p. 21
#define J1939_TP_TX_FRAME_DELAY			1		// *50ms

/// 200ms max + 50ms margin = 250ms for receival of a BAM message, appendix E, p. 21
#define J1939_TP_RX_PACKET_TIMEOUT		5		// *50ms

/// 500ms one direction progration delay. Max = 2 * 500ms + 250 of above = 1250ms.
#define J1939_TP_TURNAROUND_TIMEOUT		25		// *50ms

/// max delay for CTS pause (zero frames), appendix E, p. 17. 500ms propagation + 50ms margin
#define J1939_TP_CTS_PAUSE_TIMEOUT		11		// *50ms

/* Transport Protocol Connection Management (TP.CM in J1939 and 
 * ISO_TP_CONNECTION_MSG above) group function control value defintions
 */
#define J1939_TP_CM_RTS          16          // request to send
#define J1939_TP_CM_CTS          17          // clear to send
#define J1939_TP_CM_EOM		     19          // end of message acknowledge
#define J1939_TP_CM_BAM          32          // broadcast announce message
#define J1939_TP_CM_CONN_ABORT  255          // connection abort

/** The number of tp frames needed to transfer 'length' bytes. There are 7 bytes
 * per frame. The number division must be floored, hence the minus and plus one.
 * Length = 0 is invalid and leads to invalid results!
 */
#define J1939_TP_LENGTH_TO_NR_FRAMES(length)	((un8) ((length - 1) / 7 + 1))

/// from a frame number to the position in the data, nr cannot be 0
#define J1939_TP_FRAME_TO_POS(nr)			(((un16) nr - 1) * 7 )

// internal states for BAM and RTS/CTS
typedef enum
{
	J1939_TP_WAITING_TO_START,	///< Message has not started transmitting
	J1939_TP_SENDING_DATA,		///< Transmitting data frames
	J1939_TP_SEND_A_CTS,		///< Transmit a CTS
	J1939_TP_WAITING_FOR_DATA,  ///< Waiting for TP.DT frames
	J1939_TP_WAITING_FOR_CTS,	///< Waiting for CTS to continue
	J1939_TP_WAITING_FOR_EOM,	///< Waiting for End of Message Ack
	J1939_TP_SEND_EOM,			///< Transmit a EOM
	J1939_TP_COMPLETE,			///< Complete
	J1939_TP_ABORT				///< Send abort
} J1939TpState;

#ifndef CFG_J1939_TP_MSG_SIZE
#define CFG_J1939_TP_MSG_SIZE 2000
#endif

/** J1939 transport message (RTS/CTS or BAM).
@extends J1939Msg
*/
typedef struct _J1939Tp
{
	J1939Id				msgId;						///< PGN describing data contents
	un16				length;						///< number of bytes used 
	VeCanFlags			flags;						///< flags for special purposes.
	un8					type;						///< fp in this case 
	struct _J1939Tp*	next;						///< next message in list
	un8					mdata[CFG_J1939_TP_MSG_SIZE];	// data bytes of message
	un16				timeout;					///< Transport abort timeout counter
	un8					*state;						///< track progress (NOT used at the moment)
	J1939TpState		tpState;					///< transport protocol transmit state
	un8					nextSeqId;					///< next expected sequence id
	un8					dataFrameCount;				///< data frames expected before handshake 
	J1939Nad			receiver;					///< the intended receiver of the TP message
} J1939Tp;	

typedef struct {
	J1939Tp*	rxFree;
	J1939Tp*	txFree;
	J1939Tp*	rx;
	J1939Tp*	tx;
} J1939TpVars;

void			j1939TpInit(void);
veBool			j1939TpSend(J1939Tp* tp);
void			j1939TpTxTick(void);
void			j1939TpTxUpdate(void);

J1939Tp*		j1939TpTxAlloc(void);
void			j1939TpTxFree(J1939Tp* tp);
void			j1939TpTxPush(J1939Tp *tp, un8 *msgState);
J1939Tp*		j1939TpTxFind(J1939Sf const *sf);

void			j1939TpRxTick(void);
void			j1939TpRxUpdate(void);
void			j1939TpRxFree(J1939Tp* tp);
J1939Tp*		j1939TpRxAlloc(void);
J1939Tp*		j1939TpRxFind(J1939Sf const* sf);

// helpers
void			j1939TpDequeue(J1939Tp **queue, J1939Tp *tp);
void			j1939TpPrepare(J1939Id dataId, J1939Sf* sf);
void			j1939TpTxPrepare(J1939Tp const* tp, J1939Sf* sf);
J1939Id			j1939TpDataPgn(J1939Sf const *sf);

// Receiving TP-message
veBool			j1939TpRecBAM(J1939Sf const *sf);
VE_API	void	j1939TpRecDATA(J1939Sf const *sf);
veBool			j1939TpRecRTS(J1939Sf const *sf);
veBool			j1939TpSendEOM(J1939Tp const *tp);
veBool			j1939TpSendCTS(J1939Tp const *tp);
veBool			j1939TpRxSendABORT(J1939Id dataId, J1939Nad receiver);
void			j1939TpRxRecABORT(J1939Sf const *sf);

// Sending TP-message
veBool			j1939TpSendDATA(J1939Tp const *tp);
veBool			j1939TpSendBAM(J1939Tp const *tp);
void			j1939TpRecCTS(J1939Sf const *sf);
void			j1939TpRecEOM(J1939Sf const *sf);
veBool			j1939TpTxSendABORT(J1939Tp const *tp);
void			j1939TpTxRecABORT(J1939Sf const *sf);
veBool			j1939TpValidResponse(J1939Tp const *tp, J1939Sf const *sf);

// Receiving and sending TP-messages
VE_API	veBool	j1939TpRecRTSGroup(J1939Sf const *sf);

/// @}

#endif
