#include <velib/velib_config.h>
#include <velib/io/ve_stream.h>

/// Add a un16 in Little Endian format and move ahead.
void veOutUn16Le(un16 value)
{
	veOutUn8( (un8) value );
	veOutUn8( (un8) (value >> 8) );
}

#ifndef CFG_VE_OUT_LE_UN24_DISABLE
/// Add a un24 in Little Endian format and move ahead.
void veOutUn24Le(un32 value)
{
	veOutUn8( (un8) value );
	veOutUn16Le( (un16) (value >> 8) );
}
#endif

/// Add a un32 in Little Endian format and move ahead.
void veOutUn32Le(un32 value)
{
	veOutUn16Le( (un16) value );
	veOutUn16Le( (un16) (value >> 16) );
}

#ifdef CFG_VE_OUT_LE_FLOAT_ENABLE
#ifndef CFG_VE_OUT_LE_FLOAT_SUPPRESS_WARNING
#warning VeOut float support is enabled, suppress this warning by defining CFG_VE_OUT_LE_FLOAT_SUPPRESS_WARNING
#endif
/** Add a float in Little Endian format and move ahead.
 * @note There is no formal representation for this!
 */
void veOutFloatLe(float value)
{
	veOutUn32Le (*(un32*) &value);
}
#endif

#ifndef CFG_VE_OUT_LE_BITS_DISABLE
/**
 * Adds a bit-field of at most 8 bits to the output stream.
 * The bits can cross a byte boundary. Bytes are added in Little
 * Endian order. Already added bits are left untouched.
 *
 * @code
 * [...43210] -> [0043210.]
 * [...43210] -> [10......] [.....432]
 * @endcode
 *
 *
 */
void veOutBits8Le(un8 value, sn8 nbits)
{
	un8 mask;
	un16 shifted;

	if (veStreamOut->error)
		return;

	// remove invalid bits
	value &= (1 << nbits) - 1;

	// move to correct position [00000432] [10000000]
	// and preserve right [00111111]
	shifted = value << veStreamOut->bitsUsed;
	mask = (1 << veStreamOut->bitsUsed) - 1;

	/* Preserve values at the left (if any) */
	if (veStreamOut->bitsUsed + nbits < 7)
		mask |= 0xFF << (veStreamOut->bitsUsed + nbits);

	// set bits to zero and set ones
	veStreamOut->buffer[veStreamOut->index] &= mask;
	veStreamOut->buffer[veStreamOut->index] |= (un8) shifted;

	// is another byte needed?
	veStreamOut->bitsUsed += nbits;
	if (veStreamOut->bitsUsed < 8)
		return;
	veStreamOut->bitsUsed -= 8;

	// move to next byte, but do not overflow
	veStreamOut->index++;

	// adding 0 bits doesn't
	if (veStreamOut->bitsUsed == 0)
		return;

	if (veOutOverflow())
		return;

	// remaining bits
	veStreamOut->buffer[veStreamOut->index] = (un8) (shifted >> 8);
}
#endif

#ifndef CFG_VE_OUT_LE_BITS16_DISABLE
/// Like @ref veOutBits8Le, but up to 16 bits.
void veOutBits16Le(un16 value, sn8 nbits)
{
	veOutBits8Le( (un8) value, (sn8) (nbits < 8 ? nbits : 8) );
	if (nbits > 8)
		veOutBits8Le( (un8) (value >> 8), nbits - 8 );
}
#endif

#ifndef CFG_VE_OUT_LE_BITS32_DISABLE
/// Like @ref veOutBits8Le, but up to 32 bits.
void veOutBits32Le(un32 value, sn8 nbits)
{
	veOutBits16Le( (un16) value, (sn8) (nbits < 16 ? nbits : 16) );
	if (nbits > 16)
		veOutBits16Le( (un16) (value >> 16), nbits - 16 );
}
#endif
