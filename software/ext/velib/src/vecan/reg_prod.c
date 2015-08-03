/**
* Broadcast SF Victron proprietary data.
*
* Since all nodes on a CAN-bus can gain bus-access and send requests, the total
* number of simultaneous requests cannot be controlled from within a device.
* In order to make sure the device is always capable to respond, the messages
* are marked for sending, instead of sending them directly.
*
* Since the majority of vregs are single frame settings, only the changes are
* broadcasted over the CAN bus. To make this rather easy, checking for changes
* and marking the message is done automagically.
*
* When a setting would normally be updated by:
*		someSetting = value;
*
* Can be converted to a vreg by adding the appropriate vreg to the definition
* table and using, e.g.:
*		veRegSetUn16(&someSetting, value);
*
* If the value might need to be stored, the return value can be use to track
* if the value actually changed, so for example:
*		if (veRegSetUn16(&someSetting, value))
*			storeData();
*
* Only the basic types are supported here, more types can be implemented
* elsewhere and calling veRegOnChange(&data) when the value changed.
*
* Requirements:
*	- velib it's io Little Endian integer support (un8 / un16 / un32)
*	- definition of the array:
*		VeRegProdSfDef veRegProdSfDef[] with messages to send.
*		Several variables can be added to the same message,
*		by appending them and specifying the same register ID.
*		This should only be done if there is a really good reason to do so.
*	- The array should end with a regId of 0, iow {0}
*		Corresponding memory should be provided (since at the place of the
*		definition the array length is known.
*
*		// -1 since there is not need to reserve memory for the end record
*		un8 veRegProdSfFlags[ARRAY_LENGTH(veRegProdSfTypedDef) - 1];
*
*	- the usual veRegProdSfInit / veRegProdSfUpdate / veRegProdSfTick
*	- reg_rx should be compiled in and
*		veRegProdSfHandler should be added to veRegRxHndlrDef
*/

#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_acl.h>
#include <velib/nmea2k/n2k.h>
#include <velib/vecan/vecan.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/reg_rx.h>
#include <velib/vecan/reg_prod_sf.h>
#include <velib/utils/ve_todo.h>
#include <velib/utils/ve_assert.h>

#include <string.h>

#define VE_REG_PROD_SF_REQ					0x10
/* Status to indicate the request reason was because the value changed */
#define VE_REG_PROD_SF_REQ_ON_CHANGE		0x20
/* To prevent flooding the bus with on change messages */
#define VE_REG_PROD_SF_SENT_DELAYED			0x40
/* Answer on the loopback as well */
#define VE_REG_PROD_SF_LOOPBACK				0x80

// flags to mark messages for sending
static un8 periods[1] = {100};
static un8 timers[1];
static un8 rateLimit = 20;
static un8 rateLimitTimer;

// proto for actually sending the message
static veBool veRegProdSfSend(VeRegNr nr);

/// Initialises the sf broadcast not to send by default.
void veRegProdSfInit(void)
{
	VeRegNr n = 0;

	do {
		veRegProdSfFlags[n] = (veRegProdSfTypedDef[n].flags & V_TIMER_MASK);
		if ((veRegProdSfTypedDef[n].flags & V_TIMER_MASK) != 0)
			veRegProdSfFlags[n] |= VE_REG_PROD_SF_REQ;
	} while (veRegProdSfTypedDef[++n].regId);
}

/// Continue to send broadcast proprietary sf messages.
void veRegProdSfUpdate(void)
{
	static VeRegNr regNr;

	for (;;)
	{
		if (veRegProdSfFlags[regNr] & VE_REG_PROD_SF_REQ)
		{
			if (!veRegProdSfSend(regNr))
				return; // out of buffers..
			veRegProdSfFlags[regNr]	&= ~VE_REG_PROD_SF_REQ;
		}

		if (!veRegProdSfTypedDef[++regNr].regId) {
			regNr = 0;
			return;
		}
	}
}

