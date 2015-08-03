#include <stdio.h>

#include <velib/io/ve_vstream.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/types/types.h>
#include <velib/types/variant.h>
#include <velib/utils/ve_assert.h>

/**
 * Set the values of the variant to the value reads from the input stream in
 * Little Endian format and moves on.
 *
 * @note Not all known types are supported!
 * @note
 *		Mind the string pointers; read only and not per definition zero ended.
 *		Can be NULL as well if mallformed...
 */
void veInVariantLe(VeVariant* variant)
{
	sn8 length;

	switch(variant->type.tp)
	{
		case VE_UN8:
		case VE_SN8:
			variant->value.UN8 = veInUn8();
			return;

		case VE_UN16:
		case VE_SN16:
			variant->value.UN16 = veInUn16Le();
			return;

#ifndef CFG_VE_IN_LE_UN32_DISABLE
		case VE_UN32:
		case VE_SN32:
			variant->value.UN32 = veInUn32Le();
			return;
#endif

#ifdef CFG_WITH_NMEA2K
		case VE_STR_N2K:
			variant->value.Ptr = (un8*) veInStrN2k();
			return;
#endif

#ifdef CFG_VE_IN_LE_FLOAT_ENABLE
		case VE_FLOAT:
			variant->value.Float = veInFloatLe();
			return;
#endif
		default:
			break;
	}

#ifndef CFG_VE_IN_LE_BITS32_DISABLE
	// handle bit field, per definition UN32
	if ((length = veVariantIsBits(variant->type.tp)) != 0) {
		variant->value.UN32 = veInBits32Le(length);
		return;
	}
#endif

#ifndef CFG_VE_IN_LE_STRING_DISABLE
	if ((length = veVariantIsCharArray(variant->type.tp)) != 0) {
		// not per definition zero ended (end not copied)!
		variant->value.Ptr = (un8*) veInFixedString(length);
		return;
	}
#endif

	veStreamIn->error = veTrue;
	veAssert("Input: type not implemented!\n");
}
