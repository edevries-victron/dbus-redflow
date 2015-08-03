#include <velib/base/endian.h>

/*
 * These function implemented endianness and dereference of unaligned
 * buffers in a arch independend manner. These function might therefore
 * be slower then an arch specific version.
 */

/* output, ptr might be unaligned. val aligned. They should not equal */
void cpu_to_le_u16_tp(void* ptr, un16 val)
{
	un8 *p = (un8 *) ptr;
	p[0] = (un8) val;
	p[1] = val >> 8;
}

/* input, ptr might be unaligned. */
un16 le_u16_to_cpu_p(void const* ptr)
{
	un8 *p = (un8 *) ptr;
	return p[1] << 8 | p[0];
}

/*
 * @note le_u24_to_cpu_p is in ve_endian_le_24.c since there is no
 * architechture at all which can do this by itself.
 */

/* input, ptr might be unaligned. */
un32 le_u32_to_cpu_p(void const* ptr)
{
	un8 *p = (un8 *) ptr;
	un32 ret = (un32) p[3] << 24 | (un32) p[2] << 16 | p[1] << 8 | p[0];
	return ret;
}

/*
 * There is currently no code in velib using these functions. They are
 * therefore not implemented since there is also no method to test the
 * implementation.
 */

#if 0

/* output, ptr might be unaligned. val aligned. They should not equal */
void cpu_to_le_u32_tp(void* ptr, un32 val) {}

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
