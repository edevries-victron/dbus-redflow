/**
 * @ingroup VELIB_NMEA2K
 * @defgroup VELIB_NMEA2K_FP_RX_MIN Fast Packet Single RX
 *	Minimalistic implementation for Fast Packet reception.
 * @brief 
 *	Fast Packet reception limited to a single message. No queue is needed.
 *  Typically used by bootloaders since they require only a reconstruction
 *	of a single FP from the updater.
 * @{
 */

#include <velib/J1939/J1939.h>
#include <velib/nmea2k/n2k_fp.h>
#include <velib/utils/ve_assert.h>

/// The buffer for reconstruction, timeout != 0 is allocated
static N2kFp fpRx;

/** 
 * @brief
 *	min: Init Fast Packet reconstruction
 */
void n2kFpInit(void)
{
	fpRx.timeout = 0;	
}

/** 
 * @details
 *	Reconstructs a SINGLE Fast Packet from J1939Sf messages.
 *
 * @param frame	
 *	The incoming frames which are part of the fast packet.
 *
 * @return		
 *	The reconstructed message if the incoming message completed
 *	it, NULL otherwise.
 *
 * @note
 *	- Incoming fast packets larger than @ref CFG_N2K_FP_MSG_SIZE are ignored.
 *	- It is assumed that the incoming message has size 8.
 * 	- Which fast packet to reconstruct is up to the caller.
 */
N2kFp* n2kFpData(J1939Sf *frame)
{
	un8	dataIndex;			// loops through frame data bytes
	un8	sequence;

	veAssert(frame->length == 8);

	// remove the priority. Only usefull for arbitration, annoying otherwise.
	frame->msgId.p.pg.p.pri = 0;
	sequence = frame->mdata[0];

	// Check if the message is already being reconstructed.
	if (
			fpRx.timeout != 0 && fpRx.msgId.p.src == frame->msgId.p.src &&
			fpRx.sequence == sequence
		)
	{
		fpRx.sequence++;					// increase expected sequence
		dataIndex = 1;						// begin of data
	}
	else if ((sequence & 0x1F) == 0)		// start of new fast packet
	{
		// if the packet is longer then the storage space, ignore it
		if (frame->mdata[1] > CFG_N2K_FP_MSG_SIZE)
			return NULL;

		// Just overwrite old if there was any
		fpRx.msgId.w = frame->msgId.w;
		fpRx.length = 0;
		fpRx.sequence = sequence + 1;
		fpRx.bytes = frame->mdata[1];
		dataIndex = 2;						// begin of data
	}
	else
	{
		// invalid fragment, ignore
		return NULL;
	}

	//-- action for correct frame --

	// reset receive timeout
	fpRx.timeout = CFG_N2K_FP_RX_FRAME_TIMEOUT;

	// copy the data bytes
	while (dataIndex < 8 && fpRx.length < fpRx.bytes)
		fpRx.mdata[fpRx.length++] = frame->mdata[dataIndex++];

	// if this frame completes the message return it
	if (fpRx.length == fpRx.bytes)
	{
		fpRx.timeout = 0;
		return &fpRx;
	}

	return NULL;
}


/** 
 * @brief min: Frees the Fast Packet if it timed-out.
 */
void n2kFpTick(void)
{
	if (fpRx.timeout != 0)
		fpRx.timeout--;
}

/// @}
