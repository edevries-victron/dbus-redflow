#ifndef _VELIB_BASE_ENDIAN_H_
#define _VELIB_BASE_ENDIAN_H_

#if defined(CFG_ARCH_UNLIGNED_DATA_SUPPORTED) && VE_LITTLE_ENDIAN == 1
/* x86 and 8 bitters might derefence all pointers directly */
#include <velib/base/endian_le_odd.h>
#else
/* 
 * If there is no optimized way to handle the endian conversion, fallback to 
 * functions which handle the endian conversions platform independend.
 */
#include <velib/base/endian_generic.h>
#endif

#endif
