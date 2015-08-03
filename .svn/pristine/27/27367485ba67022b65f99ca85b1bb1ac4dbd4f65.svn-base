#include <velib/io/ve_vstream.h>

/// Add a un16 in Big Endian format and move ahead.
void veOutUn16Be(un16 value)
{
	veOutUn8( (un8) (value >> 8) );
	veOutUn8( (un8) value );
}

/// Add a un32 in Big Endian format and move ahead.
void veOutUn32Be(un32 value)
{
	veOutUn16Be( (un16) (value >> 16) );
	veOutUn16Be( (un16) value );
}

#if 0
/** Add a float in Big Endian format and move ahead.
 * @note There is no formal representation for this!
 * @note Disabled for that reason, not used anyway.
 */
void veOutFloatBe(float value)
{
	veOutUn32Be(*(un32*) &value);
}
#endif

/**
 * Adds a bit-field of at most 8 bits to the output stream.
 * The bits can cross a byte boundary. Bytes are added in
 * Big Endian order.
 *
 * @code
 * [...43210] -> [..432100]
 * [...43210] -> [......43] [21000000]
 * @endcode
 *
 * @note
 *	unused bits at the right are set to zero.
 *
 */
void veOutBits8Be(un8 value, sn8 nbits)
{
	un8 bits;
	un8 byte1;											// aligned bits added to first byte
	un8 mask;											// indicates the bits to reset

	if (veStreamOut->error)
		return;

	bits = 8 - veStreamOut->bitsUsed;					// remaining in current byte (8 - 6 = 2)
	value <<= (8 - nbits);								// left align current value [43210000]
	byte1 = value >> veStreamOut->bitsUsed;				// move to correct position [00000043]
	mask = 0xFF << bits;								// preserve [11111100]
	veStreamOut->bitsUsed += nbits;

	veStreamOut->buffer[veStreamOut->index] &= mask;	// bits to set to zero		[......00]
	veStreamOut->buffer[veStreamOut->index] |= byte1;	// set the bits				[......43]

	// not using whole byte
	if (veStreamOut->bitsUsed < 8)
		return;

	// move to next byte, but do not overflow
	veStreamOut->index++;
	veStreamOut->bitsUsed -= 8;

	if (veStreamOut->bitsUsed == 0)
		return;

	if (veOutOverflow())
		return;

	// remaining bits
	value <<= bits;										// [21000000]
	veStreamOut->buffer[veStreamOut->index] = value;
}

/// Like @ref veOutBits8Be, but up to 16 bits.
void veOutBits16Be(un16 value, sn8 nbits)
{
	if (nbits < 8)
	{
		veOutBits8Be((un8) value, nbits);
		return;
	}

	veOutBits8Be(value >> 8, nbits - 8 );
	veOutBits8Be((un8) value, 8);
}

/// Like @ref veOutBits8Be, but up to 32 bits.
void veOutBits32Be(un32 value, sn8 nbits)
{
	if (nbits < 16)
	{
		veOutBits16Be((un16) value, nbits);
		return;
	}

	veOutBits16Be(value >> 16, nbits - 16 );
	veOutBits16Be((un16) value, 16);
}

/// Like @ref veOutBits8Be, but the nbits'th bit is used as sign.
void veOutBits16BeSigned(sn16 value, sn8 nbits)
{
	if (nbits < 16 && value < 0)
		value += (1 << nbits);

	veOutBits16Be((un16) value, nbits);
}