void veRegProdSfTick(void)
{
	un8 i;

	/*
	 * There are no per vregs timers since most of them won't be periodic anyway.
	 * Vregs are flagged for rapid, slow update etc.
	 */
	for (i = 0; i < ARRAY_LENGTH(timers); i++)
	{
		if (++timers[i] > periods[i])
		{
			VeRegNr n = 0;
			timers[i] = 0;

			/*
			 * mark the vregs which have this timers as a flag
			 * +1 because 0 is the non-periodic default.
			 */
			do {
				if ((veRegProdSfFlags[n] & V_TIMER_MASK) == (i+1))
					veRegProdSfFlags[n] |= VE_REG_PROD_SF_REQ;
			} while (veRegProdSfTypedDef[++n].regId);
		}
	}

	/* house keeping for onchange rate limit */
	if (++rateLimitTimer >= rateLimit)
	{
		VeRegNr n = 0;
		do {
			/* Allow onchange sending again */
			veRegProdSfFlags[n] &= ~VE_REG_PROD_SF_REQ_ON_CHANGE;

			/* Queue the blocked onchange message now. */
			if (veRegProdSfFlags[n] & VE_REG_PROD_SF_SENT_DELAYED) {
				veRegProdSfFlags[n] &= ~VE_REG_PROD_SF_SENT_DELAYED;
				veRegProdSfFlags[n] |= VE_REG_PROD_SF_REQ | VE_REG_PROD_SF_REQ_ON_CHANGE;
			}
		} while (veRegProdSfTypedDef[++n].regId);

		rateLimitTimer = 0;
	}
}

void veRegProdSfHandler(VeEvent ev)
{
	if (ev == VE_EV_UPDATE)
		veRegProdSfUpdate();
	else if (ev == VE_EV_VREG_RX_REQ)
		veRegProdSfReq(veRegRxId, veRegReqMask);
}

/// Mark a message as being requested.
void veRegProdSfReq(VeRegId regId, un16 mask)
{
	VeRegNr n = 0;
	VeRegId last = VE_REG_INVALID;

	do
	{
		// note: Only the first occurence is marked to allow multiple fields.
		if (veRegProdSfTypedDef[n].regId == last)
			continue;

		// mark the message for sending.
		if ((veRegProdSfTypedDef[n].regId & mask) == regId)
		{
			veRegProdSfFlags[n] |= VE_REG_PROD_SF_REQ;
			if (n2kMsgIn && (n2kMsgIn->flags & VE_CAN_IS_LOOP))
				veRegProdSfFlags[n] |= VE_REG_PROD_SF_LOOPBACK;
			veRegRxHandled = veTrue;
			veTodo();
		}

		last = veRegProdSfTypedDef[n].regId;
	} while (veRegProdSfTypedDef[++n].regId);
}

void veRegProdSfReqVreg(VeRegId regId)
{
	veRegProdSfReq(regId, 0xFFFF);
}

void veRegProdSfModifyFlag(VeRegId regId, un8 flag, veBool value)
{
	VeRegNr n = 0;
	do {
		// locate the proper vreg definition.
		if (veRegProdSfTypedDef[n].regId == regId)
		{
			if (value!=0)
			{
				//set flag(s)
				veRegProdSfFlags[n] |= flag;
			}
			else
			{
				//clear flag(s)
				veRegProdSfFlags[n] &= ~flag;
			}
		}
	} while (veRegProdSfTypedDef[++n].regId);
}

/* Request a vreg by a data value */
static VeRegNr veRegProdReqVregPtr(void* ptr)
{
	VeRegNr n = 0;

	do {
		if (veRegProdSfTypedDef[n].data == ptr) {
			/* loop back to find the first field */
			while (n && veRegProdSfTypedDef[n-1].regId == veRegProdSfTypedDef[n].regId)
				n--;
			return n;
		}
	} while (veRegProdSfTypedDef[++n].regId);
	return VE_REG_NR_INVALID;
}

