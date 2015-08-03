#include <velib/base/endian.h>

un32 le_u24_to_cpu_p(void const* ptr)
{
	un8* p = (un8*) ptr;
	return (un32) p[2] << 16 | p[1] << 8 | p[0];
}
