/**
 * @addtogroup VELIB_J1939_TX 
 * @brief Outgoing queue to the J1939 CAN-bus
 *	
 * @details
 *	The j1939 TX queue keeps track of the CAN-bus messages which must be send 
 *	to the @ref VELIB_CANHW. The advantage of a queue is that sending a message 
 *	can not fail after a message has been allocated. 
 *
 *	When there is no queue, all functions must check if the actually
 *	transfering to the hardware was not, partially or completly done. The 
 *	application then needs to check and handle the success of the transmission
 *	for each message it sends. If the NAD address is lost all these locations 
 *  should also stop sending if they where.
 * 
 *	With a tx queue it is only a matter of checking if a message could be 
 *	allocated, and therefore the message only needs to be constructed once. 
 *	There is a single point, the message router, where trying / continuing to 
 *	send messages to the hardware should be done. The message router should free
 *	the message after the message is send. If the NAD address is lost the 
 *	message belonging to this address are simply freed.
 *
 *	This implementation is a FIFO queue; it doesn't take the CAN-bus priorities
 *	into account. A priority based list can easily be implemented by leaving
 *	this file out and create a new one with message ordering based on the CAN 
 *	priority. This doesn't make a lot of sense if the @ref VELIB_CANHW interface 
 *	doesn't do the same, which none of them do at the moment. For now it is just
 *	a matter of not providing too many tx messsages to the stack if the time 
 *	between queuing and sending should be limited.
 *
 * @note
 *	For the first person running into problems with a low priority message 
 *	blocking the bus, while there is a important message to be send...
 *
 *	-	One possible implementation would be to let the @ref VELIB_CANHW interface 
 *		use messag slots depending on (part of) the pgn (e.g. the priority) and make 
 *		this list sorted by CANid.
 *	-	Another possible implementation would be to let the CANhw interface 
 *		actually dequeue a message which was in the way. This message could be 
 *		added to the tx queue where it would find an appropiate spot based on 
 *		priority. At some point there will be no buffer left however and output
 *		will still suffer from priority inversion.
 *	-	The most straightforward solution seems however to use a dedicated CAN slot 
 *		for really time critical information. All others just have to wait their turn.
 *
 * @b requirements
 *	-	@ref J1939TxVars must be added to the @ref j1939Stack in velib/velib_inc_J1939.h.
 */

#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_device.h>
#include <velib/J1939/J1939_stack.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_todo.h>

/** 
 * @brief 
 * 	Removes and returns the front message of the TX queue.
 *
 * @return
 *	The message removed from the queue. NULL if no message was queued.
 */
J1939Msg* j1939TxPop(void)
{
	J1939Msg *ret;

	veCanLockClaim();

	if (!j1939Stack.tx.begin) {
		ret = NULL;
		goto out;
	}
	
	/*
	 * Return the message pointed to by the stacks next out pointer
	 * and move the stacks next out pointer to next message in line.
	 */
	ret = j1939Stack.tx.begin;
	j1939Stack.tx.begin = ret->next;
	ret->next = NULL;

	/*
	 * If the last message has been removed from the sent queue
	 * make sure both pointers indicate an empty queue.
	 */
	if (!j1939Stack.tx.begin)
		j1939Stack.tx.end = NULL;

 out:
	veCanLockRelease();

	return ret;
}

/**
 * @brief Enqueues a message for sending. 
 *
 * The src of the message will be set to the NAD of the current device.
 *
 * @param msg
 *	the message which must be transfered to the CANhw
 */
void j1939TxPush(J1939Msg* msg)
{
	veAssert(msg != NULL && msg->next == NULL);

	msg->msgId.p.src = j1939Device.nad;

	veCanLockClaim();
	if (j1939Stack.tx.end)
	{
		/* place the  message at the end of the queue */
		j1939Stack.tx.end->next = msg;
		j1939Stack.tx.end = msg;
	} else {
		/* start new queue */
		j1939Stack.tx.begin = msg;
		j1939Stack.tx.end = j1939Stack.tx.begin;
	}
	veCanLockRelease();

#if CFG_CANHW_TX_INTERRUPT
	if (veCanSendPossible())
		veCanTxEvent();
#endif

	veTodo();
}	

/**
 * @brief readds a message to the front
 *
 * This can be used in case a message could not be send.
 * @param msg the message to retry later.
 * @note j1939TxPop / j1939TxPushFront is prefered over using j1939TxCurrent
 * 		 since the later might have its next field set.
 */
void j1939TxPushFront(J1939Msg* msg)
{
	veAssert(msg != NULL && msg->next == NULL);

	veCanLockClaim();
	msg->next = j1939Stack.tx.begin;
	j1939Stack.tx.begin = msg;

	if (!j1939Stack.tx.end)
		j1939Stack.tx.end = j1939Stack.tx.begin;
	veCanLockRelease();
}

/**
 * @brief 
 * If a message is queued for sending.
 */
veBool j1939TxEmpty(void)
{
	veBool ret;

	veCanLockClaim();
	ret = j1939Stack.tx.begin == NULL;
	veCanLockRelease();

	return ret;
}
