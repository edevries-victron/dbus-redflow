#include <velib/types/bit_pair.h>
#include <velib/utils/ve_assert.h>

/**
 * Set a bit pair in a data container
 *
 * @param[in]	container	Pointer to the data container
 * @param[in]	bitNr		The bit number. Note that there are 4-bitpairs in a byte.
 * 							So bit number 4 is the first bit in the second byte.
 * @param[in]	value		The 2-bit bit pair value. The definition of the value
 * 							is user defined.
 */
void veBitPairSet(un8 *container, un8 bitNr, VeBitPair value)
{
	un16 byteOffset;
	un8 bitOffset;

	// Extract byte and bit offset
	byteOffset = bitNr >> 2;
	bitOffset = (bitNr & VE_BIT_PAIR_MASK) << 1;

	// Mask off original value and set new value
	container[byteOffset] &= ~(VE_BIT_PAIR_MASK << bitOffset);
	container[byteOffset] |= (value & VE_BIT_PAIR_MASK) << bitOffset;
}

/**
 * Set n bits pair in a data container
 *
 * @param[in]	container	Pointer to the data container
 * @param[in]	bitNr		The bit number to start with. Note that there are 4-bitpairs in a byte.
 * 							So bit number 4 is the first bit in the second byte.
 * @param[in]	count		The number of bit pairs to set.
 * @param[in]	value		The 2-bit bit pair value. The definition of the value
 * 							is user defined.
 */
void veBitPairSetRange(un8 *container, un8 bitNr, un8 count, VeBitPair value)
{
	while (count)
	{
		veBitPairSet(container, bitNr, value);
		bitNr++;
		count--;
	}
}

/**
 * Get a bit pair from a data container
 *
 * @param[in]	container	Pointer to the data container
 * @param[in]	bitNr		The bit number. Note that there are 4-bitpairs in a byte.
 * 							So bit number 4 is the first bit in the second byte.
 * @return					The bit pair value
 */
VeBitPair veBitPairGet(un8 *container, un8 bitNr)
{
	un16 byteOffset;
	un8 bitOffset;

	// Extract byte and bit offset
	byteOffset = bitNr >> 2;
	bitOffset = (bitNr & VE_BIT_PAIR_MASK) << 1;

	return (container[byteOffset] >> bitOffset) & VE_BIT_PAIR_MASK;
}
