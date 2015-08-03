#include <stdio.h>
#include <errno.h>

#include <velib/base/ve_string.h>
#include <velib/types/variant_console.h>
#include <velib/utils/ve_arg.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>

/**
 * @addtogroup VELIB_TYPES_VARIANT_CONSOLE
 * @brief
 *	All luxury / large functions not needed for embedded applications.
 */

/**
 * Converts a datatype to a descriptive text.
 */
void veTypeName(VeDatatype datatype, char* buf, un8 buflen)
{
	sn8 length;

	switch(datatype.tp)
	{
		case VE_UN8:
			ve_strcpy(buf, buflen, "un8");
			return;
		case VE_SN8:
			ve_strcpy(buf, buflen, "sn8");
			return;
		case VE_UN16:
			ve_strcpy(buf, buflen, "un16");
			return;
		case VE_SN16:
			ve_strcpy(buf, buflen, "sn16");
			return;
		case VE_UN32:
			ve_strcpy(buf, buflen, "un32");
			return;
		case VE_SN32:
			ve_strcpy(buf, buflen, "sn32");
			return;
		case VE_STR:
			ve_strcpy(buf, buflen, "str");
			return;
		case VE_STR_N2K:
			ve_strcpy(buf, buflen, "strn2k");
			return;
		default:
			break;
	}

	length = veVariantIsBits(datatype.tp);
	if (length > 0)
	{
		ve_snprintf(buf, buflen, "bit%d", length);
		return;
	}

	length = veVariantIsCharArray(datatype.tp);
	if (length > 0)
	{
		ve_snprintf(buf, buflen, "char%d", length);
		return;
	}

	ve_strcpy(buf, buflen, "-");
}

/**
 * Reverse of veTypeName, returns the datatype from description str.
 *
 * VE_UNKNOWN is returned when not known.
 */
VeDataBasicType veTypeFromName(char const* str)
{
	int length;

	if (ve_stricmp(str, "un8") == 0)
		return VE_UN8;

	if (ve_stricmp(str, "sn8") == 0)
		return VE_SN8;

	if (ve_stricmp(str, "un16") == 0)
		return VE_UN16;

	if (ve_stricmp(str, "sn16") == 0)
		return VE_SN16;

	if (ve_stricmp(str, "un32") == 0)
		return VE_UN32;

	if (ve_stricmp(str, "sn32") == 0)
		return VE_SN32;

	if (ve_stricmp(str, "str") == 0)
		return VE_STR;

	if (ve_stricmp(str, "strn2k") == 0)
		return VE_STR_N2K;

	if (ve_stricmp(str, "bit") == 0)
		return VE_BIT1;

// suppress sscanf not secure
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

	if (sscanf(str, "char%d", &length) == 1 && length <= VE_CHARN_MAX_LEN)
		return VE_CHAR0 + length;

	if (sscanf(str, "bit%d", &length) == 1 && length <= VE_BITN_MAX_LEN)
		return VE_BIT0 + length;

#ifdef _MSC_VER
#pragma warning(pop)
#endif

	return VE_UNKNOWN;
}

/**
 * Prints the value of the variant by default formatters.
 */
void veVariantFPrint(FILE* file, VeVariant* variant)
{
	switch(variant->type.tp)
	{
		case VE_SN8:
		case VE_UN8:
			fprintf(file, "%d", (un16) variant->value.UN8);
			return;

		case VE_UN16:
		case VE_SN16:
			fprintf(file, "%d", variant->value.UN16);
			return;

		case VE_UN32: // needs %ul?
		case VE_SN32:
			fprintf(file, "%d", variant->value.UN32);
			return;

#if CFG_WITH_FLOAT
		case VE_FLOAT:
			fprintf(file, "%f", variant->value.Float);
			return;
#endif

		case VE_STR_N2K:
		case VE_STR:
		case VE_HEAP_STR:
		fprintf(file, "%s", (char*) variant->value.Ptr);
			return;

		default:
			break;
	}

	if (veVariantIsBits(variant->type.tp) > 0)
	{
		fprintf(file, "%d", variant->value.UN32);
		return;
	}

	if (veVariantIsCharArray(variant->type.tp) > 0)
	{
		fprintf(file, "%s", (char *) variant->value.Ptr);
		return;
	}

	logE("Variant", "FPrint, unknown type: %d", variant->type);
	veAssert(veFalse);
}

/**
 * Prints the value of the variant in hex if possible
 */
void veVariantFPrintX(FILE* file, VeVariant* variant)
{
	switch(variant->type.tp)
	{
		case VE_SN8:
		case VE_UN8:
			fprintf(file, "0x%X", (un16) variant->value.UN8);
			return;

		case VE_UN16:
		case VE_SN16:
			fprintf(file, "0x%X", variant->value.UN16);
			return;

		case VE_UN32: // needs %ul?
		case VE_SN32:
			fprintf(file, "0x%X", variant->value.UN32);
			return;

		default:
			break;
	}

	if (veVariantIsBits(variant->type.tp) > 0)
	{
		fprintf(file, "%X", variant->value.UN32);
		return;
	}

	veVariantFPrint(file, variant);
}

/**
 * Sets the variant to the type and value as indicated by str.
 *
 * @param[out] variant
 *		The variant to set.
 * @param str
 *		Type and values string as described below.
 * @retval
 *		Whether conversion was succesfull.
 *
 * The format is type:value, e.g. un8:255 or un8:0xFF.
 * Fixed length are supported as bit12 and char12 e.g. For the ease
 * of use charn:example will automatically create a char7.
 *
 * str:zero				creates a zero ended string.
 * str_n2k:yetanother	a variable length but not zero ended string.
 *
 * @note
 *		while not zero ended string are supported as bus formats
 *		they must be zero ended for internal use.
 * @note
 *		For string types, the input string is referenced, not copied!
 *		It should therefore remain to exist.
 */
veBool veVariantFromDefStr(VeVariant* variant, char* str)
{
	char* sep = strchr(str, ':');
	sn8 length;

	variant->type.tp = VE_UN8;

	if (sep != NULL)
	{
		*sep = 0;
		variant->type.tp = veTypeFromName(str);
		if (variant->type.tp == VE_UNKNOWN)
		{
			if (ve_stricmp(str, "charn") != 0)
				return veFalse;

			// special type -> length determined by passed value
			length = (sn8) strlen(sep+1);
			if (length > VE_CHARN_MAX_LEN)
				return veFalse;
			variant->type.tp = VE_CHAR0 + length;
		}
		str = sep+1;
	}

	// If this is a fixed length string
	length = veVariantIsCharArray(variant->type.tp);
	if (length > 0 || variant->type.tp == VE_STR_N2K || variant->type.tp == VE_STR)
	{
		variant->value.Ptr = (un8*) str;
		return veTrue;
	}

	// Handle floats seperatly.
#if CFG_WITH_FLOAT
	if (variant->type.tp == VE_FLOAT)
	{
		variant->value.Float = veArgFloat(str);
		if (variant->value.Float == 0 && errno != 0)
			return veFalse;
		return veTrue;
	}
#endif

	// All numeric types.
	variant->value.SN32 = veArgInt(str);
	if (variant->value.SN32 == 0 && errno != 0)
		return veFalse;
	veVariantFromN32(variant, variant->type.tp);

	return veTrue;
}
