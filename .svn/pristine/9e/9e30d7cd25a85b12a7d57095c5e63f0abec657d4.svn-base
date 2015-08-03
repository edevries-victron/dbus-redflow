/**
 * @addtogroup VELIB_NMEA2K_FP
 * @brief NMEA 2000 Fast Packet transport protocol
 *	
 * @details
 *	The Fast Packet Transport is a simple transport protocol defined by 
 *	NMEA 2000 (not J1939), to send messages longer then 8 bytes. Fast 
 *	refers to the fact that there is no required inter-frame space like
 *	the J1939 BAM transport has.
 *
 *	Whenever the same N2kFp is send, it should have the subsequent sequence
 *	(3-bit looping counter). Each frame contains a 5-bit, non looping counter.
 *	These two fields make up the first byte in all the Fast Packet frames. The
 *	used data length is prepended to the first frame (byte). The maximum size 
 *	is therefore 31 * 7 + 6 = 223 bytes. The last frame is padded with 0xFF 
 *	till the full 8 data bytes of the CAN messages are filled.
 *
 *	An annoying fact about the fast packet transport is that it can't be
 *	recognized as such. The message router is therefore responsible for 
 *	sending only fast packet data frames to n2kFpData.
 *
 *	The queues for TX and RX messages are seperated to prevent that the
 *	device uses all frames for sending, leaving none to handle incoming 
 *	messages. 
 *
 *	Fixed number of messages is supported by n2k_fp_queue.c, @ref CFG_N2K_FP_MSGS
 *	should be defined for the total number of messages to be reserved. 
 *	@ref CFG_N2K_FP_TX_MSGS for the number of these messages which should be
 *	reserved for TX messages. n2k_fp_mem.c uses the heap for allocation.
 *
 * @b config
 *		- @ref CFG_N2K_FP_TX_SEND_SINGLE
 *		- @ref CFG_N2K_FP_MSGS
 *		- @ref CFG_N2K_FP_TX_MSGS
 *		- @ref CFG_N2K_FP_MSG_SIZE
 *
 * @b requirements
 *	-	@ref N2kFpVars must be added to the @ref j1939Stack in velib/velib_inc_J1939.h.
 *	-	@ref n2kFpInit must be called before using other functions.
 */

#include <stdio.h>

#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_stack.h>
#include <velib/nmea2k/n2k_fp.h>
#include <velib/canhw/canhw.h>
#include <velib/utils/ve_assert.h>

/** 
 * @brief
 *	Initializes the fastpacket lists.
 * @details
 *	If @ref CFG_N2K_FP_MSGS is defined for fixed amounts of messages
 *	@ref n2kFpQueueInit is also called.
 */

void n2kFpInit(void)
{
#if CFG_N2K_FP_RX_MSGS != 0
	n2kFpQueueRxInit();
#endif
#if CFG_N2K_FP_TX_MSGS != 0
	n2kFpQueueTxInit();
#endif

	j1939Stack.fp.rxInProgress = NULL;
}

/** 
 * @brief 
 *	Reconstructs a N2kFp from the J1939Sf messages.
 *
 * @details
 *	This routine is responsible for accumulating all the frames of a
 *	fast packet message. If a message with a wrong sequence or counter
 *	is received, the message is removed. Multiple fastpackets can be 
 *	reconstructed simultaniously as long as buffer are avaialble.
 *
 *	For this routine to successfully reconstruct a complete N2kFp, 
 *	every frame of the fast packet must be received in numerical order with
 *	the same sequence value, source address, pgn and increasing counter.
 *	The frames must arrive within 750 millisecond or else @ref n2kFpTick will
 *	free the partially reconstructed N2kFp.
 *
 * @param frame	
 *	The incoming frames which are part of a fast packet.
 *
 * @return		
 *	A reconstructed message if the incoming message complete it,
 *  NULL otherwise.
 *
 * @note
 *	Incoming fast packets larger then @ref CFG_N2K_FP_MSG_SIZE are ignored.
 */

