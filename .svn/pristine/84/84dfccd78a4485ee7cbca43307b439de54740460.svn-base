/**
* @addtogroup VELIB_J1939_SF
* @brief Single frame messages for the J1939 CAN-bus
*	
* @details
*	The queues for TX and RX messages are seperated to prevent that the
*	device uses all frames for sending, leaving none to handle incoming 
*	messages. 
*
* @b requirements
*	-	@ref J1939SfVars must be added to the @ref j1939Stack.
*	-	@ref j1939SfInit must be called before using the other functions.
*	-	Either J1939_sf_queue.c (fixed memory) or J1939_sf_mem.c (heap) must
*		be included.
*/

#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_device.h>
#include <velib/utils/ve_assert.h>

/**
* @brief 
*	Allocates a J1939Sf message for sending to the bus.
* @note
*	Return NULL when no valid address is obtained.
* @return 
*	The allocated message or NULL if not available / or no access to the bus.
*/
J1939Sf* j1939SfTxAlloc(void)
{
	// check if any single frame message storage blocks left
	if (!(j1939Device.flags & J1939_ACL_VALID_NAD))
		return NULL;
		
	return j1939SfTxAllocNoNad();
}

/** 
 * @brief
 *	Tries to transfer a J1939Sf to the canhw.
 *
 * @return
 *	veTrue if the transfer was successful. veFalse otherwise.
 *
 * @note
 *	No checking is done if the NAD is valid, this is done during allocation.
 */

veBool j1939SfSend(J1939Sf *sf)
{
	// sanity checks
	veAssert(sf != NULL && sf->type.p.kind == J1939_SF && sf->length <= 8);

	// try to send it to the hardware
	return veCanSend((VeRawCanMsg*) sf);
}
