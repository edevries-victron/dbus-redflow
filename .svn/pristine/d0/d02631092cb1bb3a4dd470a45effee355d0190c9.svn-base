#ifndef _VELIB_TYPES_VE_STAMP_H_
#define _VELIB_TYPES_VE_STAMP_H_

#include <velib/base/types.h>

#define VE_STAMP_MAX_S	1000
#define VE_STAMP_MAX_MS	(VE_STAMP_MAX_S * 1000)
#define VE_STAMP_MAX_US	(VE_STAMP_MAX_S * 1000000)

/* 
 * This is a "decimal" timer, it overflows at 1000 seconds.
 * Useful for console output since it is rather short and
 * it is clear that 999.999 and 000.001 differ by 2ms.
 */
typedef struct {
	un32 us;
} VeStampDec;

/* 
 * An absolute time stamp, this should never overflow.
 * For example by using 0 as process start.
 */
typedef struct {
	un64 us;
} VeStamp;

typedef struct {
	sn64 us;
} VeStampDiff;

void veStampInit(void);
void veStampAdd(VeStampDiff const *diff, VeStamp *time);
void veStampSubstract(VeStampDiff const *diff, VeStamp *time);
void veStampGet(VeStamp *time);
void veStampGetAbs(VeStamp *time);
void veStampDiff(VeStamp const *a, VeStamp const *b, VeStampDiff *diff);
void veStampDiffAbs(VeStamp const *a, VeStamp const *b, VeStamp *diff);
void veStampToDec(VeStamp const *in, VeStampDec *out);
void veStampSync(un8 src, VeStamp* time, un8 tg);
void veStampDecGet(VeStampDec *out);

#endif
