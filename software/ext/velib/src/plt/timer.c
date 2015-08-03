#include <velib/utils/ve_timer.h>
#include <velib/base/base.h>
#include <velib/platform/plt.h>

#ifdef _UNIX_
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#if defined(_UNIX_) || defined(WIN32)

un16 pltGetCount1ms(void)
{
#ifdef _UNIX_
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (un16) ((1000000 * tv.tv_sec + tv.tv_usec) / 1000);
#endif

#ifdef WIN32
#ifndef CFG_TIMER_WIN32_ACCURATE
	return (un16) GetTickCount();
#else
	LARGE_INTEGER count, freq;
	double seconds;

	QueryPerformanceCounter(&count);
	QueryPerformanceFrequency(&freq);
	seconds = (double) count.QuadPart / freq.QuadPart;
	return (un16) (seconds * 1000);
#endif // CFG_TIMER_WIN32_ACCURATE
#endif // WIN32
}

#endif
