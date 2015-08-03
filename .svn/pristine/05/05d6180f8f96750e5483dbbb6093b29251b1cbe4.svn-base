#ifndef _VELIB_TYPES_VARIANT_CONSOLE_H_
#define _VELIB_TYPES_VARIANT_CONSOLE_H_

#include <stdio.h>

#include <velib/velib_config.h>
#include <velib/types/variant.h>

/**
 * @ingroup VELIB_TYPES
 * @defgroup VELIB_TYPES_VARIANT_CONSOLE Variant Console
 */
/// @{


VE_DCL	veBool			veVariantFromDefStr(VeVariant* variant, char* str);
VE_DCL	VeDataBasicType	veTypeFromName(char const* str);
VE_DCL	void			veVariantFPrint(FILE* file, VeVariant* variant);
VE_DCL	void			veVariantFPrintX(FILE* file, VeVariant* variant);

VE_DCL	void			veTypeName(VeDatatype datatype, char* buf, un8 buflen);

/// @}

#endif
