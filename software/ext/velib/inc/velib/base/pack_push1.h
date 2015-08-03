/**
 * @ingroup VELIB_BASE
 * 
 * Structure can be closed packed by surrounding them with the pack_push1.h and 
 * pack_pop.h and adding the VE_PACK attribute. Usage should however be limited
 * and preferably not used at all. (some compilers are simply incapable to do so).
 * Structures cannot in general be overlaid with buffers, without causing problems.
 * Some processor will simply trap an exception when accesing non aligned integers.
 * See @ref VE_IO_STREAM for platform independ access to buffers.
 *
 * @code
 * #include "velib/base/pack_push1.h"
 * 
 * typedef struct
 * {
	 un8 value;
	 un16 directlyBehind;
 * } VE_PACK packedStruct;
 *
 * #include "velib/base/pack_pop.h"
 * @endcode
 */

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4103)		// WARN: packing changed after including header file...
#endif								// well.. that's the whole idea, so suppress this warning

#if defined(__C51__)
// default is packed, not possible to change
#elif defined( __C166__) 
#pragma pack(1)
#elif defined(__C30__)
#ifndef VE_PACK
#define VE_PACK __attribute__((packed))
#endif
#else
#pragma pack(push, 1)
#endif


#ifndef VE_PACK
/// attributes to make a structure close packed.
#define VE_PACK
#endif
