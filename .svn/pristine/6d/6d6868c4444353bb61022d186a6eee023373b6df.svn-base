/**
 * These functions contain the messages intended for the mk2 (the inteface product)
 * itself and not for the Ve.Bus system behind it. A shadow copy of its settings of
 * the mk2 (and the framehandler associated with this mk2) are kept in the global
 * struct mk2. Typically that struct is adjusted and sent back to the mk2 with the
 * appriopriaty command, to alter settings. See the document "Interfacing with
 * Ve.Bus products" for details about the flags / settings.
 *
 * One of the things done here as well, is to keep track of the vebus device the mk2
 * is talking to. The mk2 message format is not addresable by itself, but relies on
 * a variable in the mk2 to forward a message to a specific device. To prevent the
 * application to have to keep track of that, it is done here. Address specific
 * message can call @see mk2TalkToAddress to make sure the intended device is addressed;
 * a address change message will only be inserted if needed.
 */

#include <ctype.h>

#include <velib/velib_inc_mk2.h>
#include <velib/base/endian.h>
#include <velib/mk2/mk2.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>

/** (re)set the mk2 related variables */
void mk2Init(void)
{
	mk2.intendedAddress = 'W';
	mk2.actualAddress = 'W';
	mk2.cmd = 'X';
	mk2.state.sFlagsExt0 = MK2_S_FLAG_E0_FORWARD_ALL_CONFIG_RESP |
							MK2_S_FLAG_E0_COMPAT_SPLIT_FRAMES;
}

/** Create a message addressed to the MK2 itself */
Mk2MsgTx* mk2Msg(un8 command)
{
	Mk2MsgTx* msg = mk2TxAlloc();
	if (!msg)
		return NULL;
	msg->buf[0] = 0xFF; /* mk2 cmd */
	msg->buf[1] = command;
	msg->length = 2;
	return msg;
}

/* Requests the software version of the MK2 */
void mmhSendMk2Cmd(un8 command)
{
	Mk2MsgTx* msg = mk2Msg(command);
	if (!msg)
		return;
	mk2FhQueueFrame(msg);
}

/** Handle address reply */
static Mk2EventReply setAddressHandler(Mk2Event mk2event, Mk2MsgRx* msg)
{
	if (mk2event != MK2_FRAME || msg->buf[0] != 0xFF ||
			msg->buf[1] != 'A' || msg->length < 4)
		return MK2_CONTINUE;

	mk2.actualAddress = msg->buf[3];
	return MK2_DONE;
}

/** Sets the VE Bus address in the MK2. */
void mk2SetAddress(un8 address, Mk2Callback* callback)
{
	Mk2MsgTx* msg = mk2Msg('A');
	if (!msg)
		return;
	msg->buf[2] = 1;
	msg->buf[3] = address;
	msg->length = 4;
	msg->handler = setAddressHandler;
	msg->callback = callback;
	logI("mk2", "---- setting address: %02X ----", address);
	mk2FhQueueFrame(msg);
}

/** Sets the VE Bus address in the MK2 if it changed. */
void mk2TalkToAddress(un8 address)
{
	if (address == mk2.intendedAddress)
		return;

	/*
	 * note: the (last) intendedAddress is being tracked since there
	 * might be several address changes in the tx queue.
	 */
	mk2.intendedAddress = address;
	mk2SetAddress(address, NULL);
}

/**
 * Waits for confirmation of changing settings 'S'.
 *
 * reply: 0xFF 'S'
 */
static Mk2EventReply setMk2StateHandler(Mk2Event mk2event, Mk2MsgRx* msg)
{
	if (mk2event == MK2_FRAME && msg->buf[0] == 0xFF && msg->buf[1] == 'S') {
		logI("mk2", "---- mk2 state set done ----");
		return MK2_DONE;
	}

	return MK2_CONTINUE;
}

/** Create mk2 set state message */
Mk2MsgTx* mk2MsgSetState(Mk2State* state)
{
	Mk2MsgTx* msg = mk2Msg('S');
	if (!msg)
		return NULL;

	if (state->sFlags & MK2_S_FLAG_DO_NOT_SEND_STATE)
		logI("mk2", "----  set mk2 state ---- (no state, %d)", state->limit.current.value);
	else
		logI("mk2", "----  set mk2 state ---- (%d, %d)", state->mode, state->limit.current.value);
	msg->buf[2] = state->mode;
	cpu_to_le_u16_tp(&msg->buf[3], state->limit.current.value);
	msg->buf[5] = 1;
	msg->buf[6] = state->sFlags | MK2_S_FLAG_NEW_FORMAT;
	msg->buf[7] = 0;
	msg->buf[8] = state->sFlagsExt0;
	msg->length = 9;
	msg->handler = setMk2StateHandler;
	return msg;
}

/** Set the mk2 state */
void mk2SetState(Mk2State* state, Mk2Callback* callback)
{
	Mk2MsgTx* msg;
	msg = mk2MsgSetState(state);
	if (!msg)
		return;
	msg->callback = callback;
	mk2FhQueueFrame(msg);
}

/* Alloc winmon frame */
Mk2MsgTx *mk2MsgXYZ(un8 addr, un8 wcmd)
{
	Mk2MsgTx *msg;

	/* Make sure to talk to the correct device */
	mk2TalkToAddress(addr);

	msg = mk2Msg(mk2.cmd);
	if (!msg)
		return NULL;

	if (++mk2.cmd > 'Z')
		mk2.cmd = 'X';
	msg->buf[2] = wcmd;
	msg->length = 3;

	return msg;
}

