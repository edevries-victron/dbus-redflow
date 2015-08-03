#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/J1939/J1939.h>
#include <velib/utils/ve_todo.h>
#include <velib/J1939/J1939_stack.h>
#include <velib/J1939/J1939_tp.h>

/**
 * Check for timeouts during reception of a TP message
 */
void j1939TpRxTick(void)
{
	J1939Tp* tp = j1939Stack.tp.rx;

	// walk through the entire queue. 
	while (tp != NULL)
	{
		if (tp->timeout != 0 && --tp->timeout == 0)
			tp->tpState = J1939_TP_ABORT;
		
		tp = tp->next;
	}
}

/**
 * Update the progress of a single rx TP message. 
 */
static void j1939TpRxUpdateMsg(J1939Tp* tp)
{
	// if the message's timeout is 0, then the message is incomplete
	// and should be removed from the queue.
	switch (tp->tpState)
	{
		// RTS / CTS only
		case J1939_TP_SEND_A_CTS:
			// Report back how many data frames can be accepted. This depends
			// on the number of buffers available.
			tp->dataFrameCount = CFG_J1939_TP_RX_BUF_SIZE;

			if (j1939TpSendCTS(tp))
			{
				// CTS was sent to hardware successfully so move connection
				// to waiting for a data in response to the CTS
				tp->timeout = J1939_TP_TURNAROUND_TIMEOUT;
				tp->tpState = J1939_TP_WAITING_FOR_DATA;
			}
			break;

		case J1939_TP_SEND_EOM:

			if (j1939TpSendEOM(tp))
			{
				// update the transfered target PGN as if it was send directly
				if (tp->msgId.p.pf <= 0xEF)
					tp->msgId.p.ps = tp->receiver;

				// move to receive queue
				j1939TpDequeue(&j1939Stack.tp.rx, tp);
				j1939RxPushFront((J1939Msg*) tp);
			}
			break;

		case J1939_TP_ABORT:
			// If the TP is a BAM or the ABORT is succesfully send...
			if (
					tp->receiver == J1939_NAD_BROADCAST || 
					j1939TpRxSendABORT(tp->msgId, tp->receiver)
				)
			{
				// remove the message
				j1939TpDequeue(&j1939Stack.tp.rx, tp);
				j1939TpRxFree(tp);
			}
			break;
	}
}

/**
 * Continue the progress of all incoming TP messages.
 */
void j1939TpRxUpdate(void)
{
	J1939Tp* curPtr;
	J1939Tp* nextPtr;

	// walk through the entire queue. 
	curPtr = j1939Stack.tp.rx;
	while (curPtr != NULL)
	{
		// @note a copy is needed since the message can get dequeued!
		nextPtr = curPtr->next;
		j1939TpRxUpdateMsg(curPtr);
		curPtr = nextPtr;
	}
}


/**
 * Handle a incomming BAM or RTS by allocating a buffer for it.
 *
 * @note 
 *	There is only one BAM or RTS in a transfer
 * @note 
 *	There can at most be a one RTS and one BAM from a single sender.
 *
 * @returns 
 *	if the message was handled. This might mean succesfully sending a error.
 */
veBool j1939TpRecBAM(J1939Sf const *sf)
{
	J1939Tp*		tp;				// the transport message being reconstructed
	J1939Id			dataId;			// the pgn being transferred
	
	// get PGN from the payload of the message
	dataId = j1939TpDataPgn(sf);

	// Check for TP in progress from same sender and receiver
	// This does discriminate BAM and RTS/CTS.
	tp = j1939TpRxFind(sf);

	// message not found
	if (tp == NULL)
	{
		// If there is no resource available 
		if ((tp = j1939TpRxAlloc()) == NULL)
		{
			// try to send an error if CTS/RTS
			if (sf->msgId.p.ps != 0xFF)
				return j1939TpRxSendABORT(dataId, sf->msgId.p.ps);
			
			return veFalse;
		}

		// add the message to the receive queue
		tp->next = j1939Stack.tp.rx;
		j1939Stack.tp.rx = tp;
	}

	// @todo is the message of interest at all?

	// accept the message
	tp->nextSeqId = 1;
	tp->msgId.w = dataId.w;
	tp->receiver = sf->msgId.p.ps;
	tp->length = ((un16) sf->mdata[2] << 8) | sf->mdata[1];
	tp->timeout = J1939_TP_RX_PACKET_TIMEOUT;
	tp->tpState = J1939_TP_WAITING_FOR_DATA;

	// Send an error if the transport exceeds our message buffer size.
	if (tp->length > CFG_J1939_TP_MSG_SIZE)
	{
		veTodo();
		tp->timeout = 0;
		tp->tpState = J1939_TP_ABORT;
		return veTrue;
	}

	if (tp->receiver != J1939_NAD_BROADCAST)
	{
		veTodo();
		tp->timeout = 0;
		tp->tpState = J1939_TP_SEND_A_CTS;
	}
	
	return veTrue;
}


