#ifndef _VELIB_TYPES_VE_VARIANT_H
#define _VELIB_TYPES_VE_VARIANT_H

#include <velib/velib_config.h>
#include <velib/base/base.h>
#include <velib/types/types.h>

 /**
  * @ingroup VELIB_TYPES
  * @defgroup VELIB_TYPES_VARIANT Variant
  */
/// @{

/**
 * All basic types in 4 bytes.
 * Strings need an additional buffer..
 */
typedef union
{
	un8			UN8;
	sn8			SN8;
	un16		UN16;
	sn16		SN16;
	un32		UN32;
	sn32		SN32;
#if CFG_WITH_FLOAT
	float		Float;
#endif
	un8			Buffer[4];
	void		*Ptr;
	void const	*CPtr;
} VeVariantValue;

/**
 * Value and the type of the value.
 */
typedef struct
{
	VeVariantValue		value;
	VeDatatype			type;
} VeVariant;

VE_DCL	sn8		veVariantIsBits(VeDataBasicType datatype);
VE_DCL	sn8		veVariantIsCharArray(VeDataBasicType datatype);
VE_DCL	veBool	veVariantIsNumeric(VeVariant const *var);
VE_DCL	veBool	veVariantIsBlob(VeVariant const *var);

#if CFG_WITH_FLOAT
VE_DCL	void	veVariantToFloat(VeVariant *variant);
VE_DCL	void	veVariantFromFloat(VeVariant *var, VeDataBasicType type);
#endif

VE_DCL	void	veVariantFromN32(VeVariant *variant, VeDataBasicType type);
VE_DCL	void	veVariantToN32(VeVariant *variant);

VE_DCL	veBool	veVariantToByteBuffer(VeVariant *variant, void *buffer, un8 bufferLength);
VE_DCL	void	veVariantFromPtr(VeVariant *variant, void const *ptr);
VE_DCL	un8		veVariantBufSize(VeVariant *variant);

VE_DCL	void	veVariantTransformIn(VeVariant *variant, VeScale const *scale);
VE_DCL	void	veVariantTransformOut(VeVariant *variant, VeScale const *scale);

VE_DCL	void	veVariantInvalidate(VeVariant *variant);
VE_DCL	veBool	veVariantIsValid(VeVariant const *variant);

VE_DCL	veBool	veVariantIsOutOfRange(VeVariant *variant);
VE_DCL	veBool	veVariantIsEqual(VeVariant const *a, VeVariant const *b);

// bits
VE_DCL	void	veVariantFromBitBuffer(VeVariant *variant, void *buffer, un8 bitOffset);
VE_DCL	void	veVariantToBitBuffer(VeVariant *variant, void *buffer, un8 bitOffset);

VE_DCL	VeVariant*		veVariantSn8(VeVariant *variant, sn8 value);
VE_DCL	VeVariant*		veVariantUn8(VeVariant *variant, un8 value);
VE_DCL	VeVariant*		veVariantSn16(VeVariant *variant, sn16 value);
VE_DCL	VeVariant*		veVariantUn16(VeVariant *variant, un16 value);
VE_DCL	VeVariant*		veVariantSn32(VeVariant *variant, sn32 value);
VE_DCL	VeVariant*		veVariantUn32(VeVariant *variant, un32 value);
VE_DCL	VeVariant*		veVariantBit(VeVariant *variant, un8 n, un32 value);
VE_DCL	VeVariant*		veVariantStr(VeVariant *variant, char const *str);
VE_DCL	VeVariant*		veVariantBuf(VeVariant *variant, void const *buf, un8 len);

#if CFG_WITH_FLOAT
VE_DCL	VeVariant*		veVariantFloat(VeVariant *variant, float value);
VE_DCL	VeVariant*		veVariantFloatArray(VeVariant *variant, float *value, un8 len);
#endif

#if CFG_VARIANT_HEAP
/* Note: max 255 bytes */
void					veVariantAlloc(VeVariant *variant, un8 n);
VE_DCL	VeVariant*		veVariantHeap(VeVariant *variant, void const *buf, un8 len);
VE_DCL	VeVariant*		veVariantHeapStr(VeVariant *variant, char const* str);
void					veVariantFree(VeVariant *variant);
#else
#define			veVariantFree(variant)	do {} while(0)
#endif

/// @}

#endif
