#include <velib/io/ve_vstream.h>
#include <velib/types/variant.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>

/**
 * Add the variant to the output stream in Big Endian format.
 */
void veOutVariantBe(VeVariant* variant)
{
	sn8 length;

	switch(variant->type.tp)
	{
		case VE_UN8:
		case VE_SN8:
			veOutUn8(variant->value.UN8);
			return;

		case VE_UN16:
		case VE_SN16:
			veOutUn16Be(variant->value.UN16);
			return;

		case VE_UN32:
		case VE_SN32:
			veOutUn32Be(variant->value.UN32);
			return;
			
		default:
			break;
			
	}

	// bit type support
	if ((length = veVariantIsBits(variant->type.tp)) > 0)
	{
		veOutBits32Be(variant->value.UN32, length);
		return;
	}

	logE("veOutVariantBe", "ERROR %d\n", variant->type.tp);
	veAssert(veFalse);
}

