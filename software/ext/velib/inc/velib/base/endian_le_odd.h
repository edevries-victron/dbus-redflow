#ifndef _VELIB_BASE_ENDIAN_LE_ODD_H_
#define _VELIB_BASE_ENDIAN_LE_ODD_H_

#include <velib/base/types.h>

/* If there are no alignment issues conversions can be simplified */

/*=== Little Endian ===*/

/* output, ptr might be unaligned. val aligned. They should not equal */
#define cpu_to_le_u16_tp(ptr, val)	(*(un16*)(ptr)) = (val);
#define cpu_to_le_u32_tp(ptr, val)	(*(un32*)(ptr)) = (val);

/* input, ptr might be unaligned. */
#define le_u16_to_cpu_p(ptr)		(*(un16*)(ptr))
#define le_s16_to_cpu_p(ptr)		(*(sn16*)(ptr))
un32 le_u24_to_cpu_p(void* ptr);
#define le_u32_to_cpu_p(ptr)		(*(un32*)(ptr))

/* input, by value, these must be aligned. */
#define le_u16_to_cpu(v)			(v)

/*=== Big Endian ===*/

/* output, ptr might be unaligned. val aligned. They should not equal */
void cpu_to_be_u16_tp(void* ptr, un16 val);
void cpu_to_be_u32_tp(void* ptr, un32 val);

/* input, ptr might be unaligned. */
un16 be_u16_to_cpu_p(void* ptr);
un32 be_u32_to_cpu_p(void* ptr);
#define be_u16_to_cpu(v)	be_u16_to_cpu_p(&(v))

/* by value, these are aligned */
un16 cpu_to_be_u16(un16 val);
un32 cpu_to_be_u32(un32 v);
float cpu_to_float_be(float f);

/* in place modifications */
void to_be_u16(un16* p);
void to_be_float(float* p);

#endif
