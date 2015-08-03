#include <velib/velib_inc_J1939.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_device.h>
#include <velib/J1939/J1939_stack.h>
#include <velib/nmea2k/n2k_fp.h>
#include <velib/utils/ve_assert.h>

/** 
*	@file 
*		Keeps track of unused fast packet messages by a queue.
*		To fill a queue, use the appriopiate free functions at startup.
*		This is seperate from the n2k handling since platforms with a heap
*		can allocate messages on the fly, see n2k_fp_mem.
*/

#define CFG_N2K_FP_MSGS		(CFG_N2K_FP_RX_MSGS + CFG_N2K_FP_TX_MSGS)

static N2kFp	fpMsgMem[CFG_N2K_FP_MSGS];

/**
 * Places the reserved fast packet messages on the free lists.
 * Only applicable when fixed amount of memory is reserved for the messages.
 */
void n2kFpQueueRxInit(void)
{
	un8 n;
	
	j1939Stack.fp.rxFree = NULL;
	j1939Stack.fp.txFree = NULL;

	n = 0;
	while (n < CFG_N2K_FP_MSGS)
	{
		if (n < CFG_N2K_FP_TX_MSGS) {
			fpMsgMem[n].next = (J1939Msg *) j1939Stack.fp.txFree;
			fpMsgMem[n].type.w = N2K_FP_TX;
			j1939Stack.fp.txFree = &fpMsgMem[n++];
		} else {
			fpMsgMem[n].next = (J1939Msg *) j1939Stack.fp.rxFree;
			fpMsgMem[n].type.w = N2K_FP_RX;
			j1939Stack.fp.rxFree = &fpMsgMem[n++];
		}
	}
}

/// Already done in n2kFpQueueRxInit
void n2kFpQueueTxInit()
{
}

/** 
 * @brief Allocates a message for receiving
 *
 * @note 
 *	This function is normally only called from n2kFpData.
 */
N2kFp* n2kFpRxAlloc(void)
{
	N2kFp* ret;

	// check if any fast packet message is left
	if (j1939Stack.fp.rxFree == NULL)
		return NULL;

	// get a message buffer off the top of the free list
	ret = j1939Stack.fp.rxFree;
	j1939Stack.fp.rxFree = (N2kFp*) ret->next;
	ret->next = NULL;
	ret->length = CFG_N2K_FP_MSG_SIZE;
	ret->flags = VE_CAN_EXT;
	return ret;
}

/** 
 * @brief Hands a rx fast packet back to the stack
 */
void n2kFpRxFree(N2kFp *msg)
{
	veAssert(msg && msg->next == NULL && msg->type.w == N2K_FP_RX);

	// place the message on the top of the free list
	msg->next = (J1939Msg*) j1939Stack.fp.rxFree;
	j1939Stack.fp.rxFree = msg;
}

#ifndef CFG_N2K_FP_TX_DISABLE

/** 
 * @brief Allocates a FP message for sending
 */
N2kFp* n2kFpTxAlloc(void)
{
	N2kFp* ret;

	// allocation fails if no valid network address is obtained
	if (!(j1939Device.flags & J1939_ACL_VALID_NAD))
		return NULL;

	veCanLockClaim();

	// check if any fast packet message is left
	if (j1939Stack.fp.txFree == NULL) {
		veCanLockRelease();
		return NULL;
	}

	// get a message buffer off the top of the free list
	ret = j1939Stack.fp.txFree;
	veAssert(ret->next == NULL || ret->next->type.p.kind == N2K_FP);
	j1939Stack.fp.txFree = (N2kFp*) ret->next;
	veCanLockRelease();

	ret->next = NULL;
	ret->flags = VE_CAN_EXT;
	ret->length = CFG_N2K_FP_MSG_SIZE;

	return ret;
}

/** 
 * @brief Hands a tx fast packet back to the stack
 */
void n2kFpTxFree(N2kFp *msg)
{
	veAssert(msg && msg->next == NULL && msg->type.w == N2K_FP_TX);

	// place the message on the top of the free list
	veCanLockClaim();
	msg->next = (J1939Msg*) j1939Stack.fp.txFree;
	j1939Stack.fp.txFree = msg;
	veCanLockRelease();
}
#endif
