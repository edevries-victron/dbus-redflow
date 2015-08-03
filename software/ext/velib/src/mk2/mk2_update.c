/**
 * This will update the firmware in the mk2 itself. Since the update procedure
 * is related to the VE.Flash user interface, instructing the user to reset the
 * multi etc, this code is not that useful for pc application. In general it is
 * easier to just run the VE.Flash program itself to update the mk2.
 *
 * On embedded targets some additional io lines are used to get the mk2 in/out of
 * bootloader mode. Additional hardware functions needed to update are:
 *
 *	mk2SetBaudRate(MK2_BAUD_2400);
 *	mk2SetBaudRate(MK2_BAUD_19200);
 *	mk2SetBaudRate(MK2_BAUD_115200);
 *	mk2Break();
 *  mk2BreakClear();
 *	mk2HoldInReset(veTrue); // mk2 hold reset...
 *	mk2EnableUpdate(veTrue); // by-pass the normal frame handler
 *
 *	note that while mk2EnableUpdate == veTrue the incoming bytes should be passed to
 *		mk2RxUpdateRxByte
 *
 *	And 50 ms ticks must from main must call:
 *		mk2FirmwareTick
 *
 *	If this is done, a call to mk2FirmwareUpgrade(callback) will always result in
 *	the callback being invoke with true / false as argument to indicate success.
 *	The mk2 needs to be power cycled again, but since this is typically part of the
 *	startup, it is not done here.
 */

#include <velib/mk2/mk2.h>
#include <velib/mk2/hal.h>
#include <velib/mk2/mk2_update.h>
#include <velib/utils/ve_todo.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_timer.h>

#include "mk2_firmware.inc.135.c"

// The states used during the update procedure
typedef enum
{
	UPDATE_IDLE,
	UPDATE_MK2_OFF_BREAK,
	UPDATE_MK2_STARTING,
	UPDATE_MK2_RELEASE_BREAK,
	UPDATE_MK2_ESCAPING,
	UPDATE_AWAITING_VERSION,
	UPDATE_SENDING_FIRMWARE,
	UPDATE_DONE
} UpdateState;

typedef enum
{
	UPDATE_BEGIN,
	UPDATE_TIMER,
	UPDATE_REV_VERSION,
	UPDATE_REV_NEXT_DATA,
	UPDATE_REPLY
} UpdateEvent;

typedef enum
{
	UPDATE_ERR_OK,
	UPDATE_ERR_VERSION_TIMEOUT,
	UPDATE_ERR_VERSION_INVALID,
	UPDATE_ERR_PROG_RESPONSE,
	UPDATE_ERR_PROG_TIMEOUT
} UpdateError;

static volatile un8 buffer[5];
static volatile un8 pos = 0;
static volatile un8 bytesExpected = 0;
static volatile veBool dataReady = 0;

static UpdateError updateError;			// Last error
static UpdateState updateState;			// The current state of the update
static un16 updateTimeout = 0;			// in 50 ms, 0 is idle
static un16 bytesSent = 0;				// position in firmware image
static Mk2ResultCallback* resultCallback;

un8 const leaveTransMode[] = {0x55, 0x55, 0x55, 0x55, 0x55};

/* all exit paths should go through here */
static void updateResult(UpdateError error)
{
	updateError = error;

	updateTimeout = 0;	// no more timer events
	bytesExpected = 0;	// no data events..

	logI("update", (error == UPDATE_ERR_OK ? "ok" :  "error" ));
	updateState = UPDATE_IDLE;

	/* restore normal mk2 serial settings */
	mk2SetBaudRate(MK2_BAUD_2400);
	mk2EnableUpdate(veFalse);

	if (resultCallback) {
		resultCallback(error);
		resultCallback = NULL;
	}
}

#define UPDATE_FRAME_SIZE	76
/**
 * Send next firmware update frame.
 * @returns veTrue when done (no additional frame sent)
 */
static veBool sendFirmware(void)
{
	if (bytesSent >= MK2_FIRMWARE_SIZE)
		return veTrue;

	mk2TxBuf(&mk2Firmware[bytesSent], UPDATE_FRAME_SIZE);
	bytesSent += UPDATE_FRAME_SIZE;
	return veFalse;
}

