#ifndef _VELIB_BASE_ENDIAN_GENERIC_H_
#define _VELIB_BASE_ENDIAN_GENERIC_H_

#include <velib/base/types.h>

/*=== Little Endian ===*/

/* output, ptr might be unaligned. val aligned. They should not equal */
void cpu_to_le_u16_tp(void* ptr, un16 val);
void cpu_to_le_u32_tp(void* ptr, un32 val);

/* input, ptr might be unaligned. */
un16 le_u16_to_cpu_p(void const* ptr);
#define le_s16_to_cpu_p(p) (sn16) le_u16_to_cpu_p(p)
un32 le_u24_to_cpu_p(void const* ptr);
un32 le_u32_to_cpu_p(void const* ptr);

/* input, by value, these must be aligned. */
#define le_u16_to_cpu(v)	le_u16_to_cpu_p(&(v))

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
