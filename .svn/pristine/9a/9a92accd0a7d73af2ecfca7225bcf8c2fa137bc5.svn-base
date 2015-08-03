#include "string.h"

#include <velib/io/ve_stream_n2k.h>

VeStream n2kStreamOut;
J1939Msg* n2kMsgOut;

/** 
 * Sets the active output message to be constructed.
 * 
 * @param msg	The message to construct.
 * 
 * The functions allows NULL as an argument and will set @ref n2kMsgOut accordingly.
 * This allows attempted to allocating a message and forcing @ref n2kMsgOut to become
 * NULL, so all callees can also see the failure by testing @ref n2kMsgOut.
 *
 * @code
 *	veOutMsg((J1939Msg*) n2kFpTxAlloc());
 *	if (n2kMsgOut == NULL)
 *		return;
 *	......
 * @endcode
 */
void veOutMsg(J1939Msg* msg)
{
	n2kMsgOut = msg;
	if (msg == NULL)
		return;
	n2kStreamOut.length = msg->length;
	n2kStreamOut.buffer = msg->mdata;
	veOutStream(&n2kStreamOut);
}

#ifndef CFG_VE_OUT_N2K_STRING_DISABLE
/** 
 * Adds NMEA 2000 variable length string.
 *
 * @param str	A zero ended string, NOT in NMEA 2000 format!
 *
 * @note ISO 8859-1 only, unicode not supported.
 * @note Character set conversion is not implemented.
 */
void veOutN2kStr(char const* str)
{
	un8 length;

	length = (un8) strlen((char*) str);
	veOutUn8(length + 2);
	veOutUn8(1);
	veOutFixedStr(str, length);
}
#endif

#ifndef CFG_VE_OUT_N2K_PGN_DISABLE
/**
 * Adds a PGN to the payload.
 *
 * @param datapage
 * @param pgn
 */
void veOutPgn(un8 datapage, un16 pgn)
{
	veOutUn16Le(pgn);
	veOutUn8(datapage);
}
#endif

/** 
* @brief 
*	Pads the message and sets the length of the message to the used length.
*
* @return
*	If an overflow had occured during the construction of the frame.
*
* @note
*	The length of a Single Frame is by default set to 8. If the output 
*	message sets all the 8 bytes there is no need to call this
*	routine (nor does it hurt).
*/
veBool veOutFinalize(void)
{
	veOutPadding();
	n2kMsgOut->length = n2kStreamOut.index;
	return n2kStreamOut.error;
}

/**
* @brief 
*	Adds the additional bits and/or bytes complete the frame.
*
* @details
*	NMEA 2000 requires that frames are padded with additional 1's.
*	Single Frame messages are always padded with reserved 1's to fill
*	a single frame.
*
*	For FastPacket messages the byte padding is part of the transport.
*	Only bits are padded.
*
* @note
*	If a Single Frame already contains 8 bytes or the other message 
*	don't end with partial bit fields there is no need to call this
*	function.
*/

void veOutPadding(void)
{
	VeBufSize n;

#ifndef CFG_VE_OUT_LE_BITS_DISABLE
	// add padding bits
	if (n2kStreamOut.bitsUsed != 0)
		veOutBits8Le(0xFF, 8 - n2kStreamOut.bitsUsed);
#endif

	// add padding bytes for Single Frame
	if (n2kMsgOut->type.p.kind == J1939_SF)
	{
		for (n = n2kStreamOut.index; n < 8; n++)
			veOutUn8(0xFF);
	}
}