void veRegPadding(void)
{
	VeBufSize n;

#ifndef CFG_VE_OUT_LE_BITS_DISABLE
	// add padding bits
	if (n2kStreamOut.bitsUsed != 0)
		veOutBits8Le(0x00, 8 - n2kStreamOut.bitsUsed);
#endif

	// add padding bytes for Single Frame
	if (n2kMsgOut->type.p.kind == J1939_SF)
	{
		for (n = n2kStreamOut.index; n < 8; n++)
			veOutUn8(0x00);
	}
}

/** Try to send a messages.
*
* @param nr
*	The index in the definition of the message to send.
*	Note: if the message contains multiple field, the first occurence
*	should be passed.
*	Note: if a callback returns VACK_DO_NOT_SEND_VREG, this function return
*	true to indicate it succesfully did not send the message.
*/
static veBool veRegProdSfSend(VeRegNr nr)
{
	VeRegNr n;
	VeVariant variant;

	// Prepare a message.
	if (!veRegPrepareSf(J1939_NAD_BROADCAST, veRegProdSfTypedDef[nr].regId))
		return veFalse;

	// add data
	n = nr;
	do
	{
		// default types
		variant.type.tp = veRegProdSfTypedDef[n].datatype;
		variant.type.len = veRegProdSfTypedDef[n].size;

		if (veRegProdSfTypedDef[n].flags & V_FUN) {
			VeRegProdValueCallback *callback = (VeRegProdValueCallback *)veRegProdSfTypedDef[n].data;
			if (callback(V_GET, &veRegProdSfTypedDef[n], &variant) == VACK_DO_NOT_SEND_VREG) {
				j1939SfTxFree((J1939Sf *) n2kMsgOut);
				return veTrue;
			}
		} else {
			veVariantFromPtr(&variant, veRegProdSfTypedDef[n].data);
		}
		veOutVariantLe(&variant);

		// support for multiple values in a single message:
		// append field before end of the list and with the same regId.
	} while (veRegProdSfTypedDef[++n].regId && veRegProdSfTypedDef[n].regId == veRegProdSfTypedDef[nr].regId);

	if (veRegProdSfFlags[nr] & VE_REG_PROD_SF_LOOPBACK)
		n2kMsgOut->flags |= VE_CAN_DO_LOOP;

	veRegPadding();
	veOutSendSf();

	return veTrue;
}

void veRegOnChangeNr(VeRegNr n)
{
	if (n == VE_REG_NR_INVALID)
		return;

	/*
	 * If recently this message is already sent because it changed, delay
	 * sending to prevent flooding the bus.
	 */
	if (veRegProdSfFlags[n] & VE_REG_PROD_SF_REQ_ON_CHANGE)
	{
		veRegProdSfFlags[n] |= VE_REG_PROD_SF_SENT_DELAYED;
		return;
	}

	/* mark as requested and remember that the cause is an internal change */
	veRegProdSfFlags[n] |= VE_REG_PROD_SF_REQ_ON_CHANGE | VE_REG_PROD_SF_REQ;
}

void veRegOnChangeId(VeRegId regId)
{
	VeRegNr n = 0;

	do
	{
		if (veRegProdSfTypedDef[n].regId == regId) {
			veRegOnChangeNr(n);
			return;
		}
	} while (veRegProdSfTypedDef[++n].regId);
}

void veRegOnChange(void* tg)
{
	VeRegNr n = veRegProdReqVregPtr(tg);
	veAssert(n != VE_REG_NR_INVALID);

	veRegOnChangeNr(n);
}

veBool veRegSetUn8(un8* tg, un8 value)
{
	if (*tg == value)
		return veFalse;
	*tg = value;
	veRegOnChange(tg);
	return veTrue;
}

veBool veRegSetUn16(un16* tg, un16 value)
{
	if (*tg == value)
		return veFalse;
	*tg = value;
	veRegOnChange(tg);
	return veTrue;
}

veBool veRegSetUn32(un32* tg, un32 value)
{
	if (*tg == value)
		return veFalse;
	*tg = value;
	veRegOnChange(tg);
	return veTrue;
}
