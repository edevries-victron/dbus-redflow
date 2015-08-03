#include <velib/io/ve_stream.h>

VeStream* veStreamIn; // info about buffer used for input

/**
 * Sets the current input stream to stream.
 * The current position is reset to the beginning and the error flag reset.
 *
 * @note The buffer and its length must be set before calling.
 */
void veInStream(VeStream* stream)
{
	veStreamIn = stream;
	veStreamIn->error = veFalse;
	veStreamIn->index = 0;
#ifndef CFG_VE_IN_BITS_DISABLE
	veStreamIn->bitsUsed = 0;
#endif

}

/**
 * Returns the next byte and moves on.
 *
 * @note
 *	Reading a byte is resumed at the next byte boundary even if there
 *	are still bits left.
 */
un8 veInUn8(void)
{
	// check underflow
	if (veInUnderflow())
		return 0;

#ifndef CFG_VE_IN_BITS_DISABLE
	// ignore the bits
	if (veStreamIn->bitsUsed)
	{
		veStreamIn->index++;
		veStreamIn->bitsUsed = 0;
	}
#endif

	// return the byte and move on
	return veStreamIn->buffer[veStreamIn->index++];
}

/// Returns if there is a underflow at the moment and sets the error flag if so.
veBool veInUnderflow(void)
{
	// do not move past end of buffer!
	if (veStreamIn->index >= veStreamIn->length )
	{
		veStreamIn->error = veTrue;
		return veTrue;
	}
	return veFalse;
}

#ifndef CFG_VE_IN_REPLACE_DISABLE
/** 
 * like @ref veInStream, but set the buffer and returns the current stream.
 * This is usefull for functions with temporarily need an endian conversion.
 * The orignal stream can be reset with @ref veInRestore.
 */
VE_DCL VeStream const* veInStreamInit(VeStream* stream, un8* buffer, VeBufSize length)
{
	VeStream * cur;
	cur = veStreamIn;
	stream->buffer = buffer;
	stream->length = length;
	veInStream(stream);
	return (VeStream const*) cur;
}

/// restore the input stream
VE_DCL void veInRestore(VeStream const* stream)
{
	// the external app is not allowed to change it, from here is ok (cast away the const)
	veStreamIn = (VeStream*) stream;
}
#endif

#ifndef CFG_VE_IN_MOVE_DISABLE
/**
* Moves the current position to the n'th byte.
*
* @note
*	- error flag will be set when out of range.
*	- bit field is reset.
*/
void veInMove(VeBufSize byte)
{
	if (byte >= veStreamIn->length)
	{
		veStreamIn->error = veTrue;
		return;
	} else
		veStreamIn->error = veFalse;
	
	veStreamIn->index = byte;
#ifndef CFG_VE_IN_BITS_DISABLE
	veStreamIn->bitsUsed = 0;
#endif
}

#ifndef CFG_VE_IN_BITS_DISABLE
/** 
 * Move to a certain bit field from the begin of the input buffer.
 * 
 * @param bitPos
 *	the position to move to, e.g. 9 is byte 1, bit 2.
 */
void veInMoveBit(un8 bitPos)
{
	veInMove((bitPos >> 3));
	veStreamIn->bitsUsed = bitPos & 0x7;
}
#endif
#endif

#ifndef CFG_VE_IN_PTR_DISABLE
/// Returns a pointer to the current input byte or NULL.
un8 const* veInPtr(void)
{
	// return NULL on error
	if (veStreamIn->error)
		return (un8 const*) NULL;

	return (un8 const*) &veStreamIn->buffer[veStreamIn->index];
}
#endif

#ifndef CFG_VE_IN_BYTES_LEFT_DISABLE
/**
 * Returns the number of bytes left in the stream.
 * @note bits are ignored (rounded up to a whole byte).
 */
VeBufSize veInBytesLeft(void)
{
	return veStreamIn->length - veStreamIn->index;
}
#endif

#ifndef CFG_VE_IN_STRING_DISABLE
/**
 * Returns a pointer to the string of size length and moves on or NULL.
 *
 * @note
 *	- The error flag will be set on under run.
 *	- The string does not necessarily have a trailing 0.
 */
char const* veInFixedString(VeBufSize length)
{
	un8 n;
	char const* ret;

	ret = (char const*) veInPtr();

	// move and flag error
	for (n = 0; n < length; n++)
		veInUn8();

	// return NULL on error
	if (veStreamIn->error)
		return (char const*) NULL;

	return ret;
}
#endif
