#include <velib/io/ve_stream.h>

/// Read an un16 from a Little Endian stream and move ahead.
un16 veInUn16Le(void)
{
	un16 ret;
	ret = veInUn8();
	ret += ((un16) veInUn8()) << 8;
	return ret;
}

#ifndef CFG_VE_IN_LE_UN24_DISABLE
/// Read a un24 in Little Endian format and move ahead.
un32 veInUn24Le(void)
{
	un32 ret;
	ret = veInUn8();
	ret |= ((un32) veInUn16Le() << 8);
	return ret;
}
#endif

/// Read an un32 from a Little Endian input stream and move ahead.
un32 veInUn32Le(void)
{
	un32 ret;
	ret = veInUn16Le();
	ret += ( ((un32) veInUn16Le()) << 16);
	return ret;
}

#ifdef CFG_VE_IN_LE_FLOAT_ENABLE
#ifndef CFG_VE_IN_LE_FLOAT_SUPPRESS_WARNING
#warning VeIn float support is enabled, suppress this warning by defining CFG_VE_IN_LE_FLOAT_SUPPRESS_WARNING
#endif
/** Read a float from a Little Endian input stream and move ahead.
 * @note There is no formal representation for this!
 */
float veInFloatLe(void)
{
	un32 val = veInUn32Le();
	return *(float*) &val;
}
#endif

#ifndef CFG_VE_IN_LE_BITS_DISABLE
/**
 * Read a bit field from the stream up to 8 bits.
 * Bytes are read in Little Endian order.
 * @code
 * [.43210..]				-> [00043210]
 * [210.....] [......43]	-> [00043210]
 * @endcode
 *
 * @internal
 * implementation:
 @verbatim
	- all 3 bits are in the byte
		 76543210	76543210
		[..210...]				original data
		[........] [..210...]	create un16
		[210...00] [00000000]	shift left
		[00000000] [00000210]	shift right

	- 7 bits field crossing a byte boundary
		 76543210	76543210
		[210.....] [....6543]   original data
		[....6543] [210.....]	create un16
		[6543210.] [....0000]   shift left
		[00000000] [06543210]   shift right
 @endverbatim
 */
un8 veInBits8Le(sn8 nbits)
{
	un16 ret;

	// check underflow
	if (veInUnderflow())
		return 0;

	// get first byte
	ret = veStreamIn->buffer[veStreamIn->index];
	veStreamIn->bitsUsed += nbits;

	// add next byte if required
	if (veStreamIn->bitsUsed >= 8)
	{
		veStreamIn->index++;
		if (veStreamIn->bitsUsed != 8)
		{
			// do not move past end of buffer!
			if (veInUnderflow())
				return 0;
			ret += veStreamIn->buffer[veStreamIn->index] << 8;
		}
	}

	// move unknown bits out, set to right position
	ret <<= (un8) (16 - veStreamIn->bitsUsed);
	ret >>= (un8) (16 - nbits);
	veStreamIn->bitsUsed %= 8;

	return (un8) ret;
}
#endif

#ifndef CFG_VE_IN_LE_BITS16_DISABLE
/// Like @ref veInBits8Le, but up to 16 bits
un16 veInBits16Le(sn8 nbits)
{
	un16 ret;
	ret = veInBits8Le( (sn8) (nbits > 8 ? 8 : nbits) );
	if (nbits > 8)
		ret += ((un16) veInBits8Le(nbits - 8)) << 8;

	return ret;
}
#endif

#ifndef CFG_VE_IN_LE_BITS32_DISABLE
/// Like @ref veInBits8Le, but up to 32 bits
un32 veInBits32Le(sn8 nbits)
{
	un32 ret;

	ret = veInBits16Le( (sn8) (nbits > 16 ? 16 : nbits) );
	if (nbits > 16)
		ret += ((un32) veInBits16Le(nbits - 16)) << 16;

	return ret;
}
#endif

