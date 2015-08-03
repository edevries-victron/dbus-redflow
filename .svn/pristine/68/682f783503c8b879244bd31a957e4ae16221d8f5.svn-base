#include <stdlib.h>

#include <velib/velib_inc_J1939.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_device.h>
#include <velib/nmea2k/n2k_fp.h>
#include <velib/utils/ve_assert.h>

/** 
 *	@file 
 *		Dynamically allocates fastpackets. For platforms without a heap
 *		see n2k_fp_queue.c.
 */

/** 
 * @brief Allocates a message for receiving
 *
 * @note 
 *	This function is normally only called from n2kFpData.
 */
N2kFp* n2kFpRxAlloc(void)
{
	N2kFp* ret;

	ret = (N2kFp*) malloc(sizeof(N2kFp));
	if (ret != NULL)
	{
		ret->type.w = N2K_FP_RX;
		ret->flags = VE_CAN_EXT;
		ret->length = CFG_N2K_FP_MSG_SIZE;
		ret->next = NULL;
	}
	return ret;
}

#ifndef CFG_N2K_FP_TX_DISABLE
/** 
 * @brief Hands a rx fast packet back to the stack
 */
void n2kFpRxFree(N2kFp *msg)
{
	veAssert(msg && msg->next == NULL && msg->type.p.kind == N2K_FP);
				
	// place the message on the top of the free list
	free(msg);
}

/** 
 * @brief Allocates a message for sending
 */
N2kFp* n2kFpTxAlloc(void)
{
	N2kFp* ret;
	if (!(j1939Device.flags & J1939_ACL_VALID_NAD))
		return NULL;

	// rest is equal to rx allocation
	ret = n2kFpRxAlloc();
	if (ret == NULL)
		return NULL;
	ret->type.w = N2K_FP_TX;
	return ret;
}

/**
 * @brief Hands a tx fast packet back to the stack
 */
void n2kFpTxFree(N2kFp *msg)
{
	veAssert(msg && msg->next == NULL && msg->type.p.kind == N2K_FP);
				
	free(msg);
}
#endif
