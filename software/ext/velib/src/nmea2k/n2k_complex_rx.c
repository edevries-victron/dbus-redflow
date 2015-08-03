#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/base/base.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939.h>
#include <velib/nmea2k/n2k.h>
#include <velib/nmea2k/n2k_complex_rx.h>
#include <velib/nmea2k/n2k_fp.h>

/**
 * @addtogroup VELIB_NMEA2K_COMPLEX
 * @details
 *
 * <b>Sending Complex Acks</b>
 *
 * @code
 *	n2kCmPrepareAck(0x50, 0,  J1939_PGN0_ACL);
 *	if (n2kMsgOut == NULL)
 *		return;
 *	veOutUn8(3);		// number of arguments
 *	veOutBits8Le(N2K_CM_PARAM_INVALID, 4);
 *	veOutBits8Le(N2K_CM_PARAM_INVALID, 4);
 *	veOutBits8Le(N2K_CM_PARAM_INVALID, 4);
 *	n2kCmSend();
 *
 * @endcode
 */


/**
 * Will send the ACK if it was addressed otherwise it is freed.
 *
 * Complex ACKs should only be returned when the target is destinition specific.
 * Both sitatuations can be handled seperately (but then care should be taken
 * not to add a reply field to the non-existing ACK). The simpler approach is to
 * always build the ACK and just free it again if it doesn't need to be send.
 */
void n2kCmSendAddressed(void)
{
	if (n2kMsgIn->msgId.p.ps == J1939_NAD_BROADCAST)
	{
		n2kFpTxFree((N2kFp*) n2kMsgOut);
		return;
	}

	n2kCmSend();
}

/**
 * Send Complex ACK without parameters.
 *
 * It must have been allocated first, @see n2kCmPrepareAck.
 */
void n2kCmSendAck(N2kCmPgnCode pgnCode, N2kCmTransCode transCode)
{
	veOutUn8((transCode << 4) | pgnCode);	// error codes.
	veOutUn8(0);							// number of parameters
	n2kCmSendAddressed();
}

/**
 * Minimum requirement for Complex Messages; responding that it is not supported.
 *
 * This message is e.g. send for unknown opcodes (functions).
 * The ACK will only be send if the tg is not the Broadcast address.
 * The ACK will be allocated, @ref n2kCmPrepareAck should NOT be called before
 * calling this function.
 */
veBool n2kCmSendNotImplemented(J1939Nad tg, un8 dp, un16 pgn)
{
	if (tg == J1939_NAD_BROADCAST)
		return veFalse;

	n2kCmPrepareAck(tg, dp, pgn);
	if (n2kMsgOut == NULL)
		return veFalse;

	n2kCmSendAck(N2K_CM_PGN_REQ_NOT_SUPPORTED, N2K_CM_TRANS_REQ_NOT_SUPPORTED);
	return veTrue;
}
