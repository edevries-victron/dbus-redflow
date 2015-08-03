#include <velib/velib_inc_J1939.h>
#include <velib/canhw/canhw.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_stack.h>
#include <velib/utils/ve_assert.h>

#define CFG_J1939_SF_MSGS (CFG_J1939_SF_RX_MSGS + CFG_J1939_SF_TX_MSGS)
static J1939Sf	sfMsgMem[CFG_J1939_SF_MSGS];


/** 
 * @brief
 *	Initializes the free list for single CAN bus messages.
 * @details
 *	For the fixed size queue version the total number of single frames must be set
 *	- reception must be set with @ref CFG_J1939_SF_RX_MSGS. 
 *  - transmission must be set with @ref CFG_J1939_SF_TX_MSGS.
 * These values can be zero.
 */
void j1939SfInit(void)
{
	un8 n = 0;

#if CFG_J1939_SF_TX_MSGS != 0
	j1939Stack.sf.txFree = NULL;
	while (n < CFG_J1939_SF_TX_MSGS)
	{
		sfMsgMem[n].next = (J1939Msg *) j1939Stack.sf.txFree;
		sfMsgMem[n].type.w = J1939_SF_TX;
		j1939Stack.sf.txFree = &sfMsgMem[n++];
	}
#endif

#if CFG_J1939_SF_RX_MSGS != 0
	j1939Stack.sf.rxFree = NULL;
	while(n < CFG_J1939_SF_MSGS)
	{
		sfMsgMem[n].next = (J1939Msg *) j1939Stack.sf.rxFree;
		sfMsgMem[n].type.w = J1939_SF_RX;
		j1939Stack.sf.rxFree = &sfMsgMem[n++];
	}
#endif
}

#if CFG_J1939_SF_RX_MSGS != 0

/**
 * @brief 
 *	Allocates a J1939Sf message for receiving from the bus.
 *
 * @return 
 *	The allocated message or NULL if none available.
 */
J1939Sf* j1939SfRxAlloc(void)
{
	J1939Sf* ret;
		
	// check if any single frame message storage blocks
	// are left in the free queue
	veCanLockClaim();
	if (j1939Stack.sf.rxFree == NULL) {
		veCanLockRelease();
		return NULL;	
	}
		
	// get a message buffer off the top of the free list
	ret = j1939Stack.sf.rxFree;
	j1939Stack.sf.rxFree = (J1939Sf*) ret->next;
	veCanLockRelease();

	ret->next = NULL;
	ret->length = 8;					// max size

	return ret;
}

/** 
 * @brief Gives a rx message back to the stack.
 */
void j1939SfRxFree(J1939Sf *sf)
{
	veAssert(sf != NULL && sf->type.w == J1939_SF_RX);

	veCanLockClaim();
	sf->next = (J1939Msg*) j1939Stack.sf.rxFree;
	j1939Stack.sf.rxFree = sf;
	veCanLockRelease();
}

#endif

#if CFG_J1939_SF_TX_MSGS != 0

/**
 * @brief 
 *	Allocates a J1939Sf message for sending to the bus also without valid NAD.
 * @note
 *	Also allocates a frame when no valid address is obtained.
 * @return 
 *	The allocated message or NULL if none available.
 */
J1939Sf* j1939SfTxAllocNoNad(void)
{
	J1939Sf* ret;

	veCanLockClaim();
	if (j1939Stack.sf.txFree == NULL) {
		veCanLockRelease();
		return NULL;
	}

	// get a message buffer off the top of the free list
	ret = j1939Stack.sf.txFree;
	j1939Stack.sf.txFree = (J1939Sf*) ret->next;
	veCanLockRelease();

	ret->next = NULL;
	ret->flags = VE_CAN_EXT;
	ret->length = 8;					// max size

	return ret;
}

/**
 * @brief Gives a tx message back to the stack.
 */
void j1939SfTxFree(J1939Sf *sf)
{
	veAssert(sf != NULL);
	veAssert(sf->type.w == J1939_SF_TX);

	veCanLockClaim();
	sf->next = (J1939Msg*) j1939Stack.sf.txFree;
	j1939Stack.sf.txFree = sf;
	veCanLockRelease();
}

#endif

