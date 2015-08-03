#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/base/base.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_tx.h>
#include <velib/nmea2k/n2k.h>
#include <velib/nmea2k/n2k_complex.h>
#include <velib/nmea2k/n2k_fp.h>

static un8 cmFpSequence = 0;

/**
 * Allocates a messages for a Complex Group Function.
 * 
 * @param tg		NAD where the message must be addresses to.
 * @param dp		datapage
 * @param pgn		parameter group number
 * @param cmType	Type of Complex Function, request, ack, command etc.
 * 
 * The message is set as output message. @ref n2kMsgOut will be NULL when no
 * message could be allocated.
 */
void n2kCmAlloc(J1939Nad tg, un8 dp, un16 pgn, N2kCmType cmType)
{
	switch (cmType)
	{
	case N2K_CM_ACK:
	case N2K_CM_READ_REPLY:
	case N2K_CM_WRITE_REPLY:
		// Replies might use a different resource since the command cannot be complied
		// with if there is no message available to respond.
		veOutMsg((J1939Msg*) n2kFpReplyAlloc());
		break;

	default:
		// Command and request etc can simply retry later if all message are used.
		veOutMsg((J1939Msg*) n2kFpTxAlloc());
		break;
	}

	if (n2kMsgOut == NULL)
		return;

	/* set properties of message */
	n2kMsgOut->msgId.w = J1939ID_PRI(3) | J1939ID_DP(1) | 
						J1939ID_PGN(N2K_PGN1_COMPLEX_GROUP) | J1939ID_TG(tg);

	/* start of data */
	veOutUn8((un8)cmType);
	veOutPgn(dp, pgn);
}

/** 
 * Queues the constructed NMEA 2000 Complex Function for output.
 * 
 * This is a common function since the sequence counter must increase for
 * each different fuction (since the same PGN is used). The fp counter is
 * automatically increased.
 */
void n2kCmSend(void)
{
	(void)veOutFinalize();
	n2kFpSequence((N2kFp*) n2kMsgOut, &cmFpSequence);
	j1939TxPush(n2kMsgOut);
}
