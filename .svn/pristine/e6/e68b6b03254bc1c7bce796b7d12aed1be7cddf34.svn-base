#include "string.h"

#include <velib/io/ve_stream.h>

VeStream* 	veStreamOut;

/**
 * Sets the output stream. The position and error are reset.
 * The buffer and it's length must be set before calling.
 */
void veOutStream(VeStream* stream)
{
	veStreamOut = stream;
	veStreamOut->index = 0;
#ifndef CFG_VE_OUT_BITS_DISABLE
	veStreamOut->bitsUsed = 0;
#endif
	veStreamOut->error = veFalse;
}

#ifndef CFG_VE_OUT_REPLACE_DISABLE

/**
 * like @ref veInStream, but set the buffer and returns the current stream.
 * This is usefull for functions with temporarily need an endian conversion.
 * The orignal stream can be reset with @ref veInRestore.
 *
 * The new stream must be different from the current.
 */
VE_DCL VeStream const* veOutStreamInit(VeStream* stream, un8* buffer, VeBufSize length)
{
	VeStream const *cur;
	cur = (VeStream const*) veStreamOut;
	stream->buffer = buffer;
	stream->length = length;
	veOutStream(stream);
	return cur;
}

/// restore the output stream
VE_DCL void veOutRestore(VeStream const* stream)
{
	// the external app is not allowed to change it, from here is ok (cast away the const)
	veStreamOut = (VeStream*) stream;
}

#endif

/// Returns if there is a overflow at the moment and set error flag if so.
veBool veOutOverflow(void)
{
	// do not move past end of buffer!
	if (veStreamOut->index >= veStreamOut->length )
	{
		veStreamOut->error = veTrue;
		return veTrue;
	}
	return veFalse;
}

/// Add an un8 to the buffer.
void veOutUn8(un8 value)
{
	/* do not write past end of buffer! */
	if (veOutOverflow())
		return;

#ifndef CFG_VE_OUT_BITS_DISABLE
	/* don't overwrite the bitfields */
	if (veStreamOut->bitsUsed != 0)
	{
		veStreamOut->error = veTrue;
		return;
	}
#endif

	veStreamOut->buffer[veStreamOut->index++] = value;
}

#ifndef CFG_VE_OUT_BYTES_LEFT_DISABLE
/// Number of bytes which can still be added.
VeBufSize veOutBytesLeft(void)
{
	return veStreamOut->length - veStreamOut->index;
}
#endif

#ifndef CFG_VE_OUT_PTR_DISABLE
/**
* returns the pointer to the current position in the out-stream.
* @note returns NULL on overflow.
*/

un8 * veOutPtr(void)
{
	// return NULL on error
	if (veStreamOut->error)
		return NULL;

	return &veStreamOut->buffer[veStreamOut->index];
}
#endif

#ifndef CFG_VE_OUT_MOVE_DISABLE
/**
* Moves the current position to the n'th byte.
*
* @note
*	- error flag will be set when out of range.
*	- bit field is reset.
*/
void veOutMove(VeBufSize byte)
{
	if (byte >= veStreamOut->length)
	{
		veStreamOut->error = veTrue;
		return;
	}

	veStreamOut->index = byte;
#ifndef CFG_VE_BITS_DISABLE
	veStreamOut->bitsUsed = 0;
#endif
}

#ifndef CFG_VE_OUT_BITS_DISABLE
/**
 * Move to a certain bit field from the begin of the buffer.
 *
 * @param bitPos
 *	the position to move to, e.g. 9 is byte 1, bit 2.
 */
void veOutMoveBit(un8 bitPos)
{
	veOutMove((bitPos >> 3));
	veStreamOut->bitsUsed = bitPos & 0x7;
}
#endif

#endif // CFG_VE_OUT_MOVE_DISABLE

#ifndef CFG_VE_OUT_STRING_DISABLE
/**
 * Adds a zero ended string to the buffer as far a it fits.
 *
 * @note the trailing zero is not added if it didn't fit.
 */
void veOutStr(char const * const str)
{
	veOutFixedStr(str, (un8) strlen( str));
	veOutUn8(0);
}

/**
 * Adds the str to the buffer up maxlength.
 * If the string is shorter it is padded with 0xFF, apparently some n2k corrigendum.
 *
 * @param str
 *	the string to add.
 * @param maxlength
 *	the length to add or to padd till.
 */
void veOutFixedStr(char const * const str, un8 const maxlength)
{
	un8 n;
	un8 length;
	char const * ptr = str;

	length = (un8) strlen(str);

	for (n=0; n < maxlength; n++)
	{
		if (n >= length)
			veOutUn8(0xFF);
		else
			veOutUn8(*ptr++);

	}
}
#endif

#ifndef CFG_VE_OUT_BUF_DISABLE
/**
 * Adds a fixed sized buffer and moves on.
 */
void veOutBuf(un8 const * buf, un8 len)
{
	while(len--)
		veOutUn8(*buf++);
}
#endif
