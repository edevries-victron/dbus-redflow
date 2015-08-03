#include "string.h"
#include "stdio.h"

#include <velib/types/variant.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_assert.h>

/**
 * @addtogroup VELIB_TYPES_VARIANT
 * @brief
 *	Not really a Variant but a general union able to store different types
 *	and with the type attached.
 * @details
 *	The variant reused the same 4 bytes of memory to store the received
 *	values from the bus. Contrary to just reinterpreting a buffer, the values
 *	are typed and the compiler / linker can properly align these variables
 *	depending on platform requirements.
 */

#if CFG_WITH_FLOAT

/**
 * Conversion of numerical formats to a float.
 *
 * Reminder: A float cannot store a number at infinite accuracy.
 * Typically casting from / to int between -2^24 .. 2^24 is fine.
 * In general floats should be regarded as inaccurate.
 */
void veVariantToFloat(VeVariant* variant)
{
	if (veVariantIsBits(variant->type.tp))
		variant->value.Float = (float) variant->value.UN32;

	switch (variant->type.tp)
	{
		case VE_UN8:
			variant->value.Float = (float) variant->value.UN8;
			break;

		case VE_SN8:
			variant->value.Float = (float) variant->value.SN8;
			break;

		case VE_UN16:
			variant->value.Float = (float) variant->value.UN16;
			break;

		case VE_SN16:
			variant->value.Float = (float) variant->value.SN16;
			break;

		case VE_UN32:
			variant->value.Float = (float) variant->value.UN32;
			break;

		case VE_SN32:
			variant->value.Float = (float) variant->value.SN32;
			break;

		case VE_FLOAT:
			break;

		default:
			return;
	}

	variant->type.tp = VE_FLOAT;
}


#define CMIN(a) if(variant->value.Float  < a) variant->value.Float  = (float) a;
#define CMAX(a) if(!validFloat(variant->value.Float) || variant->value.Float > a) variant->value.Float  = (float) a;

/**
 * Conversion from a float to another type.
 *
 * The type will be set accordingly.
 */
void veVariantFromFloat(VeVariant* variant, VeDataBasicType type)
{
	// If this is needed, something strange is going on!
	if (veVariantIsBits(variant->type.tp))
		variant->value.UN32 = (un32) variant->value.Float;

	// Regular conversion
	switch (type)
	{
		case VE_UN8:
			CMIN(0);
			CMAX(0xFF);
			variant->value.UN8 = (un8) variant->value.Float;
			break;

		case VE_SN8:
			CMIN(-0x80);
			CMAX(0x7F);
			variant->value.SN8 = (sn8) variant->value.Float;
			break;

		case VE_UN16:
			CMIN(0);
			CMAX(0xFFFF);
			variant->value.UN16 = (un16) variant->value.Float;
			break;

		case VE_SN16:
			CMIN(-8000);
			CMAX(0x7FFF);
			variant->value.SN16 = (sn16) variant->value.Float;
			break;

		case VE_UN32:
			CMIN(0);
			CMAX(0xFFFFFFFFUL);
			variant->value.UN32 = (un32) variant->value.Float;
			break;

		case VE_SN32:
			CMIN(-80000000L);
			CMAX(0x7FFFFFFFL);
			variant->value.SN32 = (sn32) variant->value.Float;
			break;

		case VE_FLOAT:
			break;

		default:
			return;
	}

	variant->type.tp = type;
}


/// Applies scale to the value for incoming values.
/// @note This will force, the variant to become a float type!
void veVariantTransformIn(VeVariant * variant, VeScale const * scale)
{
	veVariantToFloat(variant);
	variant->value.Float *= scale->factor;
	variant->value.Float += scale->offset;
}

/// Applies scale to the value for outgoing values.
/// @note This will force, the variant to become a float type!
void veVariantTransformOut(VeVariant * variant, VeScale const * scale)
{
	veVariantToFloat(variant);
	variant->value.Float -= scale->offset;
	variant->value.Float /= scale->factor;
}

#endif

#define CUN32MAX(a) if(variant->value.UN32  > a) variant->value.UN32  = (un32) a;
#define CSN32MIN(a) if(variant->value.SN32  < a) variant->value.SN32  = (sn32) a;
#define CSN32MAX(a) if(variant->value.SN32  > a) variant->value.SN32  = (sn32) a;

