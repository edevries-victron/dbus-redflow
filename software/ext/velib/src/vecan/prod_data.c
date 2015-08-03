#include <velib/velib_inc_J1939.h>
#include <velib/velib_inc_vecan.h>

#include <velib/base/base.h>
#include <velib/canhw/canhw.h>
#include <velib/io/ve_vstream.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939_device.h>
#include <velib/J1939/J1939_tx.h>
#include <velib/nmea2k/n2k_fp.h>
#include <velib/vecan/prod_data.h>
#include <velib/types/variant.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_todo.h>

/**
 * @addtogroup VELIB_VECAN_PROD_DATA
 *	Sending CAN-bus messages.
 *
 * @details
 *	This module send CAN-bus messages from a data definition and optionally
 *	supports incoming request / commands on it. Message can be send periodically
 *	or on request. Messages are marked for sending, so they will be transmitted
 *	even if no resources are available at the time of the request.
 *
 *	Message are constructed out of one or more "signals". Signals are the
 *	variables to add to the messages and are related to a internal variable.
 *	The bus-format can be specified. Signal might need a transform to be send in
 *	the correct units over the bus. The actual transform is left to the
 *  application to allow functions, different original datatypes etc.
 *  The signal definition also include the variables size to make sure the CAN
 *  interface can never corrupt the device its memory.
 *
 *	Since the message definition consists of a constant part, a involatile part
 *	and needs some memory the definition is accessed indirectly by macro's. A
 *	platform can then simply choose the best option to implement this. These
 *	defines are normally in velib_inc_vecan.h.
 *
 *	- @ref VE_PROD_MSG_DEF_CONST
 *	- @ref VE_PROD_MSG_DEF_VOLATILE
 *	- @ref VE_PROD_MSG_DEF_INVOLATILE
 *
 *	Signals on the otherhand are assumed to be in constant array.
 *
 * - When the same Fast Packet is included multiple times, the sequence counters
 *	 must use the same counters. The veProdSequenceCallback should take care of
 *   this.
 *
 * - Several devices are supported but great care must be taken to only call
 *   some functions when the correct device is active, see @device comments.
 *
 *	The ACL can be included to make it requestable by the Complex Request and
 *	the instance fields can be altered by the Complex Write. @ref
 *	CFG_PROD_ACL_INCLUDED must be defined in such a case, since the progress of
 *	ACL messages should be monitored.
 */

#if CFG_PROD_ACL_INCLUDED
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_acl.h>
#endif

static un8 timer;
un8 veProdDataSid;
static un8 rateLimit = 20;
static un8 rateLimitTimer;

/* Init. */
void veProdDataInit(void)
{
	timer = 0;
	veProdDataSid = 0;
}

/**
 * Send requested messages.
 *
 * @device
 *	only acts on the current active device.
 */
void veProdDataUpdate(void)
{
	VeMsgNr n;

	/* Try to send a message if requested */
	for (n = VE_PROD_MSG_FIRST; n <= VE_PROD_MSG_LAST ; n++)
	{
		if (VE_PROD_MSG_DEF_VOLATILE(n).state & MESSAGE_REQUESTED)
		{
			if (veProdDataSendMsgNr(n))
				VE_PROD_MSG_DEF_VOLATILE(n).state &= ~(MESSAGE_REQUESTED | MESSAGE_REQ_EXPLICIT);
		}
	}
}

/**
 * Marks periodic message for sending.
 *
 * @device
 *	none, acts on all devices.
 */
