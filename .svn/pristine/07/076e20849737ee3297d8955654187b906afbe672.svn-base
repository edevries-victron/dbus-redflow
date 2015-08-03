/*
 * The simplest implementation to obey the expected function of the sending
 * part. Just check if there is room in the canhw and place the message
 * directly there. This will not work with transport protocols, only with
 * single messages. It is also not interrupt safe (and can't be made as such
 * in a decent manner). It is useful for test scripts / bootloaders though.
 */

#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_device.h>
#include <velib/utils/ve_assert.h>

#if CFG_CANHW_TX_INTERRUPT != 0
#error This code is not interrupt safe!
#endif

#ifdef CFG_WITH_FP
#if CFG_WITH_FP != 0
#error This assumes single CAN messages only!
#endif
#endif

static J1939Sf msg;
static veBool allocated;

J1939Sf* j1939SfTxAlloc(void)
{
#if CFG_WITH_J1939_ACL != 0
	if (!(j1939Device.flags & J1939_ACL_VALID_NAD))
		return NULL;
#endif

	if (allocated || !veCanSendPossible())
		return NULL;

	msg.flags = VE_CAN_EXT;
	msg.type.w = J1939_SF_TX;
	msg.length = 8;
	allocated = veTrue;
	return &msg;
}

void j1939SfTxFree(J1939Sf *sf)
{
	VE_UNUSED(sf);
}

veBool j1939SfSend(J1939Sf *sf)
{
	if (veCanSend((VeRawCanMsg*) sf)) {
		allocated = veFalse;
		return veTrue;
	}
	veAssert(veFalse);
	return veFalse;
}

void j1939TxPush(J1939Msg* msg)
{
	j1939SfSend((J1939Sf *) msg);
}
