#include <velib/io/ve_vstream.h>
#include <velib/types/variant.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>

/**
 * Reads the type of the variant from the input stream in Big Endian format
 * and moves on.
 *
 * @note Not all known types are supported!
 */
void veInVariantBe(VeVariant* variant)
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
			variant->value.UN16 = veInUn16Be();
			return;

		case VE_UN32:
		case VE_SN32:
			variant->value.UN32 = veInUn32Be();
			return;
			
		default:
			break;
	}

#ifndef CFG_VE_IN_BE_BITS32_DISABLE
	// handle bit field, per definition UN32
	if ((length = veVariantIsBits(variant->type.tp)) != 0) {
		variant->value.UN32 = veInBits32Be(length);
		return;
	}
#endif

	logE("INVariant", "ERROR: variant_be type not implemented!\n");
	veAssert(veFalse);
}