/**
 * Conversion from a 32bit integer to another type.
 * It is assumed that the type of the same sign must be used.
 * (floats are considered signed as well). The difference with a direct
 * cast is that values with are too large to fit in the new type get
 * the max value. Idem for the lower bound.
 */
void veVariantFromN32(VeVariant* variant, VeDataBasicType type)
{
	// Bit fields are stored as un32.
	if (veVariantIsBits(variant->type.tp))
		return;

	// Regular conversion
	switch (type)
	{
		case VE_UN8:
			CUN32MAX(0xFF);
			variant->value.UN8 = (un8) variant->value.UN32;
			break;

		case VE_SN8:
			CSN32MIN(-0x80);
			CSN32MAX(0x7F);
			variant->value.SN8 = (sn8) variant->value.SN32;
			break;

		case VE_UN16:
			CUN32MAX(0xFFFF);
			variant->value.UN16 = (un16) variant->value.UN32;
			break;

		case VE_SN16:
			CSN32MIN(-8000);
			CSN32MAX(0x7FFF);
			variant->value.SN16 = (sn16) variant->value.SN32;
			break;

#if CFG_WITH_FLOAT
		case VE_FLOAT:
			variant->value.Float = (float) variant->value.SN32;
			break;
#endif

		case VE_UN32:
		case VE_SN32:
		default:
			return;
	}

	variant->type.tp = type;
}


/**
 * Conversion from another type to 32bit integer.
 */
void veVariantToN32(VeVariant* variant)
{
	veBool valid = veVariantIsValid(variant);

	// Regular conversion
	switch (variant->type.tp)
	{
	case VE_UN8:
		variant->value.UN32 = variant->value.UN8;
		variant->type.tp = VE_UN32;
		break;

	case VE_SN8:
		variant->value.SN32 = variant->value.SN8;
		variant->type.tp = VE_SN32;
		break;

	case VE_UN16:
		variant->value.UN32 = variant->value.UN16;
		variant->type.tp = VE_UN32;
		break;

	case VE_SN16:
		variant->value.SN32 = variant->value.SN16;
		variant->type.tp = VE_SN32;
		break;

#if CFG_WITH_FLOAT
	case VE_FLOAT:
		variant->value.SN32 = (sn32) variant->value.Float;
		variant->type.tp = VE_SN32;
		break;
#endif

	case VE_UN32:
	case VE_SN32:
		return;

	default:
		// Bit fields are stored as un32.
		if (veVariantIsBits(variant->type.tp)) {
			variant->type.tp = VE_UN32;
			break;
		}

		logE("Variant", "veVariantToN32 unknown type: %d", variant->type);
		return;
	}

	/* if the value was invalid, keep it invalid */
	if (!valid)
		veVariantInvalidate(variant);
}

/// byte length of the current type.
un8 veVariantBufSize(VeVariant* variant)
{
	switch (variant->type.tp)
	{
		case VE_UN8:
		case VE_SN8:
			return 1;

		case VE_UN16:
		case VE_SN16:
			return 2;

		case VE_UN32:
		case VE_SN32:
		case VE_FLOAT:
			return 4;

		default:
			logE("Variant", "veVariantBufSize unknown type: %d", variant->type);
			veAssert(veFalse);
			break;
	}
	return 0;
}

/**
 * Sets the variant to value to indicate it contains no real data.
 */