void veProdDataTick(void)
{
	un8 n;

	if (++veProdDataSid >= 252)
		veProdDataSid = 0;

	if (++rateLimitTimer >= rateLimit)
	{
		rateLimitTimer = 0;
		/* Update the timers. */
		for (n=0; n < CFG_PROD_MESSAGE_NUMBER; n++)
		{
			VE_PROD_MSG_DEF_VOLATILE(n).state &= ~MESSAGE_ON_CHANGE;
			if (VE_PROD_MSG_DEF_VOLATILE(n).state & MESSAGE_SENT_DELAYED) {
				VE_PROD_MSG_DEF_VOLATILE(n).state &= ~MESSAGE_SENT_DELAYED;
				VE_PROD_MSG_DEF_VOLATILE(n).state |= MESSAGE_REQUESTED | MESSAGE_ON_CHANGE;
			}
		}
	}

	/* Update the timers. */
	for (n=0; n < CFG_PROD_MESSAGE_NUMBER; n++)
	{
		/* Skip inactive messages */
		if (!VE_PROD_MSG_DEF_VOLATILE(n).timer && !VE_PROD_MSG_DEF_INVOLATILE(n).interval)
			continue;

		/* Mark message to be send */
		if (!VE_PROD_MSG_DEF_VOLATILE(n).timer || !--VE_PROD_MSG_DEF_VOLATILE(n).timer)
		{
			VE_PROD_MSG_DEF_VOLATILE(n).timer = VE_PROD_MSG_DEF_INVOLATILE(n).interval;
			if ((VE_PROD_MSG_DEF_VOLATILE(n).state & MESSAGE_TMR_OFF) == 0)
				VE_PROD_MSG_DEF_VOLATILE(n).state |= MESSAGE_REQUESTED;
		}
	}
}

/**
 * Reset all timers, to make sure data is close to eachother
 *
 * @device
 *	none, acts on all devices.
 */
void veProdDataSync(void)
{
	VeMsgNr msgNr;

	/* Sync all messages. */
	for (msgNr = 0; msgNr < CFG_PROD_MESSAGE_NUMBER; msgNr++)
		VE_PROD_MSG_DEF_VOLATILE(msgNr).timer = 0;
}

/**
 * Adds a signal to the current output message.
 *
 * @device not applicable
 * @internal normally called from @ref veProdDataSendMsgNr.
 */
void veProdDataOutSignal(VeProdSgnDef const * const signal)
{
	VeVariant variant;

	variant.type.tp = signal->type;

	if (variant.type.tp == VE_BUF)
		variant.type.len = signal->size;

	/* Ask application for value of signal which need scaling etc. */
	if (signal->trans)
	{
		veProdScaledValueCallback(signal, &variant);
		veOutVariantLe(&variant);
		return;
	}

	if (veVariantIsBits(signal->type) > 0)
	{
		veVariantFromBitBuffer(&variant, signal->value, signal->size);
		veOutVariantLe(&variant);
		return;
	}

	/* default types */
	veVariantFromPtr(&variant, signal->value);
	veOutVariantLe(&variant);
}

/**
 * Create and queue message for output.
 *
 * @device must be called from the correct context.
 * @internal normally called from @ref veProdDataUpdate.
 */
veBool veProdDataSendMsgNr(VeMsgNr msgNr)
{
	VeProdSgnDef const *signal;
	un16 n;

	/*
	 * Hook for application to update signals or ignore the message
	 * if it contains no valid data.
	 */
	if (veProdSendMsgEvent(msgNr) == veFalse)
		return veTrue;

	veOutMsg(j1939TxAlloc(VE_PROD_MSG_DEF_CONST(msgNr).type));
	if (n2kMsgOut == NULL)
		return veFalse;

	/* Create message */
	signal = &veProdSgnDef[VE_PROD_MSG_DEF_CONST(msgNr).signal];
	for (n = 0; n < VE_PROD_MSG_DEF_CONST(msgNr).number; n++)
	{
		veProdDataOutSignal(signal);
		signal++;
	}

	/* Set correct msgId */
	n2kMsgOut->msgId.p.pg.w = VE_PROD_MSG_DEF_CONST(msgNr).dp;
	n2kMsgOut->msgId.p.pg.p.pri = VE_PROD_MSG_DEF_INVOLATILE(msgNr).priority;
	n2kMsgOut->msgId.p.pf = (un8) (VE_PROD_MSG_DEF_CONST(msgNr).pgn >> 8);
	if (n2kMsgOut->msgId.p.pf <= 0xEF)
		n2kMsgOut->msgId.p.ps = J1939_NAD_BROADCAST;
	else
		n2kMsgOut->msgId.p.ps = (un8) (VE_PROD_MSG_DEF_CONST(msgNr).pgn);

	/* Loopback when requested from the loopback */
	if (VE_PROD_MSG_DEF_VOLATILE(msgNr).state & MESSAGE_LOOPBACK)
		n2kMsgOut->flags |= VE_CAN_DO_LOOP;

	/*
	 * Fast packets need a sequence number per CAN ID, but multiple messages
	 * might use the same -> ask the application for the correct sequence..
	 */
	if (n2kMsgOut->type.p.kind == N2K_FP)
	{
		un8* seq = veProdSequenceCallback(msgNr);

		/* The application is mall configured when returning NULL. */
		veAssert(seq != NULL);
		if (seq == NULL)
			veStreamOut->error = veTrue;	/* force clean if this occurs runtime. */

		veOutSendFp(seq);
	} else {
		/* single frame.. */
		veOutSendSf();
	}

	return veTrue;
}

