#include <stdlib.h>

#include <velib/velib_inc_J1939.h>
#include <velib/J1939/J1939.h>
#include <velib/utils/ve_assert.h>

void j1939SfInit(void)
{
}

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
		
	ret = malloc(sizeof(J1939Sf));
	if (ret == NULL)
		return NULL;

	ret->next = NULL;
	ret->length = 8;
	ret->type.w = J1939_SF_RX;
	ret->flags = VE_CAN_EXT;

	return ret;
}

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
	ret = j1939SfRxAlloc();
	if (ret == NULL)
		return NULL;
	ret->type.p.tx = 1;
	return ret;
}

/**
 * @brief Gives a rx message back to the stack 
 */
void j1939SfRxFree(J1939Sf *sf)
{
	veAssert(sf != NULL && sf->type.w == J1939_SF_RX);
	free(sf);
}

/** 
* @brief Gives a tx message back to the stack
*/
void j1939SfTxFree(J1939Sf *sf)
{
	veAssert(sf != NULL && sf->type.w == J1939_SF_TX);
	free(sf);
}
