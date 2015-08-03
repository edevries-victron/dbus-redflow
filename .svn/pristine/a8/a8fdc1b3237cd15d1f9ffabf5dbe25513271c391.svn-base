#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/canhw/canhw.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_stack.h>
#include <velib/J1939/J1939_tp.h>

/**
 * Function which checks if the TP single frame corresponds with the
 * transported frame being reconstructed.
 */
veBool j1939TpValidResponse(J1939Tp const *msg, J1939Sf const *frame)
{
	J1939Id id;

	id = j1939TpDataPgn(frame);
	id.p.src = frame->msgId.p.ps;

	return id.w == (msg->msgId.w & ~0x1C000000);	// ignore priority fields
}

/** 
 * Handles a incoming CTS.
 *
 * @return veFalse if the message could temporarily not be processed.
 */
void j1939TpRecCTS(J1939Sf const *sf)
{
	J1939Tp*  tp;
	un8       framesLeft;

	// no such message..
	if ((tp = j1939TpTxFind(sf)) == NULL)
		return;

	tp->timeout = 0;

	// check if it is the CTS is for the same PGN as intended to send,
	// the sequence is correct and we are expecting a CTS. 
	if (
			!j1939TpValidResponse(tp, sf) || 
			tp->tpState != J1939_TP_WAITING_FOR_CTS || 
			sf->mdata[2] > tp->nextSeqId	// @todo re-request at most 2 times, E. p 21
		)
	{
		tp->tpState	= J1939_TP_ABORT;
		return;
	}

	// if the number of packets cleared to send is 0 then this is a
	// pause transmission frame. Wait for the next CTS, reset timeout.
	if (sf->mdata[1] == 0)
	{
		tp->dataFrameCount = 0;
		tp->tpState = J1939_TP_WAITING_FOR_CTS;
		tp->timeout = J1939_TP_CTS_PAUSE_TIMEOUT;
		return;
	}

	// allow to revert if the receiver missed some frames (ever used?)
	tp->nextSeqId = sf->mdata[2];

	// Send the requested number of frames, but at most the number of frames left.
	framesLeft = J1939_TP_LENGTH_TO_NR_FRAMES(tp->length) - (tp->nextSeqId - 1);
	tp->dataFrameCount = framesLeft < sf->mdata[1] ? framesLeft : sf->mdata[1];
	tp->tpState = J1939_TP_SENDING_DATA;
}

/** 
 * Handles an incoming EOM.
 *
 * @details
 *	only if a EOM is received the message is freed directly.
 */
void j1939TpRecEOM(J1939Sf const *sf)
{
	J1939Tp*	tp;
	un16 		length;

	// find the corresponding transport message.
	if ((tp = j1939TpTxFind(sf)) == NULL)
	   return;

   	length = (un16) (((un16) sf->mdata[2] << 8) | sf->mdata[1]);

	// If an End of Message Acknowledgment is received by the originating 
	// controller prior to the final Data Transfer, then the originating
	// controller ignores it... (quote?)
	// Otherwise block the transmission for a while till timeout...
	if (
			!j1939TpValidResponse(tp, sf) || 
			length != tp->length ||
			sf->mdata[3] != J1939_TP_LENGTH_TO_NR_FRAMES(tp->length) ||
			tp->tpState != J1939_TP_WAITING_FOR_EOM
		)
	{
		return;
	}

	// free the frame
	tp->tpState = J1939_TP_COMPLETE;
	tp->timeout = 0;
}

/**
 * Send an abort of the current transfer to the intended receiver.
 *
 * @param tp
 *	the TP message to abort
 *
 * @return If the abort is succesfully queued
 */

veBool j1939TpTxSendABORT(J1939Tp const* tp)
{
	J1939Sf sf;

	j1939TpTxPrepare(tp, &sf);
	//frame.mdata[0] = J1939_TP_CM_CONN_ABORT; // default

	return veCanSend((VeRawCanMsg*) &sf);
}

/**
 * Handle an ABORT message send by the intended receiver.
 */
void j1939TpTxRecABORT(J1939Sf const *sf)
{
	J1939Tp* tp;

	// Find the corresponding TP message being send
	if ((tp =j1939TpTxFind(sf)) == NULL)
		return;

	// dequeue
	j1939TpDequeue(&j1939Stack.tp.tx, tp);
	j1939TpTxFree(tp);
}

/**
 * Locate a TP message being send from a transport protocol message.
 *
 * @details
 *	Only usefull for RTS / CTS since there are no responses to a BAM transfer.
 *
 * @see j1939TpRxFind.
 */
J1939Tp* j1939TpTxFind(J1939Sf const *sf)
{
	J1939Tp* tp;

	tp = j1939Stack.tp.tx;
	while (tp != NULL)
	{
		if (tp->msgId.p.src == sf->msgId.p.ps && tp->receiver == sf->msgId.p.src)
			return tp;

		tp = tp->next;
	}

	return NULL;
}
