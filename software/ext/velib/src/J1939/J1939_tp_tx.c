#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/canhw/canhw.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_device.h>
#include <velib/J1939/J1939_stack.h>
#include <velib/J1939/J1939_tp.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_todo.h>

/** 
 * Returns a TP message to the stack.
 */
void j1939TpTxFree(J1939Tp* tp)
{
	tp->next = j1939Stack.tp.txFree;
	j1939Stack.tp.txFree = tp;
}

/** 
 * Allocates a TP frame for sending.
 *
 * @return the frame or NULL if none available or no bus access yet
 */
J1939Tp* j1939TpTxAlloc(void)
{
	J1939Tp* tp;

	if (!(j1939Device.flags & J1939_ACL_VALID_NAD) || j1939Stack.tp.txFree == NULL)
		return NULL;
	
	tp = j1939Stack.tp.txFree;
	j1939Stack.tp.txFree = tp->next;
	tp->next = NULL;
	tp->length = CFG_J1939_TP_MSG_SIZE;

	return tp;
}

/**
 * Continue sending the transport protocol messages.
 *
 * @note	
 *	At the moment only one message is send at a time!
 *	While this makes absolute sure no two message are simultaniously send
 *	to the same device / multiple BAMs are send at the same time, it can 
 *	also cause delays if large amounts of CTS/RTS are send to different 
 *	devices.
 */
void j1939TpTxUpdate(void)
{
	J1939Tp* tp;

	if (j1939Stack.tp.tx == NULL)
		return;

	// Try to continue sending the top TP message
	if (j1939TpSend(j1939Stack.tp.tx))
	{
		// sending is done, free the message
		tp = j1939Stack.tp.tx;
		j1939Stack.tp.tx = tp->next;
		tp->next = NULL;
		j1939TpTxFree(tp);
	}
}

/** 
 * Update the timeout counters, expected to be called every 50 milliseconds.
 */
void j1939TpTxTick(void)
{
	J1939Tp * curPtr;

	curPtr = j1939Stack.tp.tx;
	if (curPtr && curPtr->timeout)
		curPtr->timeout--;
}

/** 
 * Queue the message for sending with the Transfer Protocol.
 * 
 * @details
 *	when the member 'receiver' is set to 0xFF the message is send as a BAM.
 *
 * @param tp		The message to send.
 * @param msgState	State to follow progress of the transport (not implemented).
 */
void j1939TpTxPush(J1939Tp *tp, un8 *msgState)
{
	J1939Tp* last;

	veAssert(tp != NULL && tp->next == NULL && tp->type == J1939_TP);
	veTodo();

	// track the status report location with the message
	tp->msgId.p.src = j1939Device.nad;
	tp->state = msgState;
	tp->timeout = (tp->receiver == J1939_NAD_BROADCAST ? 1 : 0); // BAM -> sync to timer
	tp->tpState = J1939_TP_WAITING_TO_START;
	tp->dataFrameCount = J1939_TP_LENGTH_TO_NR_FRAMES(tp->length);
	tp->nextSeqId = 1;

	// The tranported PGN is intended for the same device as the transport
	// protocol is directed to (which might be broadcast)
	if (tp->msgId.p.pf <= 0xEF)
		tp->msgId.p.ps = tp->receiver;

	// Put the message at the end off the queue
	last = j1939Stack.tp.tx;
	if (last == NULL)
	{
		j1939Stack.tp.tx = tp;
	} else {
		while (last->next != NULL)
			last = last->next;
		last->next = tp;
	}
}

/**
 * Continue sending by the transport protocol
 *
 * @returns veTrue when done (and the message can be freed) 
 */
