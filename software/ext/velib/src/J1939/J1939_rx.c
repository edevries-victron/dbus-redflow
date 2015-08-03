#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/J1939/J1939.h>
#include <velib/utils/ve_assert.h>
#include <velib/J1939/J1939_stack.h>

/**
 * @addtogroup VELIB_J1939_RX 
 * @brief Incoming queue to the J1939 CAN-bus
 *	
 * @details
 *	When CAN bus message are received on interrupt they can be queued for 
 *	processing by the main loop. When no resources are present for outgoing
 *	message, the incoming message can temporarily be stored on the queue to
 *	wait for resources to come available.
 *
 *	A rx queue is not per definition necessary, message can also be processed
 *	directly when arrived. Library code should therefore not depend on the 
 *	presence of a rx queue.
 */

/** 
 * Pop a message of the rx-queue.
 *
 * @return the incoming message which must be processed first.
 */
J1939Msg* j1939RxPop(void)
{
	J1939Msg *ret;

	veCanLockClaim();
	if (!j1939Stack.rx.begin) {
		veCanLockRelease();
		return NULL;
	}

	// Return the message pointed to by the stacks next out pointer
	// and move the stacks next out pointer to next message in line.
	ret = j1939Stack.rx.begin;
	j1939Stack.rx.begin = ret->next;
	ret->next = NULL;

	// if the last message has been removed from the received queue
	// make sure both pointers indicate an empty queue.
	if (!j1939Stack.rx.begin)
		j1939Stack.rx.end = NULL;

	veCanLockRelease();

	return ret;
}

/** 
 * This routine puts a message at the end of the receive queue, 
 * it will be the last one dequeued.
 *
 * @param msg	  message to be placed at the end of the queue
 */
void j1939RxPush(J1939Msg* msg)
{
	veAssert(msg != NULL && msg->next == NULL);

	veCanLockClaim();

	if (j1939Stack.rx.end)
	{
		// place the  message at the end of the queue
		j1939Stack.rx.end->next = msg;
		j1939Stack.rx.end = msg;
	} else {
		// start new queue
		j1939Stack.rx.begin = msg;
		j1939Stack.rx.end = j1939Stack.rx.begin;
	}

	veCanLockRelease();
}	

/** 
 * This routine puts a message at the beginning of the 
 * receive queue, it will be the first one dequeued.
 *
 * @param msg message to be placed at the beginning of the queue
 */
void j1939RxPushFront(J1939Msg* msg)
{
	veAssert(msg != NULL && msg->next == NULL);

	veCanLockClaim();

	msg->next = j1939Stack.rx.begin;
	j1939Stack.rx.begin = msg;

	if (j1939Stack.rx.end == NULL)
		j1939Stack.rx.end = j1939Stack.rx.begin;

	veCanLockRelease();
}	