/* The update logic itself */
static void mk2FirmwareUpdateState(UpdateEvent ev)
{
	switch(updateState)
	{
	case UPDATE_DONE:
		break;

	case UPDATE_IDLE:
		if (ev == UPDATE_BEGIN)
		{
			updateError = UPDATE_ERR_OK;
			pos = 0;
			bytesExpected = 0;

			logI("mk2_upd", "sending break and resetting mk2");
			updateState = UPDATE_MK2_OFF_BREAK;
			mk2SetBaudRate(MK2_BAUD_115200);
			mk2Break();
			mk2HoldInReset(veTrue); // mk2 hold reset...
			mk2EnableUpdate(veTrue); /* by-pass the normal frame handler */
			updateTimeout = 2 + 1;
		} else {
			veAssert(veFalse);
		}
		break;

	case UPDATE_MK2_OFF_BREAK:
		if (ev == UPDATE_TIMER)
		{
			// mk2 is now in reset
			logI("mk2_upd", "starting mk2");
			updateState = UPDATE_MK2_STARTING;
			mk2HoldInReset(veFalse); // release from reset...

#ifdef CFG_HAVE_VE_WAIT
			/*
			 * mmm, if the breaks is held for more then 10-20ms, a vebus system
			 * will temporarily shutdown. So delay if that is possible.
			 * Note: blocking for such a long time is bad!
			 */
			veWait(10000); // note: promised to wait for _at least_ 10ms
			// NOTE: Fall through
#else
			updateTimeout = 2 + 1;
			break;
#endif
		} else {
			veAssert(veFalse);
			break;
		}
		// NOTE: fall through from above ifdef!!

	case UPDATE_MK2_STARTING:
		if (ev == UPDATE_TIMER)
		{
			// Assuming the mk2 has seen the break, stop breaking...
			updateState = UPDATE_MK2_RELEASE_BREAK;
			logI("mk2_upd", "stop breaking");
			mk2BreakClear();
			updateTimeout = 2;
		} else {
			veAssert(veFalse);
		}
		break;

	case UPDATE_MK2_RELEASE_BREAK:
		if (ev == UPDATE_TIMER)
		{
			/* mk2 is in transpartent mode -> leave it... */
			logI("mk2_upd", "sending leave trans mode @115200");
			updateState = UPDATE_MK2_ESCAPING;
			mk2TxBuf(leaveTransMode, sizeof(leaveTransMode));
			updateTimeout = 4;
		} else {
			veAssert(veFalse);
		}
		break;

	case UPDATE_MK2_ESCAPING:
		if (ev == UPDATE_TIMER)
		{
			const un8 P = 'P';

			logI("mk2_upd", "querying version @19200");
			updateState = UPDATE_AWAITING_VERSION;
			mk2SetBaudRate(MK2_BAUD_19200);
			pos = 0;
			bytesExpected = 5;
			mk2TxBuf(&P, 1);
			updateTimeout = 100 + 1;
		} else {
			veAssert(veFalse);
		}
		break;

	case UPDATE_AWAITING_VERSION:
		if (ev == UPDATE_TIMER)
		{
			updateResult(UPDATE_ERR_VERSION_TIMEOUT);
		} else if (ev == UPDATE_REPLY) {
			veAssert(pos == 5);
			if (buffer[0] != 3 || buffer[1] != 'V' || (un8) ('V' + buffer[2] + buffer[3] + buffer[4]) != 0)
			{
				updateResult(UPDATE_ERR_VERSION_INVALID);
				logW("mk2_upd", "invalid version received");
				return;
			}

			logI("mk2_upd", "got version, sending firmware");
			bytesSent = 0;
			pos = 0;
			bytesExpected = 1;
			updateTimeout = 360 + 1;		// Peter uses 18 sec
			sendFirmware();
			updateState = UPDATE_SENDING_FIRMWARE;
		} else {
			veAssert(veFalse);
		}
		break;

	case UPDATE_SENDING_FIRMWARE:
		if (ev == UPDATE_TIMER)
		{
			logW("mk2_upd", "programming timed out");
			updateResult(UPDATE_ERR_PROG_TIMEOUT);
		} else if (ev == UPDATE_REPLY) {
			if (buffer[0] == 'N')
			{
				logI("mk2_upd", "sending more data..");
				pos = 0;
				bytesExpected = 1;
				updateTimeout = 360 + 1;	// Peter uses 18 sec
				if (sendFirmware()) {
					logI("mk2_upd", "update succesfull (needs reset though)");
					updateResult(UPDATE_ERR_OK);
				}
			} else {
				logE("mk2_upd", "invalid programming response");
				updateResult(UPDATE_ERR_PROG_RESPONSE);
			}
		} else {
			veAssert(veFalse);
		}
		break;
	}
}

/* Start updating, eventually callback will be invoked with true/false */
void mk2FirmwareUpgrade(Mk2ResultCallback* callback)
{
	veAssert(updateState == UPDATE_IDLE);
	resultCallback = callback;
	mk2FirmwareUpdateState(UPDATE_BEGIN);
}

/* 50ms, main */
void mk2FirmwareTick(void)
{
	if (updateTimeout && --updateTimeout == 0)
		mk2FirmwareUpdateState(UPDATE_TIMER);

	if (dataReady)
	{
		dataReady = veFalse;
		mk2FirmwareUpdateState(UPDATE_REPLY);
	}
}

// on interrupt!
void mk2RxUpdateRxByte(un8 inbyte)
{
	if (bytesExpected == 0 || pos >= sizeof(buffer))
		return;

	buffer[pos++] = inbyte;
	// If data was expected, let the main loop pick it up..
	if (--bytesExpected == 0)
	{
		dataReady = veTrue;
		veTodo();
	}
}

/* Returns if update is busy, to suspend normal behaviour */
veBool mk2UpdateBusy(void)
{
	return updateState != UPDATE_IDLE;
}
