#include "string.h"

#include <velib/io/ve_stream_n2k.h>

VeStream n2kStreamIn;
J1939Msg const* n2kMsgIn;

/** 
* Sets the active input message and points to the begin.
*
* @param msg	the message to read from
* @note			NULL can be passed.
*/
void veInMsg(J1939Msg const* msg)
{
	n2kMsgIn = msg;
	if (msg == NULL)
		return;
	n2kStreamIn.length = n2kMsgIn->length;
	n2kStreamIn.buffer = (un8*) n2kMsgIn->mdata;
	veInStream(&n2kStreamIn);
}

#ifndef CFG_VE_IN_N2K_PGN_DISABLE
/**
 * Read PGN / datapage from the message and move on.
 *
 * @param[out] pgn		the PGN
 * @retval				datapage
 */
un8 veInPgn(un16 *pgn)
{
	*pgn = veInUn16Le();
	return veInUn8();
}
#endif


#ifndef CFG_VE_IN_STRING_DISABLE
/**
 * Reads a NMEA 2000 string from the messages and moves on.
 * 
 * The string itself is not zero ended and can't be forced to be so, since there
 * might be other data behind it. The string is therefore returned in the original
 * format and is therefore not zero terminited!
 *
 * The string is checked for saneness, NULL is returned on incorrect format 
 * or underflow!
 *
 * @note this is thus NOT the reverse of @ref veOutN2kStr.
 */
char const* veInStrN2k(void)
{
	char const* ptr;

	ptr = (char const*) veInPtr();	// to the length
	if (ptr == NULL)
		return (char const*) NULL;

	// Even a empty string has length 2...
	if (*ptr < 2)
	{
		veStreamIn->error = veTrue;
		return (char const*) NULL;
	}

	// move length bytes on
	return veInFixedString(*ptr);
}
#endif

