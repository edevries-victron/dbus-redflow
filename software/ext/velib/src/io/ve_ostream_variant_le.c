#include "stdio.h"

#include <velib/io/ve_vstream.h>
#include <velib/types/variant.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>
#include <velib/io/ve_stream_n2k.h>


/**
 * Add the variant to the output stream in Little Endian format.
 */
void veOutVariantLe(VeVariant* variant)
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
			veOutUn16Le(variant->value.UN16);
			return;

#ifndef CFG_VE_OUT_LE_UN24_DISABLE
		case VE_UN24:
			veOutUn24Le(variant->value.UN32);
			return;
#endif

#ifndef CFG_VE_OUT_LE_UN32_DISABLE
		case VE_UN32:
		case VE_SN32:
			veOutUn32Le(variant->value.UN32);
			return;
#endif

#ifdef CFG_VE_OUT_LE_FLOAT_ENABLE
		case VE_FLOAT:
			veOutFloatLe(variant->value.Float);
			return;
#endif

#ifndef CFG_VE_OUT_STRING_DISABLE
		case VE_STR:
			veOutStr((char*) variant->value.Ptr);
			return;

#if CFG_WITH_NMEA2K
		case VE_STR_N2K:
			veOutN2kStr((char const*) variant->value.Ptr);
			return;
#endif
#endif

#ifndef CFG_VE_OUT_BUF_DISABLE
		case VE_BUF:
			veOutBuf(variant->value.Ptr, variant->type.len);
			return;
#endif

		default:
			break;
	}

	// bit type support
	if ((length = veVariantIsBits(variant->type.tp)) > 0)
	{
		veOutBits32Le(variant->value.UN32, length);
		return;
	}

	if ((length = veVariantIsCharArray(variant->type.tp)) > 0)
	{
		veOutFixedStr((char const*) variant->value.Ptr, length);
		return;
	}

	veStreamOut->error = veTrue;
	logE("io", "veOutVariantLe: unknown type!");
	veAssert(veFalse);
}