N2kFp* n2kFpData(J1939Sf *frame)
{
	N2kFp*			curPtr;				// points to message being evaluated
	N2kFp*			prevPtr;			// points to last message evaluated
	un8				dataIndex;			// loops through frame data bytes
	un8				sequence, counter;
	un8				priority;

	sequence = (un8) ((frame->mdata[0] >> 5) & 0x07);
	counter = (un8) (frame->mdata[0] & 0x1F);

	// remove the priority. Only usefull for arbitration, annoying otherwise.
	priority = frame->msgId.p.pg.p.pri;
	frame->msgId.p.pg.p.pri = 0;

	// Check if the message is already being reconstructed. 
	curPtr = j1939Stack.fp.rxInProgress;
	prevPtr = NULL;
	while (curPtr != NULL)
	{
		veAssert(curPtr->next == NULL || curPtr->next->type.p.kind == N2K_FP);

		// If PGN and source address match then try to continue assembly 
		if (frame->msgId.w == curPtr->msgId.w)
			break;

		prevPtr = curPtr;
		curPtr = (N2kFp*) curPtr->next;
	}

	// Check if the pgn and sequence matches. 
	if (curPtr && curPtr->sequence == sequence && (curPtr->length - 6) / 7 + 1 == counter)
	{
		dataIndex = 1;
	}
	else if (counter == 0)		// start of new fast packt
	{
		// if the packet is longer then the storage space, ignore it
		if (frame->mdata[1] > CFG_N2K_FP_MSG_SIZE)
			return NULL;

		//	If a frame of the last fp is missed, the fp is reused directly, 
		//	in order to prevent delays. Normally a new frame must be allocated.
		if (curPtr == NULL)
		{
			if ( (curPtr = n2kFpRxAlloc()) == NULL)
			{
				// No message buffer available, forced to drop message
				return NULL;
			}

			// place a new message in the stack's in progress queue (if it was not yet there)
			curPtr->next = (J1939Msg*) j1939Stack.fp.rxInProgress;
			j1939Stack.fp.rxInProgress = curPtr;
			// A short fast packet will directly be removed from the queue.
			// Since it is added add the begin of the queue, invalidate prevPtr.
			prevPtr = NULL;
		}

		// a new fastpacket, setup fields
		curPtr->msgId.w = frame->msgId.w;
		curPtr->length = 0;
		curPtr->sequence = sequence;
		curPtr->bytes = frame->mdata[1];

		// copy the new data bytes onto the end of the message
		dataIndex = 2;
	} 
	else		// invalid
	{
		// Free fp buffer if there was any.
		if (curPtr)
		{
			// Get the message out of the list
			if (prevPtr)
			{
				veAssert((N2kFp*) prevPtr->next == curPtr);
				prevPtr->next = curPtr->next;
			}
			else
				j1939Stack.fp.rxInProgress = (N2kFp*) curPtr->next;

			curPtr->next = NULL;

			// place the message back in the free message queue
			n2kFpRxFree(curPtr);
		}

		return NULL;
	}


	// -- action for correct frame

	// set the maximum receive timeout
	curPtr->timeout = CFG_N2K_FP_RX_FRAME_TIMEOUT;

	// copy the new data bytes onto the end of the message
	while ( dataIndex < 8 && curPtr->length < curPtr->bytes)
	{
		curPtr->mdata[curPtr->length] = frame->mdata[dataIndex];
		curPtr->length++;
		dataIndex++;
	}

	// if this frame completes the message return it
	if (curPtr->length == curPtr->bytes)
	{
		// add priority of the last frame, so it looks correctly.
		curPtr->msgId.p.pg.p.pri = priority;

		// Get the message out of the list
		if (prevPtr)
		{
			veAssert((N2kFp*) prevPtr->next == curPtr);
			prevPtr->next = curPtr->next;
		}
		else
			j1939Stack.fp.rxInProgress = (N2kFp*) curPtr->next;

		curPtr->next = NULL;

#if CFG_CAN_MSG_EXTRA
		curPtr->extra = frame->extra;
#endif

		return curPtr;
	}

	return NULL;
}


/** 
 * @brief 
 *	Removes the Fast Packets which timed-out
 *
 * @details
 *	The fast packet messages that are timed-out returned to the stacks 
 *	rx free message queue.
 */

