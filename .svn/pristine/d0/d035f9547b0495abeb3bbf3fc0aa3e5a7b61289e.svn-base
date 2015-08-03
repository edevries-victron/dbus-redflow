#include <velib/velib_inc_J1939.h>
#include <velib/J1939/J1939.h>
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

static N2kFp	fpRxMsgMem[CFG_N2K_FP_RX_MSGS];

/**
 * Places the reserved fast packet messages on the free lists.
 * Only applicable when fixed amount of memory is reserved for the messages.
 */
void n2kFpQueueRxInit(void)
{
	un8 n;
	
	j1939Stack.fp.rxFree = NULL;
	for (n = 0; n < CFG_N2K_FP_RX_MSGS; n++)
	{
		fpRxMsgMem[n].type.w = N2K_FP_RX;
		fpRxMsgMem[n].next = NULL;
		n2kFpRxFree(&fpRxMsgMem[n]);
	}
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
