#include <velib/velib_inc_J1939.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_device.h>
#include <velib/nmea2k/n2k_fp.h>
#include <velib/utils/ve_assert.h>

static N2kFp fpTx;

/// mark tx as unused..
void n2kFpQueueTxInit(void)
{
	fpTx.flags = 0;
}

/** 
 * @brief Allocates a FP message for sending.
 */
N2kFp* n2kFpTxAlloc(void)
{
	// allocation fails if no valid network address is obtained
	if (!(j1939Device.flags & J1939_ACL_VALID_NAD) || fpTx.flags != 0)
		return NULL;

	fpTx.type.w = N2K_FP_TX;
	fpTx.next = NULL;
	fpTx.flags = VE_CAN_EXT;
	fpTx.length = CFG_N2K_FP_MSG_SIZE;
	return &fpTx;
}

/** 
 * @brief Hands a tx fast packet back to the stack
 */
void n2kFpTxFree(N2kFp *msg)
{
	veAssert(msg == &fpTx);
	
	fpTx.flags = 0;
}
