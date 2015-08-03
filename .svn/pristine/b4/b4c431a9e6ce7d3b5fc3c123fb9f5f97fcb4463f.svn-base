/**
 * The mk2 framehandler.
 *
 * The framehandler checks the basic message format of the message received
 * from the mk2 and can enqueue requests. Resending is done here, to prevent
 * having resend logic everywhere. Besides basic format there is no interpretation
 * of the messages itself, which is left to callback functions. @see mk2FhQueueFrame
 * for more detail.
 *
 * This is also the only module where the serial interrupts / thread context
 * should be taken into account. All the callbacks are executed from the main
 * context and not in the serial context.
 *
 * The hardware / platform specific functions are not implemented here, since this is
 * shared code. See velib/mk2/hal.h for their definition. Windows / posix serial
 * implementation live in src/plt accompanied by function needed for thread safety.
 * Embed version are typically included in the application, since they are quite
 * hardware specific.
 *
 * To use this module:
 *   - call mk2FhInit on init.
 *   - mk2FhTick every 50ms from the main context
 *   - mk2FhUpdate from the mainloop (optional: as long as veTodo is set)
 */

#include <string.h>

#include <velib/velib_inc_mk2.h>

#include <velib/mk2/hal.h>
#include <velib/mk2/frame_handler.h>
#include <velib/platform/plt.h>
#include <velib/types/ve_str.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_todo.h>

static void mk2FhTxHandled(void);

#define MK2_RETRIES 5

#define TX_IDLE -1
#define TX_BUSY -2

/**
 * For tracing / intercepting messages, CFG_MK2_FH_HOOKS can be defined
 * and the mk2FhFrameSendEvent and mk2FhFrameReceivedEvent will be called
 * for any message being send / received. If not defined the compiler just
 * optimizes them out.
 */
#ifndef CFG_MK2_FH_HOOKS
static void mk2FhFrameSendEvent(Mk2MsgTx* mk2msg)
{
	VE_UNUSED(mk2msg);
}

static void mk2FhFrameReceivedEvent(Mk2MsgRx* mk2msg)
{
	VE_UNUSED(mk2msg);
}
#endif

/**
 * Allocate a mk2 message for receiving.
 * @note interrupt safe.
 */
Mk2MsgRx* mk2FhRxAlloc(void)
{
	Mk2MsgRx* ret;

	pltInterruptSuspend();

	if (!mk2.fh.rxFree) {
		// veAssert(veFalse);
		pltInterruptResume();
		return NULL;
	}
	ret = mk2.fh.rxFree;
	mk2.fh.rxFree = mk2.fh.rxFree->next;

	pltInterruptResume();

	ret->next = NULL;
	return ret;
}

/**
 * Free a rx message.
 * Only intended to be run from a main context!
 */
static void mk2FhRxFree(Mk2MsgRx* msg)
{
	veAssert(msg && msg->next == NULL);

	if (!mk2.fh.rxFree) {
		mk2.fh.rxFree = msg;
		return;
	}
	msg->next = mk2.fh.rxFree;
	mk2.fh.rxFree = msg;
}

/**
 * The actual processing of the received message.
 * @note This is done in the main context.
 */
static void mk2RxFrame(Mk2MsgRx* frm)
{
	/*
	* If the MSB of the length is set, then the ledstatus has been
	* appended to the message and the length increased by 2.
	*/
	if (frm->length & 0x80) {
		frm->length &= ~0x80;
		frm->length -= 2;
	}

	mk2FhFrameReceivedEvent(frm);

	if (mk2.fh.txPos == TX_BUSY && mk2.fh.msgOut->handler)
		if (mk2.fh.msgOut->handler(MK2_FRAME, frm) == MK2_DONE) {
			Mk2Callback* callback;

			/* assert the handler did not call mk2FhReset */
			veAssert(mk2.fh.msgOut);
			callback = mk2.fh.msgOut->callback;
			mk2FhTxHandled();

			if (callback)
				callback();
		}
}

#ifdef CFG_MK2_LOG_INVALID_FRAMES
static void dumpRxBuf(char const *reason)
{
	VeStr s;
	un8 n;

	veStrNew(&s, 100, 100);
	veStrAdd(&s, reason);
	veStrAddFormat(&s, " %02X ", mk2.fh.rxMsg->length);
	for (n = 0; n < mk2.fh.bytesReceived; n++)
		veStrAddFormat(&s, "%02X ", mk2.fh.rxMsg->buf[n]);
	logW("mk2fh", veStrCStr(&s));
	veStrFree(&s);
}
#else
static void dumpRxBuf(char const *reason)
{
	logE("mk2fh", reason);
}
#endif