void veVariantInvalidate(VeVariant* variant)
{
	// Regular conversion
	switch (variant->type.tp)
	{
	case VE_BIT1:
		logE("Variant", "veVariantInvalidate called for a bit....");
		veAssert(veFalse);
		break;

	case VE_UNKNOWN:
		break;

	case VE_UN8:
		invalidateUN8(variant->value.UN8);
		break;

	case VE_SN8:
		invalidateSN8(variant->value.SN8);
		break;

	case VE_UN16:
		invalidateUN16(variant->value.UN16);
		break;

	case VE_SN16:
		invalidateSN16(variant->value.SN16);
		break;

	case VE_UN24:
	case VE_UN32:
		invalidateUN32(variant->value.UN32);
		break;

	case VE_SN32:
		invalidateSN32(variant->value.SN32);
		break;

#if CFG_WITH_FLOAT
	case VE_FLOAT:
		invalidateFloat(variant->value.Float);
		break;

	case VE_FLOAT_ARRAY:
		/* Empty array. Do not set the pointer to NULL! */
		variant->type.len = 0;
		break;
#endif

#if CFG_VARIANT_HEAP
	case VE_HEAP:
	case VE_HEAP_STR:
		veVariantFree(variant);
		break;
#endif

	case VE_BUF:
	case VE_STR:
		variant->value.Ptr = NULL;
		break;

	default:
		{
			un8 n = veVariantIsBits(variant->type.tp);
			if (n) {
				variant->value.UN32 = (un32) (1 << n) - 1;
				return;
			}

			logE("Variant", "veVariantInvalidate: Unknown type!");
			veAssert(veFalse);
		}
	}
}

/** returns if the variant contains valid data */
veBool veVariantIsValid(VeVariant const *variant)
{
	switch (variant->type.tp)
	{
	case VE_UNKNOWN:
		return veFalse;
	case VE_UN8:
		return validUN8(variant->value.UN8);
	case VE_SN8:
		return validSN8(variant->value.SN8);
	case VE_UN16:
		return validUN16(variant->value.UN16);
	case VE_SN16:
		return validSN16(variant->value.SN16);
	case VE_UN32:
		return validUN32(variant->value.UN32);
	case VE_SN32:
		return validSN32(variant->value.SN32);
	case VE_BIT1:
		return veTrue;
#if CFG_WITH_FLOAT
	case VE_FLOAT:
		return validFloat(variant->value.Float);
	case VE_FLOAT_ARRAY:
		return variant->value.Ptr != NULL && variant->type.len > 0;
#endif
	case VE_BUF:
	case VE_HEAP:
	case VE_STR:
	case VE_HEAP_STR:
		return variant->value.Ptr != NULL;
	default:
		{
			un8 n = veVariantIsBits(variant->type.tp);
			if (n)
				return variant->value.UN32 != (un32) (1 << n) - 1;
		}
	}

	logE("Variant", "veVariantIsValid: Unknown type!");
	veAssert(veFalse);
	return veFalse;
}

/** returns if the variant contains is out of range */
veBool veVariantIsOutOfRange(VeVariant *variant)
{
	switch (variant->type.tp)
	{
		case VE_UN8:
			return isOutOfRangeUN8(variant->value.UN8);
		case VE_SN8:
			return isOutOfRangeSN8(variant->value.SN8);
		case VE_UN16:
			return isOutOfRangeUN16(variant->value.UN16);
		case VE_SN16:
			return isOutOfRangeSN16(variant->value.SN16);
		case VE_UN32:
			return isOutOfRangeUN32(variant->value.UN32);
		case VE_SN32:
			return isOutOfRangeSN32(variant->value.SN32);
		default:
			;
	}

	logE("Variant", "veVariantIsOutOfRange: Unknown type!");
	veAssert(veFalse);
	return veFalse;
}

/**
 * Copies the content of the variant to the app variable pointed to by buffer.
 *
 * @return veTrue on success, veFalse otherwise.
 */
veBool veVariantToByteBuffer(VeVariant *variant, void* buffer, un8 bufferLength)
{
	un8 varLength = veVariantBufSize(variant);

	if (varLength > bufferLength)
	{
		logE("Variant", "ERROR: veVariantToByteBuffer: target buffer too small");
		return veFalse;
	}

	(void) memcpy(buffer, variant->value.Buffer, varLength);

	return veTrue;
}

/**
 * Set the variant by copying from the application memory.
 *
 * The type must be set before calling.
 */