/**
 * Adds the transfered data to TP being transferred.
 *
 * @retval whether the message could be processed.
 */
void j1939TpRecDATA(J1939Sf const *sf)
{
	J1939Tp*		tp;		// the TP message being reconstructed
	un16			pos;	// current position in the data of the tp
	un8				n;
	
	// Try to locate the message being reconstructed.
	tp = j1939TpRxFind(sf);

	// Error, not found or in error state
	if (tp == NULL || tp->tpState == J1939_TP_ABORT)
		return;

	// Check the sequence number. ERROR when incorrect.
	if (sf->mdata[0] != tp->nextSeqId)
	{
		tp->tpState = J1939_TP_ABORT;
		return;
	}

	// copy data to the TP being reconstructed
	n = 1;
	pos = ((un16) tp->nextSeqId - 1) * 7;
	while (n < 8 && pos < tp->length)
		tp->mdata[pos++] = sf->mdata[n++];

	// data complete?
	if (pos >= tp->length)
	{
		// is it a BAM which is complete?
		if (tp->receiver == J1939_NAD_BROADCAST)
		{
			// update the transfered target PGN as if it was send directly
			if (tp->msgId.p.pf <= 0xEF)
				tp->msgId.p.ps = tp->receiver;

			// move to received queue
			j1939TpDequeue(&j1939Stack.tp.rx, tp);
			j1939RxPushFront((J1939Msg*) tp);
			return;
		} 

		// a RTS/CTS is finished -> send the EOM
		tp->tpState = J1939_TP_SEND_EOM;
		tp->timeout = 0; // asap
		veTodo();
		return;
	}

	// more data is expected
	tp->nextSeqId++;
	tp->timeout = J1939_TP_RX_PACKET_TIMEOUT;
	if (tp->receiver == J1939_NAD_BROADCAST)
		return;

	// CTS only: decrease data counter and check if a new CTS needs to be send.
	tp->dataFrameCount--;
	if (tp->dataFrameCount == 0)
	{
		tp->tpState = J1939_TP_SEND_A_CTS;
		tp->timeout = 0;
		veTodo();
	}
}

/**
 * Return a TP rx message to the stack
 */
void j1939TpRxFree(J1939Tp* tp)
{
	tp->next = j1939Stack.tp.rxFree;
	j1939Stack.tp.rxFree = tp;
}

/**
 * Allocates a message for reconstruction of a incomming TP message
 *
 * @return A TP message or NULL if none available.
 */
J1939Tp* j1939TpRxAlloc(void)
{
	J1939Tp* msg;

	if (j1939Stack.tp.rxFree == NULL)
		return NULL;
	
	msg = j1939Stack.tp.rxFree;
	j1939Stack.tp.rxFree = msg->next;
	msg->next = NULL;

	return msg;
}

/** 
 * Locate a TP-rx being reconstructed by an incoming message.
 *
 * @details
 *	The sender and receiver must match. This allows simulations processing
 *	of RTS/CTS and BAM. Furthermore it allows having several devices sharing
 *	the same stack.
 *
 * @return The matching TP message. NULL otherwise.
 */
J1939Tp* j1939TpRxFind(J1939Sf const* sf)
{
	J1939Tp* tp;

	tp = j1939Stack.tp.rx;
	while (tp != NULL)
	{
		if (tp->msgId.p.src == sf->msgId.p.src && tp->receiver == sf->msgId.p.ps)
			return tp;

		tp = tp->next;
	}

	return NULL;
}