/* Alloc winmon frame with un16 payload */
Mk2MsgTx *mk2MsgXYZInfo(un8 addr, un8 wcmd, un16 info)
{
	Mk2MsgTx *msg = mk2MsgXYZ(addr, wcmd);

	if (msg == NULL)
		return NULL;

	cpu_to_le_u16_tp(&msg->buf[3], info);
	msg->length = 5;

	return msg;
}

/*
 * Check if the response is a XYZ response, the commands and responses loop
 * through XYZ and should match.
 */
veBool mk2IsXYZMinLength(Mk2MsgRx* frm, un8 rsp, un8 minLength)
{
	veAssert(mk2.fh.msgOut);

	return (frm->length >= minLength) && frm->buf[0] == 0xFF &&
		frm->buf[1] == mk2.fh.msgOut->buf[1] && frm->buf[2] == rsp;
}


/// @see mk2IsXYZMinLength. This checks there is at least one data field.
veBool mk2IsXYZ(Mk2MsgRx* frm, un8 rsp)
{
	return mk2IsXYZMinLength(frm, rsp, 5);
}

/// @see mk2IsXYZMinLength. Tolerate response without any data.
/// This used to be an invalid case, but the v2xx versions do send
/// these kind of replies.
veBool mk2IsXYZNoData(Mk2MsgRx* frm, un8 rsp)
{
	return mk2IsXYZMinLength(frm, rsp, 3);
}

/** Waits for answer to L request */
static Mk2EventReply mk2VebusStateHandler(Mk2Event mk2event, Mk2MsgRx *msg)
{
	VebusState* info = mk2.fh.msgOut->ctx;

	if (mk2event == MK2_FRAME && msg->buf[0] == 0xFF && msg->buf[1] == 'L' && msg->length >= 6 )
	{
		info->leds = msg->buf[2];
		info->ledsBlink = msg->buf[3];
		info->alarms = msg->buf[4];
		/* masked since 25-09-2014. High bits might get defined */
		info->vebusError = msg->buf[5] & 0x1F;
		/* copy the flags in the vebus error but to a different member for clarity */
		info->flags = msg->buf[5] & 0xE0;

		logI("mk2", "++++ got leds / vebus error/config ++++");
		return MK2_DONE;
	}

	return MK2_CONTINUE;
}

/// Request LED status.
void mk2GetVebusState(VebusState* info, Mk2Callback* callback)
{
	Mk2MsgTx* msg = mk2Msg('L');
	if (!msg)
		return;

	logI("mk2", "---- get leds / vebus error/config ----");
	msg->ctx = info;
	msg->callback = callback;
	msg->handler = mk2VebusStateHandler;
	mk2FhQueueFrame(msg);
}

/**
 * reply: 'D', Switch state, Pot meter value, Panel scale, xIDs, Flags,SRF ID
 */
static Mk2EventReply mk2GetStateHandler(Mk2Event mk2event, Mk2MsgRx* msg)
{
	Mk2State* state = mk2.fh.msgOut->ctx;

	if (mk2event == MK2_FRAME && msg->buf[0] == 0xFF &&
		msg->buf[1] == 'D' && msg->length >= 7)
	{
		/*
		 * These are the settings in the MK2 itself, which are sent to it from here
		 * or the defaults, so just ignore it...
		 */
		state->mode = msg->buf[2];
		state->limit.old.potMeter = msg->buf[3];
		state->limit.old.panelScale = msg->buf[4];
		state->xids = msg->buf[5];
		state->dFlags = msg->buf[6];

		logI("mk2", "++++ got MK2 settings ++++");

		return MK2_DONE;
	}

	return MK2_CONTINUE;
}

/** Get the MK2 settings state (D message). */
void mk2GetState(Mk2State* state, Mk2Callback* callback)
{
	Mk2MsgTx* msg = mk2Msg('D');
	if (!msg)
		return;

	logI("mk2", "---- get MK2 settings ----");
	msg->ctx = state;
	msg->callback = callback;
	msg->handler = mk2GetStateHandler;
	mk2FhQueueFrame(msg);
}

/**
 * parses an version reply. Also send when no communication is going on.
 * < 07 FF 'V 94 3E 11 00 00 [C1]
 */
static Mk2EventReply mk2GetVersionHandler(Mk2Event mk2event, Mk2MsgRx *msg)
{
	un32* version = mk2.fh.msgOut->ctx;

	if (mk2event == MK2_FRAME && msg->buf[0] == 0xFF &&
			msg->buf[1] == 'V' && msg->length >= 7)
	{
		/*
		 * These are the settings in the MK2 itself, which are sent to it from here
		 * or the defaults, so just ignore it...
		 */
		*version = le_u32_to_cpu_p(&msg->buf[2]);
		if (msg->buf[6] != mk2.actualAddress) {
			logI("mk2", "reset?");
			mk2.actualAddress = msg->buf[6];
		}

		return MK2_DONE;
	}

	return MK2_CONTINUE;
}

/** Get the MK2 settings state (D message). */
void mk2GetVersion(un32 *version, Mk2Callback* callback)
{
	Mk2MsgTx* msg = mk2Msg('V');
	if (!msg)
		return;

	logI("mk2", "---- get mk2 version ----");
	msg->ctx = version;
	msg->callback = callback;
	msg->handler = mk2GetVersionHandler;
	mk2FhQueueFrame(msg);
}

/*
 * Send a reset command to initiate a reset of the mk2 itself.
 * Since its addresses and flags will reset to defaults an application
 * has to make the proper init sequence is used again.
 */
void mk2SoftReset(Mk2Callback* callback)
{
	Mk2MsgTx* msg = mk2Msg('R');
	if (!msg)
		return;

	logI("mk2", "---- reset mk2 ----");
	msg->callback = callback;
	msg->handler = mk2GetVersionHandler;
	mk2FhQueueFrame(msg);
}
