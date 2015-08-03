#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939_sf.h>
#include <velib/J1939/J1939_tx.h>
#include <velib/nmea2k/n2k_fp.h>

// utils, while the other file focus on easily creating an message,
// the utils combine this with actually sending.
//
// This is seperated, so message can be send by whatever means possible.
// The stack itself would however do the same thing over and over again.
// (allocating a message, setting it as output etc.)

/** 
 * Allocated a single frame message and set it as active output.
 * @ref n2kMsgOut will be NULL if allocation failed.
 * @retval if allocation was successfull
 */
veBool veOutAllocSf(void)
{
	veOutMsg((J1939Msg*) j1939SfTxAlloc());
	return n2kMsgOut != NULL;
}

/**
 * Pad the active frame, update the length and queue it. 
 * The message is freed if an error occured.
 */
void veOutSendSf(void)
{
	if (veOutFinalize() == veTrue)
	{
		j1939SfTxFree((J1939Sf*) n2kMsgOut);
		return;
	}

	j1939TxPush(n2kMsgOut);
}

#if CFG_WITH_FP

/** 
 * Allocated a fast packet message and set it as active output.
 * @ref n2kMsgOut will be NULL if allocation failed.
 * @retval if allocation was successfull
 */
veBool veOutAllocFp(void)
{
	veOutMsg((J1939Msg*) n2kFpTxAlloc());
	return n2kMsgOut != NULL;
}

/**
 * Update the length of the fast packet, set sequence and queue it. 
 * The message is freed if an error occured.
 */
void veOutSendFp(un8* seq)
{
	if (veOutFinalize() == veTrue)
	{
		n2kFpTxFree((N2kFp*) n2kMsgOut);
		return;
	}
	n2kFpSequence((N2kFp*) n2kMsgOut, seq);
	j1939TxPush(n2kMsgOut);
}

#endif