/**
 * Called whenever a character is received.
 *
 * Frame format is <Length> <Data[0]> ... <Data[Length - 1]> <Checksum>
 *
 * @param inbyte The received byte
 */
void mk2RxByte(un8 inbyte)
{
	pltInterruptBegins(1);

	if (!mk2.fh.rxMsg || mk2.fh.rxMsg->length == 0) {

		if (!mk2.fh.rxMsg) {
			if (! (mk2.fh.rxMsg = mk2FhRxAlloc()) ) {
				logW("mk2", "lost incoming byte %02X", inbyte);
				mk2.fh.stats.rxLost++;
				goto out;
			}
		}

		logI("fh", "new frame");
		mk2.fh.checksum = inbyte;
		mk2.fh.bytesReceived = 0;
		mk2.fh.rxTimeout = 0;

		/*
		 * Start of frame, this should be the length field. Frames with LED
		 * status appended have bit 7 set in the length field.
		 */
		mk2.fh.rxMsg->length = inbyte;
		if ((mk2.fh.rxMsg->length & ~0x80) == 0 || (mk2.fh.rxMsg->length & ~0x80) > MK2_MAX_MESSAGE_LENGTH)
			mk2.fh.rxMsg->length = 0;
	} else {
		mk2.fh.rxMsg->buf[mk2.fh.bytesReceived++] = inbyte;
		mk2.fh.checksum += inbyte;
		if (mk2.fh.bytesReceived > (mk2.fh.rxMsg->length & ~0x80))
		{
			if (!mk2.fh.checksum) {
				/*
				 * The calculated checksum is correct, so this is a valid msg.
				 *
				 * Add it to the list for processing in the main context..
				 * Since the mk2 protocol is mainly a question / answer protocol, the linked list
				 * is generaly 1 message long. Version frames are sent async though and might
				 * cause the answer to be missed if there was no space to store more then one
				 * message.
				 */
				mk2FhRawFrameReceivedEvent(mk2.fh.rxMsg);
				if (!mk2.fh.rxMsgMainLoop) {
					mk2.fh.rxMsgMainLoop = mk2.fh.rxMsg;
				} else {
					Mk2MsgRx* tail = mk2.fh.rxMsgMainLoop;
					while (tail->next)
						tail = tail->next;
					tail->next = mk2.fh.rxMsg;
				}
				mk2.fh.stats.rxCounter++;
				mk2.fh.rxMsg = NULL;
				logI("mk2fh", "rx msg queued");
				veTodo();

			} else {
				dumpRxBuf("invalid checksum");
				mk2.fh.stats.rxCrc++;
				mk2.fh.rxMsg->length = 0;
			}
		}
	}

out:
	pltInterruptEnds(1);
}

/** allocate a mk2 message for sending */
Mk2MsgTx* mk2TxAlloc(void)
{
	Mk2MsgTx* ret;

	pltInterruptSuspend();

	if (!mk2.fh.txFree) {
		veAssert(veFalse);
		pltInterruptResume();
		return NULL;
	}
	ret = mk2.fh.txFree;
	mk2.fh.txFree = mk2.fh.txFree->next;

	pltInterruptResume();

	ret->handler = NULL;
	ret->callback = NULL;
	ret->next = NULL;
	ret->addChecksum = veTrue;
	ret->retries = MK2_RETRIES;
	ret->noDelay = veFalse;

	return ret;
}

/**
 * Free a tx message.
 * @note this might run from a tx interrupt. It needs to be called
 *       from a interrupt / thread safe when called from the main context.
 */
static void mk2FhTxFree(Mk2MsgTx* msg)
{
	veAssert(msg && msg->next == NULL);

	if (!mk2.fh.txFree) {
		mk2.fh.txFree = msg;
		return;
	}
	msg->next = mk2.fh.txFree;
	mk2.fh.txFree = msg;
}

/**
 * Start sending the current tx message. Depending on the CFG_MK2_TX_BYTES
 * either mk2TxByte or mk2TxBuf is called in the hardware abstration. Both
 * should call mk2FhTxDone when done.
 */
