#include <stdlib.h>

#include <velib/platform/plt.h>
#include <velib/utils/ve_rand.h>

un8 veRand(void)
{
	srand(pltGetCount1ms());

	return (un8) ((float) rand() / RAND_MAX * VE_RAND_MAX);
}