void veVariantFromPtr(VeVariant *variant, void const * ptr)
{
	switch(variant->type.tp)
	{
		case VE_SN8:
			variant->value.SN8 = *(sn8 const *) ptr;
			return;

		case VE_UN8:
			variant->value.UN8 = *(un8 const *) ptr;
			return;

		case VE_SN16:
			variant->value.SN16 = *(sn16 const *) ptr;
			return;

		case VE_UN16:
			variant->value.UN16 = *(un16 const *) ptr;
			return;

		case VE_SN24:
		case VE_SN32:
			variant->value.SN32 = *(sn32 const *) ptr;
			return;

		case VE_UN24:
		case VE_UN32:
			variant->value.UN32 = *(un32 const *) ptr;
			return;

#if CFG_WITH_FLOAT
		case VE_FLOAT:
			variant->value.Float = *(float const *) ptr;
			return;
#endif

		case VE_BUF:
		case VE_STR_N2K:
		case VE_STR:
			variant->value.CPtr = ptr;
			return;

#if CFG_VARIANT_HEAP
		case VE_HEAP_STR:
			veVariantHeapStr(variant, ptr);
			return;

		case VE_HEAP:
			veVariantHeap(variant, ptr, variant->type.len);
			return;
#endif

		default:
			break;
	}

	if (veVariantIsCharArray(variant->type.tp)) {
		variant->value.CPtr = ptr;
		return;
	}

	logE("Variant", "veVariantFromPtr: Unknown type!");
	veAssert(veFalse);
}

/**
 * Returns the number of bits of the bit type.
 *
 * @retval The number of bits or 0 if not a bit type.
 */
sn8 veVariantIsBits(VeDataBasicType datatype)
{
	if ((datatype & VE_TP_MASK) == VE_BIT)
		return datatype - VE_BIT0;
	return 0;
}

/**
 * Returns the number of chars in a fixed char type.
 *
 * @retval The number of chars or 0 if not a fixed char type.
 */
sn8 veVariantIsCharArray(VeDataBasicType datatype)
{
	if ((datatype & VE_TP_MASK) == VE_CHARN)
		return datatype - VE_CHAR0;
	return 0;
}

veBool veVariantIsNumeric(VeVariant const *var)
{
	switch (var->type.tp)
	{
	case VE_SN8:
	case VE_UN8:
	case VE_SN16:
	case VE_UN16:
	case VE_SN24:
	case VE_SN32:
	case VE_UN24:
	case VE_UN32:
	case VE_FLOAT:
		return veTrue;
	default:
		if (veVariantIsBits(var->type.tp))
			return veTrue;
		return veFalse;
	}
}

veBool veVariantIsBlob(VeVariant const *var)
{
	switch (var->type.tp)
	{
	case VE_BUF:
	case VE_HEAP:
		return veTrue;
	default:
		return veFalse;
	}
}

veBool veVariantIsEqual(VeVariant const *a, VeVariant const *b)
{
	VeVariant a2, b2;

	if (!veVariantIsValid(a) && !veVariantIsValid(b))
		return veTrue;

	if (!veVariantIsNumeric(a) || !veVariantIsNumeric(b) ) {

		/* String */
		if (((a->type.tp == VE_STR || a->type.tp == VE_HEAP_STR)
			 && (b->type.tp == VE_STR || b->type.tp == VE_HEAP_STR)))
		{
			/* If they point to the same data or both null */
			if (a->value.Ptr == b->value.Ptr)
				return veTrue;
			/* otherwise compare the actual data (if there is) */
			if (!validPtr(a->value.Ptr) || !validPtr(b->value.Ptr))
				return veFalse;
			return strcmp(a->value.Ptr, b->value.Ptr) == 0;
		}

		/* raw buffer */
		if (veVariantIsBlob(a) && veVariantIsBlob(b)) {
			/* unequal length ... */
			if (a->type.len != b->type.len)
				return veFalse;
			/* If they point to the same data or both null */
			if (a->value.Ptr == b->value.Ptr)
				return veTrue;
			/* otherwise compare the actual data (if there is) */
			if (!validPtr(a->value.Ptr) || !validPtr(b->value.Ptr))
				return veFalse;
			return memcmp(a->value.Ptr, b->value.Ptr, a->type.len) == 0;
		}

#if CFG_WITH_FLOAT
		/* Float array */
		if (a->type.tp == VE_FLOAT_ARRAY && b->type.tp == VE_FLOAT_ARRAY) {
			if (!validPtr(a->value.Ptr) || !validPtr(b->value.Ptr))
				return veFalse;
			if (a->type.len != b->type.len)
				return veFalse;
			return memcmp(a->value.Ptr, b->value.Ptr, a->type.len * sizeof(float)) == 0;
		}
#endif

		/* not implemented.. */
		return veFalse;
	}

	a2 = *a;
	b2 = *b;

#if CFG_WITH_FLOAT
	/* don't compare floats without knowing what you do! */
	if (a->type.tp == VE_FLOAT || b->type.tp == VE_FLOAT) {
		veVariantToFloat(&a2);
		veVariantToFloat(&b2);

		/*
		 * note: comparisions of Nans is always false, therefore this
		 * case is handled explicitly.
		 */
		if (!validFloat(a2.value.Float) && !validFloat(b2.value.Float))
			return veTrue;

		return a2.value.Float == b2.value.Float;
	}
#endif

	veVariantToN32(&a2);
	veVariantToN32(&b2);
	return a2.value.UN32 == b2.value.UN32;
}