/**
 * Tries to locate a message by dp / PGN combination, starting after msgNr.
 *
 * @param	dp
 *		The datapage to locate.
 * @param	pgn
 *		The PGN to locate.
 * @param[in,out] msgNr
 *		The last msgNr or -1 to start from the begin.
 *		Set to the number of the msg when found.
 *
 * @return
 *		Whether the message is found
 */
veBool veProdDataIsMsg(un8 dp, J1939Pgn pgn, VeMsgNr* msgNr)
{
	while (++(*msgNr) <= VE_PROD_MSG_LAST)
	{
		if (
				VE_PROD_MSG_DEF_CONST(*msgNr).dp == dp &&
				VE_PROD_MSG_DEF_CONST(*msgNr).pgn == pgn
			)
			return veTrue;
	}

	return veFalse;
}

/**
 * Mark message for sending by index in the message definition.
 *
 * @device not applicable except for the ACL.
 */
void veProdDataMarkMsg(VeMsgNr msgNr)
{
	veTodo();

#if CFG_PROD_ACL_INCLUDED
	if (VE_PROD_MSG_DEF_CONST(msgNr).dp == 0 && VE_PROD_MSG_DEF_CONST(msgNr).pgn == J1939_PGN0_ACL)
	{
		j1939AclUpdateState(J1939_ACL_EV_REQ);
		return;
	}
#endif

	VE_PROD_MSG_DEF_VOLATILE(msgNr).state |= MESSAGE_REQUESTED | MESSAGE_REQ_EXPLICIT;
}

/**
 * Mark message for sending by index in the message definition.
 *
 * @device not applicable.
 */
void veProdDataPeriodic(VeMsgNr msgNr, veBool enable)
{
	if (enable) {
		if (VE_PROD_MSG_DEF_VOLATILE(msgNr).state & MESSAGE_TMR_OFF)
		{
			veProdDataOnChange(msgNr);
			VE_PROD_MSG_DEF_VOLATILE(msgNr).state &= ~MESSAGE_TMR_OFF;
		}
	} else
		VE_PROD_MSG_DEF_VOLATILE(msgNr).state |= MESSAGE_TMR_OFF;
}

/**
 * Mark message for sending by pgn.
 *
 * All message instances will be send if more then one is present for the
 * active device. Incoming J1939 request can for instance use this.
 *
 * @return veTrue if at least one message is queued for sending.
 *
 * @device only acts on the active device.
 */
veBool veProdDataMarkPgn(un8 dp, J1939Pgn pgn)
{
	veBool found = veFalse;
	VeMsgNr msgNr = VE_PROD_MSG_FIRST - 1;

	while (veProdDataIsMsg(dp, pgn, &msgNr))
	{
		found = veTrue;
		veProdDataMarkMsg(msgNr);

		/* If the request came from the loopback answer on it */
		if (n2kMsgIn && (n2kMsgIn->flags & VE_CAN_IS_LOOP))
			VE_PROD_MSG_DEF_VOLATILE(msgNr).state |= MESSAGE_LOOPBACK;
	}
	return found;
}
/**
 * Sent for onchange (with rate limit).
 *
 * @device not applicable.
 */
void veProdDataOnChange(VeMsgNr msgNr)
{
	if (VE_PROD_MSG_DEF_VOLATILE(msgNr).state & MESSAGE_ON_CHANGE) {
		VE_PROD_MSG_DEF_VOLATILE(msgNr).state |= MESSAGE_SENT_DELAYED;
		return;
	}

	VE_PROD_MSG_DEF_VOLATILE(msgNr).state |= MESSAGE_REQUESTED | MESSAGE_ON_CHANGE;
	veTodo();
}