void n2kFpTick(void)
{
	N2kFp* curPtr;		// fast packet message being evaluated
	N2kFp* prevPtr;		// fast packet message previously evaluated
	N2kFp* tempPtr;		// points to the message being removed

	// walk through the entire queue. NULL is end of list.
	curPtr = j1939Stack.fp.rxInProgress;
	prevPtr = NULL;

	while (curPtr != NULL)
	{
		// the message's timed-out when it was or becomes 0
		if (curPtr->timeout == 0 || --curPtr->timeout == 0)
		{
			// remove the message from the list
			if (prevPtr)
				prevPtr->next = curPtr->next;
			else
				j1939Stack.fp.rxInProgress = (N2kFp*) curPtr->next;

			// keep a pointer to be able to free it
			tempPtr = curPtr;
			curPtr = (N2kFp*) curPtr->next;
			tempPtr->next = NULL;

			// place the message back in the free message queue
			n2kFpRxFree(tempPtr);
		}
		else
		{
			// get the next message
			prevPtr = curPtr;
			curPtr = (N2kFp*) curPtr->next;
		}
	}
}

#ifndef CFG_N2K_FP_TX_DISABLE

/** 
* @brief Sets the sequence number and the send index to the beginning. 
*
* @details
*	The sequence is automatically updated to the next valid one.
*
* @param msg
*	The message to set the sequence to.
*
* @param sequence
*	The sequence number which must be used for the fast packet.
*/

void n2kFpSequence(N2kFp *msg, un8 *sequence)
{
	veAssert(msg && sequence && msg->type.p.kind == N2K_FP && msg->next == NULL);

	// store the current sequence value increment it
	msg->sequence = *sequence;
	*sequence = (un8)((*sequence + 1) & 0x07);

	// reset the index pointer to the start, in order to check progress
	msg->bytes = 0;
}

/** 
 * @brief Send (part of) a the N2kFp to the canhw.
 *
 * @details
 *	This routine creates the single CAN-bus messages for the Fast Packets
 *	with adds the sequence identifier and frame counter to the Fast Packet 
 *	message's frames and sends the single frames to the hardware.
 *
 * @param fullMsg
 *	 the fast packet message to send
 *
 * @return 
 *	veTrue if the whole fast packet is send. veFalse otherwise.
 *
 * @note
 *	- @ref n2kFpSequence must be called first.
 *
 *	- The function can / must be called multiple times. It continues where it has
 *	ended the last time.
 *
 *	- This never times out.
 */
veBool n2kFpSend(N2kFp *fullMsg)
{
	VeRawCanMsg		frame;			// frame data
	un8				dataIndex;		// frame data looper
	un8				byteIndex;		// looper in message

	// initialize total message bytes sent counter
	byteIndex = fullMsg->bytes;

	// Copy message info into SF
	frame.length = 8;
	frame.flags = fullMsg->flags;
	frame.canId = fullMsg->msgId.w;
#if CFG_CANHW_J1939_COMPATIBLE
	frame.notused1 = J1939_SF_TX;	// for assertion only
#endif

	// repeat until all bytes have been packed into a frame and
	// passed to the hardware.
	while (byteIndex < fullMsg->length)
	{
		frame.mdata[0] = (un8)((fullMsg->sequence << 5) & 0xE0);

		// increment the fast packet frame counter
		if (byteIndex == 0)
		{
			// prepare the first message for transmission
			frame.mdata[1] = (un8) (fullMsg->length);
			dataIndex = 2;
		} else {
			frame.mdata[0] += ((byteIndex + 1) / 7);
			dataIndex = 1;
		}

		while(dataIndex < 8)
		{
			if (byteIndex < fullMsg->length)
				frame.mdata[dataIndex] = fullMsg->mdata[byteIndex++];
			else
				frame.mdata[dataIndex] = 0xFF;

			dataIndex++;
		}
		
		if (!veCanSend( (VeRawCanMsg*) &frame))
			return veFalse;

		// Remember how far it is send
		fullMsg->bytes = byteIndex;

#if CFG_N2K_FP_TX_SEND_SINGLE
		// if requested not to spin, return immediatly
		return byteIndex >= fullMsg->length;
#endif

	}

	return veTrue;
}

#endif
