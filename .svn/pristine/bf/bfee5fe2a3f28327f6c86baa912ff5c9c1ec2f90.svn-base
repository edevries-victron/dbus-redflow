#include <stdio.h>

#include <velib/base/ve_string.h>
#include <velib/types/variant.h>
#include <velib/types/variant_print.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>

size_t veVariantSprintf(VeVariant const *variant, VeVariantUnitFmt const *fmt, char *buf, size_t len)
{
	VE_UNUSED(fmt);

	if (veVariantIsBits(variant->type.tp))
		return ve_snprintf(buf, len, "%X", variant->value.UN32);

	switch(variant->type.tp)
	{
	case VE_UN8:
		return ve_snprintf(buf, len, "%u", (un16) variant->value.UN8);
	case VE_SN8:
		return ve_snprintf(buf, len, "%d", (sn16) variant->value.SN8);

	case VE_UN16:
		return ve_snprintf(buf, len, "%u", variant->value.UN16);
	case VE_SN16:
		return ve_snprintf(buf, len, "%d", variant->value.SN16);

	case VE_UN32:
		return ve_snprintf(buf, len, "%u", variant->value.UN32);
	case VE_SN32:
		return ve_snprintf(buf, len, "%d", variant->value.SN32);

#if CFG_WITH_FLOAT
	case VE_FLOAT: {
		char fmtbuf[10];
		if ((size_t) ve_snprintf(fmtbuf, sizeof(fmtbuf), "%%.%df", fmt->decimals) >= sizeof(fmtbuf))
			return 0;
		return ve_snprintf(buf, len, fmtbuf, variant->value.Float);
	}
	case VE_FLOAT_ARRAY:
		return ve_snprintf(buf, len, "float array");
#endif

	case VE_STR:
	case VE_HEAP_STR:
		return ve_snprintf(buf, len, "%s", (char *) variant->value.Ptr);

	case VE_BUF:
	case VE_HEAP:
		return ve_snprintf(buf, len, "buffer (%d bytes)", variant->type.len);

	default:
		logE("Variant", "veVariantSprintf unknown type: %d", variant->type);
		veAssert(veFalse);
		return 0;
	}
}

size_t veVariantFmt(VeVariant *var, void const *ctx, char* buf, size_t len)
{
	VeVariantUnitFmt const *fmt = (VeVariantUnitFmt const *) ctx;
	size_t n;

	if (!veVariantIsValid(var))
		return ve_snprintf(buf, len, "%s", "");

	n = veVariantSprintf(var, fmt, buf, len);
	if (n >= len)
		return n;

	len -= n;
	if (fmt->unit)
		n += ve_snprintf(&buf[n], len, "%s", fmt->unit);

	return n;
}
