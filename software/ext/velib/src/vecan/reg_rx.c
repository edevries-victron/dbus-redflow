/**
 * @addtogroup VELIB_VECAN_REG_RX
 *
 * Victron function registers allow an application to hook into any data
 * received by victron registers, similiar to VE.Net external command register.
 * All function pointers register in @ref veRegRxHndlrDef will receive the
 * following events:
 *
 * - VE_REG_REQ
 *		On an incoming request. @ref VeRegId will contain the requested
 *		register. @ref veRegReqMask the mask to allow querying multiple
 *		registers at once.
 * - VE_REG_ACK
 *		On an incoming acknowledge. @ref VeRegId will contain the acked
 *		register. @ref veRegAckCode the ack code.
 * - VE_EV_VREG_RX
 *		When not a special command. Only @ref VeRegId is set.
 *
 * Handlers should set veRegRxHandled to veTrue on valid data. When not set
 * a NACK is returned to the sender.
 */

#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/base/base.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939.h>
#include <velib/nmea2k/n2k.h>
#include <velib/vecan/vecan.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/reg_rx.h>

// Properties of incoming vreg msg.
VeRegId 				veRegRxId;
VeRegId					veRegReqMask;
veBool					veRegRxHandled;
VeRegAckCode			veRegAckCode;

static veBool errorOverflow;

/// Init.
void veRegRxInit(void)
{
	errorOverflow = veFalse;
}

/**
 * Prepare a message for VE.Reg format. When succesfull message is available
 * as current output message, n2kMsgOut.
 *
 * Note: if noRetry is set and a no message could be allocated, a global error is sent
 * instead and it is up to the remote site to handle that. Don't set noRetry if the
 * caller will retry sending later!
 */
veBool veRegPrepareSfRetryArg(J1939Nad tg, VeRegId regId, veBool noRetry)
{
	veRegRxHandled = veTrue;
	if (!veOutAllocSf()) {
		if (noRetry && tg != J1939_NAD_BROADCAST)
			errorOverflow = veTrue;
		return veFalse;
	}

	n2kMsgOut->msgId.w = J1939ID_PRI(7) | J1939ID_PGN(PGN0_VECAN_VREG | tg);
	veOutUn16Le(N2K_MAN_TAG_VICTRON);
	veOutUn16Le(regId);

	return veTrue;
}

/// Queues a VACK for output.
veBool veRegSendAckRetryArg(J1939Nad tg, VeRegId regId, VeRegAckCode code, veBool noRetry)
{
	if (!veRegPrepareSfRetryArg(tg, VE_REG_ACK, noRetry))
		return veFalse;

	veOutUn16Le(regId);
	veOutUn16Le(code);
	veOutSendSf();

	return veTrue;
}

/**
 * Process incoming VE register messages.
 *
 * Assumes:
 * 	Incoming message is a proprietary VE reg;
 *	Manufacturer code already checked and position located after the first 2 bytes.
 */
void veRegRxData(void)
{
	VeBufSize pos;
	VeEventHndlr const*	veRegRxHndl;
	VeEvent ev = VE_EV_VREG_RX;

	// Get (valid) register id from message
	veRegRxId = veInUn16Le();

	veRegRxHndl = veRegRxHndlrDef;
	veRegRxHandled = veFalse;

	// Process request for registers.
	if (veRegRxId == VE_REG_REQ)
	{
		ev = VE_EV_VREG_RX_REQ;
		veRegRxId = veInUn16Le();
		veRegReqMask = veInUn16Le();

	} else if (veRegRxId == VE_REG_ACK)
	{
		// get (valid) register id from message
		ev = VE_EV_VREG_RX_ACK;
		veRegRxId = veInUn16Le();
		veRegAckCode = veInUn16Le();
		veRegRxHandled = veTrue;		// never nack and ack

	} else if (n2kMsgIn->msgId.p.ps != J1939_NAD_BROADCAST)
	{
		// Commands are addressed, responses broadcasted.
		ev = VE_EV_VREG_RX_CMD;
	}

	if (veStreamIn->error)
		return;

	// Process messages.
	pos = veStreamIn->index;
	while (*veRegRxHndl != NULL)
	{
		(*veRegRxHndl)(ev);
		veRegRxHndl++;
		veInMove(pos);
	}

	// Send an error back if the register is unknown.
	if (n2kMsgIn->msgId.p.ps != J1939_NAD_BROADCAST && !veRegRxHandled)
	{
		VeRegAckCode code;
		code = (ev == VE_EV_VREG_RX_REQ ? VACK_ERR_REQ : VACK_ERR_CMD);
		(void)veRegSendAck(n2kMsgIn->msgId.p.src, veRegRxId, code);
	}
}

/// Send fatal error on overflow.
void veRegRxUpdate(void)
{
	// If there are too many invalid request, it might be impossible to NACK
	// them all, hence an overflow message is send. Senders might use it for
	// taken the appriopriate action. But more important it is quite useful
	// for tracking down problems since it will be in a dump of the messages.
	if (errorOverflow && veRegSendAck(J1939_NAD_BROADCAST, VE_REG_ACK, VACK_ERR_OVERFLOW))
		errorOverflow = veFalse;
}

#if CFG_WITH_J1939_NMT
veBool veRegPrepareSfWhenOnBus(struct J1939Func* tg, VeRegId regId)
{
	if (!j1939NmtIsOnBus(tg))
		return veFalse;

	return veRegPrepareSfRetryArg(tg->nmt.nad, regId, veFalse);
}
#endif