veBool j1939TpSend(J1939Tp* tp)
{
	if (tp->timeout != 0)
		return veFalse;

	// the current state of the message transmission determines what
	// action should be taken
	switch (tp->tpState)
	{
		case J1939_TP_WAITING_TO_START:
			// send BAM or RTS to initiate transmission 
			if (!j1939TpSendBAM(tp))
				break;
		
			// message send, update the state
			if (tp->receiver == J1939_NAD_BROADCAST)
			{
				tp->tpState = J1939_TP_SENDING_DATA;
				tp->timeout = J1939_TP_TX_FRAME_DELAY;
			} else {
				// need to wait for CTS from other side
				tp->tpState = J1939_TP_WAITING_FOR_CTS;
				tp->timeout = J1939_TP_TURNAROUND_TIMEOUT;
			}
			break;
			
		case J1939_TP_WAITING_FOR_CTS:
			tp->tpState = J1939_TP_ABORT;
			break;

		case J1939_TP_SENDING_DATA:
			// send the next data packet
			if (!j1939TpSendDATA(tp))
				break;

			// placed in transmit queue so prep for next packet
			tp->nextSeqId++;
			tp->dataFrameCount--;

			// if the sequence id exceeds the number of packets necessary
			// to completely transmit the message, remove the message from 
			// the queue because it completed transmission.
			if (tp->dataFrameCount == 0)
			{
				// If this is a BAM sending is done 
				if (tp->receiver == J1939_NAD_BROADCAST)
					return veTrue;	// free message

				// A RTS/CTS must wait for confirmation. Either CTS if not done
				// or EOM to acknowlegde full reception.
				tp->timeout = J1939_TP_TURNAROUND_TIMEOUT;
				if (tp->nextSeqId <= J1939_TP_LENGTH_TO_NR_FRAMES(tp->length))
					tp->tpState = J1939_TP_WAITING_FOR_CTS;
				else
					tp->tpState = J1939_TP_WAITING_FOR_EOM;
			
			} else if (tp->receiver == J1939_NAD_BROADCAST) {
				// BAM: set the 50 millisecond minimum time delay
				tp->timeout = J1939_TP_TX_FRAME_DELAY;
			} else {
				// RTS/CTS: no delay, timeout to zero (already the case)
				veTodo();
			}

			break;

		case J1939_TP_ABORT:
			if (tp->receiver == J1939_NAD_BROADCAST)
				return veTrue; // done, free the message
			
			// send an abort for RTS / CTS
			if (j1939TpTxSendABORT(tp))
				return veTrue;
			break;

		case J1939_TP_COMPLETE:
			return veTrue;
	
		default:
			break;
	}

	return veFalse;
}

/**
 * @brief
 *	Sets common data for the tx messages.
 * @details
 * Sets the common parts for a RTS group message directed to the 
 * intended receiver. The pgn of the message, including source and target
 * are set. The pgn in the data is set as well. All other fields are set to 0xFF.
 *
 * @param tp	The TP message to create the control message for
 * @param sf	The control message to prepare for sending
 */
void j1939TpTxPrepare(J1939Tp const* tp, J1939Sf* sf)
{
	j1939TpPrepare(tp->msgId, sf);

	sf->msgId.p.ps = tp->receiver;	// target of this message is the receiver of the TP
	sf->msgId.p.src = tp->msgId.p.src;		// a device in this node that sends it
	sf->mdata[7] &= ~0x1C;					// do not send priority bits!
}

/**
 * Send a BAM / RTS to initiate a transfer.
 *
 * @details
 *	If the msg->receiver is set broadcast the message is transfered as BAM.
 *	Otherwise as a RTS/CTS.
 *
 * @param tp		The message to be transported with RTS/CTS or BAM.
 */
veBool j1939TpSendBAM(J1939Tp const *tp)
{
	J1939Sf sf;
	
	j1939TpTxPrepare(tp, &sf);
	
	sf.mdata[0] = (tp->receiver == J1939_NAD_BROADCAST ? J1939_TP_CM_BAM : J1939_TP_CM_RTS);
	sf.mdata[1] = (un8) tp->length;
	sf.mdata[2] = (un8) (tp->length >> 8);
	sf.mdata[3] = J1939_TP_LENGTH_TO_NR_FRAMES(tp->length);

	return veCanSend((VeRawCanMsg*) &sf);
}

/** 
 * Create the next DATA frame for the message and try to send it.
 */
veBool j1939TpSendDATA(J1939Tp const *tp)
{
	J1939Sf		sf;
	un16		pos;
	un8			n;

	// load the message frame and pass it to the hardware
	sf.flags = VE_CAN_EXT;
	sf.length = 8;

	sf.msgId.p.pg.w = 0;
	sf.msgId.p.pg.p.pri = tp->msgId.p.pg.p.pri;
	sf.msgId.p.pf = J1939_PGN0_PF_TP_DATA;
	sf.msgId.p.ps =	tp->receiver;
	sf.msgId.p.src = tp->msgId.p.src;

	// copy the next 7 bytes of data (unused to 0xFF)
	sf.mdata[0] = tp->nextSeqId;
	pos = J1939_TP_FRAME_TO_POS(tp->nextSeqId);
	for (n = 1; n < 8; n++)
		sf.mdata[n] = pos < tp->length ? tp->mdata[pos++] : 0xFF;

	return veCanSend((VeRawCanMsg*) &sf);
}
