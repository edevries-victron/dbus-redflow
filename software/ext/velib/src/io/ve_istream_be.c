#include <velib/io/ve_stream.h>

/// Read an un16 from a Big Endian input stream and move ahead.
un16 veInUn16Be(void)
{
	un16 ret;
	ret = veInUn8();
	ret <<= 8;
	ret += veInUn8();
	return ret;
}

/// Read an un32 from a Big Endian input stream and move ahead.
un32 veInUn32Be(void)
{
	un32 ret;
	ret = veInUn16Be();
	ret <<= 16;
	ret += veInUn16Be();
	return ret;
}

#if 0
/**
 * Read an float from a Big Endian input stream and move ahead.
 * @note There is no formal representation for this!
 * @note Disabled for that reason, not used anyway.
 */
float veInFloatBe(void)
{
	un32 value = veInUn32Be();
	return * ((float*) &value);
}

#endif

/**
 * Read a bit field from the stream up to 8 bits.
 * Bytes are read in Big Endian order.
 *
 * @code
 * [.43210..]				-> [00043210]
 * [......43] [210.....]	-> [00043210]
 * @endcode
 *
 * @internal
 *
 @verbatim
	- all 3 bits are in the byte [.43210..]
		 01234567   01234567
		[.43210..] [00000000]	get first byte
		[43210..0] [00000000]	align left
		[00000000] [00043210]	align right

	- 7 bits field crossing a byte boundary [.....654] [3210....]
		 01234567   01234567
		[.....654] [00000000]   get first byte
		[.....654] [3210....]   add second byte
		[6543210.] [...00000]	align left
		[00000000] [06543210]   align right
 @endverbatim
 */
un8 veInBits8Be(sn8 nbits)
{
	un16 ret;
	un8 bits;	// number of bits already read

	// check underflow
	if (veInUnderflow())
		return 0;

	// get first byte
	ret = veStreamIn->buffer[veStreamIn->index] << 8;
	bits = veStreamIn->bitsUsed;
	veStreamIn->bitsUsed += nbits;

	// add second if needed
	if (veStreamIn->bitsUsed >= 8)
	{
		veStreamIn->bitsUsed -= 8;
		veStreamIn->index++;

		// make space and add next byte if needed
		if (veStreamIn->bitsUsed != 0)
		{
			// do not move past end of buffer!
			if (veInUnderflow())
				return 0;

			// add next byte
			ret += veStreamIn->buffer[veStreamIn->index];
		}
	}

	ret <<= bits;			// align left
	ret >>= 16 - nbits;		// align right
	return (un8) ret;
}

/// Like @ref veInBits8Be, but up to 16 bits.
un16 veInBits16Be(sn8 nbits)
{
	un16 ret;

	if (nbits > 8)
	{
		ret = (un16) veInBits8Be(8) << (nbits - 8);
		ret += veInBits8Be(nbits - 8);
	} else {
		ret = veInBits8Be(nbits);
	}
	return ret;
}

/// Like @ref veInBits8Be, but up to 32 bits.
un32 veInBits32Be(sn8 nbits)
{
	un32 ret;

	if (nbits > 16)
	{
		ret = (un32) veInBits16Be(16) << (nbits - 16);
		ret += veInBits16Be(nbits - 16);
	} else {
		ret = veInBits16Be(nbits);
	}
	return ret;
}

/**
 * Like @ref veInBits8Be, but with the most significant bit
 * interpreted as sign.
 *
 * @code
 * Roughly: [......s3] [210.....] -> [s0003210]
 * @endcode
 * (above is not entirily correct but just to illustrate,
 *  negative numbers use two's complement)
 */
sn8 veInBits8BeSigned(sn8 nbits)
{
	sn8 ret;
	un8 mask;

	// 8-bit -> sign in place
	ret = (sn8) veInBits8Be(nbits);
	if (nbits == 8)
		return ret;

	// move sign to correct place
	mask = 1 << (nbits - 1);
	if (ret & mask)
		ret -= (mask << 1);

	// positive
	return ret;
}

/// Like @ref veInBits8BeSigned, but up to 16 bits.
sn16 veInBits16BeSigned(sn8 nbits)
{
	sn16 ret;
	un16 mask;

	// 16-bit -> sign in place
	ret = (sn16) veInBits16Be(nbits);

	if (nbits == 16)
		return ret;


	// move sign to correct place
	mask = 1 << (nbits - 1);
	if (ret & mask)
		ret -= mask << 1;

	// positive
	return ret;
}