static void startTransmission(void)
{
	veAssert(mk2.fh.msgOut);

	mk2.fh.txPos = 0;
	mk2.fh.msgOut->checksum = mk2.fh.msgOut->length;

#if CFG_MK2_TX_BYTES
	/*
	 * Embedded chip might have a single tx byte, after sending
	 * it mk2FhTxDone should be called to obtain the next one.
	 */
	mk2TxByte(mk2.fh.msgOut->length);
#else
	/*
	 * One systems with an OS your typically better of handing the
	 * whole buffer over. Like above mk2FhTxDone should be called after
	 * the buffer is send / queued.
	 */
	if (mk2.fh.msgOut->addChecksum) {
		while (mk2.fh.txPos < mk2.fh.msgOut->length) {
			mk2.fh.msgOut->checksum += mk2.fh.msgOut->buf[mk2.fh.txPos];
			mk2.fh.txPos++;
		}
		mk2.fh.msgOut->buf[mk2.fh.txPos++] = -mk2.fh.msgOut->checksum;
	} else {
		mk2.fh.txPos = mk2.fh.msgOut->length + 2;
	}
	mk2.fh.msgOut->buf[mk2.fh.txPos++] = -mk2.fh.msgOut->checksum;
	mk2TxBuf(&mk2.fh.msgOut->length, mk2.fh.msgOut->length + 2);
#endif
}

/**
 * Reset state after a tx message is handled.
 * @note this might run from a tx interrupt.
 */
static void mk2FhTxHandled(void)
{
	Mk2MsgTx* msg;

	pltInterruptSuspend();

	mk2.fh.rspTimeout = 0;
	msg = mk2.fh.msgOut;
	mk2.fh.msgOut = mk2.fh.msgOut->next;
	msg->next = NULL;
	mk2FhTxFree(msg);
	mk2.fh.txPos = TX_IDLE;

	if (mk2.fh.msgOut && mk2.fh.msgOut->noDelay)
		startTransmission();

	pltInterruptResume();
}

/**
 * Called by the hal after the byte / message had been sent.
 * @note this might run from a tx interrupt.
 */
void mk2FhTxDone(void)
{
	pltInterruptBegins(1);

	if (!mk2.fh.msgOut)
		goto out;

#if CFG_MK2_TX_BYTES
	/* Append checksum to the message */
	if (mk2.fh.txPos == mk2.fh.msgOut->length) {
		mk2.fh.txPos++;
		mk2.fh.msgOut->checksum = -mk2.fh.msgOut->checksum;
		mk2TxByte(mk2.fh.msgOut->checksum );
		goto out;
	}
#endif

	/* Done.. */
	if (mk2.fh.txPos > mk2.fh.msgOut->length)
	{
		mk2.fh.stats.txCounter++;
		mk2FhFrameSendEvent(mk2.fh.msgOut);
		if (!mk2.fh.msgOut->handler) {
			mk2FhTxHandled();
		} else {
			mk2.fh.rspTimeout = 20;
			mk2.fh.txPos = TX_BUSY;
			/* Support single shot */
			if (mk2.fh.msgOut->handler(MK2_TX_SENT, NULL) == MK2_DONE) {
				Mk2Callback *callback = mk2.fh.msgOut->callback;
				mk2FhTxHandled();
				if (callback)
					callback();
			}
		}
		goto out;
	}

#if CFG_MK2_TX_BYTES
	{
		/* sent the next byte */
		un8 chr = mk2.fh.msgOut->buf[mk2.fh.txPos];
		mk2.fh.msgOut->checksum += chr;
		mk2.fh.txPos++;
		mk2TxByte(chr);
	}
#endif

out:
	pltInterruptEnds(1);
}

/**
 * Enqueue a frame for transmission.
 *
 * @param frm
 *  The message to send, this object also contains the function pointers
 *  of the method to handle the response. Typically the handler converts
 *  the message to the object in ctx. When succesfull, the callback is
 *  invoked, which is typically part of the application.
 *
 *  Resending is done automatically if no valid response was received in
 *  time (except if the TIMEOUT event returns MK2_DONE, which indicates
 *  that the timeout was expected). If there is no valid response after
 *  several attempts, the mk2NoResponseCallback() is called which should
 *  reset the whole communication / values, since most likely the device
 *  is off / removed and discovery must be restarted etc.
 *
 *  All callbacks are in the main context.
 */