VeVariant* veVariantSn8(VeVariant *variant, sn8 value)
{
	variant->type.tp = VE_SN8;
	variant->value.SN8 = value;
	return variant;
}

VeVariant* veVariantUn8(VeVariant *variant, un8 value)
{
	variant->type.tp = VE_UN8;
	variant->value.UN8 = value;
	return variant;
}

VeVariant* veVariantSn16(VeVariant *variant, sn16 value)
{
	variant->type.tp = VE_SN16;
	variant->value.SN16 = value;
	return variant;
}

VeVariant* veVariantUn16(VeVariant *variant, un16 value)
{
	variant->type.tp = VE_UN16;
	variant->value.UN16 = value;
	return variant;
}

VeVariant* veVariantSn32(VeVariant *variant, sn32 value)
{
	variant->type.tp = VE_SN32;
	variant->value.SN32 = value;
	return variant;
}

VeVariant* veVariantUn32(VeVariant *variant, un32 value)
{
	variant->type.tp = VE_UN32;
	variant->value.UN32 = value;
	return variant;
}

#if CFG_WITH_FLOAT
VeVariant* veVariantFloat(VeVariant *variant, float value)
{
	variant->type.tp = VE_FLOAT;
	variant->value.Float = value;
	return variant;
}

VeVariant* veVariantFloatArray(VeVariant *variant, float *value, un8 len)
{
	variant->type.tp = VE_FLOAT_ARRAY;
	variant->type.len = len;
	variant->value.Ptr = value;
	return variant;
}
#endif

VeVariant* veVariantBit(VeVariant *variant, un8 n, un32 value)
{
	variant->type.tp = (VeDataBasicType)(VE_BIT0 + n);
	variant->value.UN32 = value;
	return variant;
}

VeVariant* veVariantStr(VeVariant *variant, char const *str)
{
	variant->type.tp = VE_STR;
	variant->value.CPtr = str;
	return variant;
}

VeVariant* veVariantBuf(VeVariant *variant, void const *buf, un8 len)
{
	variant->type.tp = VE_BUF;
	variant->type.len = len;
	variant->value.CPtr = buf;
	return variant;
}

#if CFG_VARIANT_HEAP
#include <stdlib.h>

/* Note: max 255 bytes */
void veVariantAlloc(VeVariant *variant, un8 n)
{
	variant->type.tp = VE_HEAP;
	variant->type.len = n;
	variant->value.Ptr = malloc(n);
}

VeVariant* veVariantHeap(VeVariant *variant, void const *buf, un8 len)
{
	variant->type.len = len;
	variant->type.tp = VE_HEAP;
	variant->value.Ptr = malloc(len);
	if (variant->value.Ptr)
		memcpy(variant->value.Ptr, buf, len);
	return variant;
}

VeVariant* veVariantHeapStr(VeVariant *variant, char const *str)
{
	variant->type.tp = VE_HEAP_STR;
	variant->value.Ptr = strdup(str);
	return variant;
}

void veVariantFree(VeVariant *variant)
{
	switch (variant->type.tp)\
	{
	case VE_HEAP:
	case VE_HEAP_STR:
		free(variant->value.Ptr);
		variant->value.Ptr = NULL;
		break;
	default:
		;
	}
}
#endif
