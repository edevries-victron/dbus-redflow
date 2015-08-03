#ifndef _VELIB_TYPES_VARIANT_PRINT_H_
#define _VELIB_TYPES_VARIANT_PRINT_H_

#include <velib/types/variant.h>

typedef struct {
	un8 decimals;
	char* unit;
} VeVariantUnitFmt;

size_t veVariantSprintf(VeVariant const *variant, VeVariantUnitFmt const *fmt, char *buf, size_t len);
size_t veVariantFmt(VeVariant *var, const void *ctx, char* buf, size_t len);

#endif
