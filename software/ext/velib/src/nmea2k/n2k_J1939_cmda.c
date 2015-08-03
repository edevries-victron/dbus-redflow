/** 
 * @addtogroup VELIB_NMEA2K_FP_CMDA
 * @brief 
 *	Support for J1939 BAM CMDA without the transport protocol.
 * @details
 *
 * Supports the J1939 commanded address without requiring the RTS/CTS, BAM 
 * transport protocol, see @ref VELIB_J1939_TRANSPORT. NMEA 2000 relies on the
 * Fast Packet transport for longer messages and does not require the J1939
 * transport. However the CMDA, which allows changing the current address is a 
 * J1939 transport messages and might be useful for NMEA 2000 devices as well.
 * To include the whole J1939 tranport procotol to receive a single message is
 * a bit of overkill though. Hence the transport protocol and the CMDA is 
 * handled here simultaniously and the encapsulated message is returned as a 
 * Fast Packet. Thus not requiring the complete J1939 transport protocol nor 
 * its messages. Since the code acts on @ref J1939Msg it is independend on the 
 * actual type of message being used; using Fast Packets is therefore completely
 * identical. The message will automatically be freed correctly, since it 
 * includes its type.
 */

#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/nmea2k/n2k_J1939_cmda.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_tp.h>

#include <string.h> // memcpy, memcmp

static N2kFp* msgCmda = NULL;

static const un8 bamCMDA[] = {0x20, 0x09, 0x00, 0x02, 0xFF, 0xD8, 0xFE, 0x00};

/**
 * Start reconstruction a CMDA BAM if it is announced.
 *
 * @ref J1939_PGN0_TP_CTSG should be passed here by @ref n2kMsgIn.
 */
void n2kJ1939CmdaDataBam(void)
{
	// only one BAM CMDA at a time.
	if (msgCmda != NULL || n2kMsgIn->length != 8)
		return;

	// A broadcast announce starts with a fixed message.
	if (memcmp(n2kMsgIn->mdata, bamCMDA, 8) != 0)
		return;

	msgCmda = n2kFpRxAlloc();
	if (msgCmda == NULL)
		return;

	// setup the message to be reconstructed.
	msgCmda->msgId.w = J1939ID_PGN(J1939_PGN0_CMD_NAD) | n2kMsgIn->msgId.p.src;
	msgCmda->sequence = 1;
	msgCmda->length = 9;
	msgCmda->timeout = J1939_TP_RX_PACKET_TIMEOUT;
}

/**
 * Reconstructs the data of the BAM CMDA in a fp message.
 *
 * @return The CMDA if completly reconstructed, NULL otherwise.
 *
 * @ref J1939_PGN0_TP_DATA should be passed here by @ref n2kMsgIn.
 */
N2kFp* n2kJ1939CmdaData(void)
{
	N2kFp* ret;

	if (
			msgCmda == NULL || msgCmda->msgId.p.src != n2kMsgIn->msgId.p.src ||
			msgCmda->sequence != veInUn8()
		)
	{
		return NULL;
	}

	// The second data frame..
	if (msgCmda->sequence == 2)
	{
		(void)memcpy(&msgCmda->mdata[7], &n2kMsgIn->mdata[1], 2);
		ret = msgCmda;
		msgCmda = NULL;
		return ret;
	}

	// The first data frame.
	(void)memcpy(&msgCmda->mdata[0], &n2kMsgIn->mdata[1], 7);
	msgCmda->sequence++;

	return NULL;
}

/// Releases the CMDA on timeout.
void n2kJ1939CmdaTick(void)
{
	if (msgCmda && msgCmda->timeout-- == 0)
	{
		n2kFpRxFree(msgCmda);
		msgCmda = NULL;
	}
}