void mk2FhQueueFrame(Mk2MsgTx* frm)
{
	pltInterruptSuspend();

	veAssert(frm && !frm->next);

	/* Transmission is busy, append to queue. */
	if (mk2.fh.msgOut != NULL)
	{
		Mk2MsgTx* tail;

		tail = mk2.fh.msgOut;
		while (tail->next) {
			veAssert(tail != frm);
			tail = tail->next;
		}
		tail->next = frm;
	} else {
		mk2.fh.msgOut = frm;
		if (frm->noDelay)
			startTransmission();
	}

	pltInterruptResume();
}

/**
 * Update time specific actions for the frame handler.
 * Check for timeouts and continue with the next tx message when idle.
 *
 * Should be called every 50ms from the main context.
 */
void mk2FhTick(void)
{
	pltInterruptSuspend();

	if (mk2.fh.txPos != TX_IDLE && mk2.fh.rspTimeout && --mk2.fh.rspTimeout == 0) {
		if (mk2.fh.msgOut->retries)
			mk2.fh.msgOut->retries--;

		logW("mk2_fh", "mk2 no reply");

		/* just continue if the timeout is intended */
		if (mk2.fh.msgOut->handler && mk2.fh.msgOut->handler(MK2_TIMEOUT, NULL) == MK2_DONE) {
			Mk2Callback *callback = mk2.fh.msgOut->callback;

			mk2FhTxHandled();

			if (callback)
				callback();
		} else {
			if (mk2.fh.msgOut->retries != 0) {
				startTransmission();
			} else {
				mk2FhTxHandled();
				mk2NoResponseCallback();
			}
		}
	}

	/* transmission is started here, to provide rate limiting */
	if (mk2.fh.txPos == TX_IDLE && mk2.fh.msgOut)
		startTransmission();

	/*
	 * If expired and not being processed or already idle, reset the frame
	 * handler by force.
	 */
	if (mk2.fh.rxMsg && mk2.fh.rxMsg->length && ++mk2.fh.rxTimeout > 6)
	{
		dumpRxBuf("rx timeout");
		mk2.fh.rxMsg->length = 0;
		mk2.fh.stats.rxTimeout++;
	};

	pltInterruptResume();
}


/** Process incoming message from the main context. */
void mk2FhUpdate(void)
{
	pltInterruptSuspend();

	while (mk2.fh.rxMsgMainLoop) {
		Mk2MsgRx* msg = mk2.fh.rxMsgMainLoop;
		mk2RxFrame(msg);

		/* Check that the framehandler is not reset from with the callback */
		veAssert(msg == mk2.fh.rxMsgMainLoop);

		mk2.fh.rxMsgMainLoop = mk2.fh.rxMsgMainLoop->next;
		msg->next = NULL;
		mk2FhRxFree(msg);
	}

	pltInterruptResume();
}

/**
 * Setup the framehandler
 *
 * This will setup CFG_MK2_TX_FRAMES messages for sending and
 * CFG_MK2_RX_FRAMES for receiving. note: used for reset as well,
 * but the complete mk2 struct is already reset to zero in that case.
 */
void mk2FhInit(void)
{
	un8 n;

	pltInterruptAssertDisabled(1);

	mk2.fh.txFree = &mk2.fh.txMsgs[0];
	for (n = 1; n < CFG_MK2_TX_FRAMES; n++) {
		mk2.fh.txMsgs[n].next = mk2.fh.txFree;
		mk2.fh.txFree = &mk2.fh.txMsgs[n];
	}

	mk2.fh.rxFree = &mk2.fh.rxMsgs[0];
	for (n = 1; n < CFG_MK2_RX_FRAMES; n++) {
		mk2.fh.rxMsgs[n].next = mk2.fh.rxFree;
		mk2.fh.rxFree = &mk2.fh.rxMsgs[n];
	}

	mk2.fh.txPos = TX_IDLE;
}

/**
 * Reset the framehandler to default.
 *
 * DO NOT call this from a msg callback itself... Only
 * mk2NoResponseCallback.
 */
void mk2FhReset(void)
{
	pltInterruptSuspend();

	memset(&mk2.fh, 0, sizeof(mk2.fh));
	mk2FhInit();

	pltInterruptResume();
}
