#ifndef _VELIB_UTILS_TIMER_H_
#define _VELIB_UTILS_TIMER_H_

#include <velib/velib_config.h>
#include <velib/base/base.h>
#include <velib/platform/plt.h>

/*	This function always returns the smallest distance between the two
*	measurements. This limits the maximum interval to be determined to
*	approximately 32 sec with an accuracy of 1 ms.
*/

un16 veCircularTimeDiff(un16 last);

veBool veTick1ms(un16* last, sn16 ms);

/* Stop execution for AT LEAST n microseconds */
VE_DCL void veWait(un32 microseconds);

/* veWait actually implemented? */
#if defined(_UNIX_) || defined(WIN32)
#define CFG_HAVE_VE_WAIT                1
#endif

#endif
