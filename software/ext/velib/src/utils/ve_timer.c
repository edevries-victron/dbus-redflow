#include <velib/utils/ve_timer.h>
#include <velib/platform/plt.h>

/**
 * @brief
 *	Returns the time difference since last in ms.
 *
 * @details
 *	This function always returns the smallest distance between the two
 *	measurements. This limits the maximum interval to be determined to
 *	approximately 32 sec with an accuracy of 1 ms.
 */
un16 veCircularTimeDiff(un16 last)
{
	un16 diff = pltGetCount1ms() - last;
	return diff < 0x7FFF ? diff : 0xFFFF - diff;
}

/**
 * @brief
 *	Returns veTrue every n milliseconds.
 *
 * @note
 *	last must be set to pltGetCount1ms on init.
 *	ms is a sn16 as a reminder this this works tills MAX_SN16.
 */
veBool veTick1ms(un16* last, sn16 ms)
{
	un16 now = pltGetCount1ms();
	sn16 ticks = (sn16) (now - *last);

	if (ticks < 0)
		ticks = 0xFFFF - ticks; 

	if (ticks >= ms)
	{
		*last = now;
		return veTrue;
	}
	return veFalse;
}
