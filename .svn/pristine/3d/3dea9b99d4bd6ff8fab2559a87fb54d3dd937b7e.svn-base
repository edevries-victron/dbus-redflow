#include <velib/base/base.h>
#include <velib/utils/ve_timer.h>

#ifdef _UNIX_
#include <unistd.h>
#endif

#ifdef WIN32
#include <stdio.h>
#endif

#ifdef WIN32
// WIN32 doesn't have a microsecond sleep....
// Just waste the cpu time...
void veWaitWin32(un32 microseconds)
{
	LARGE_INTEGER ticksPerSecond;
	LARGE_INTEGER start, now;   // A point in time
	LONGLONG diff;

	if (microseconds >= 0x7FFF)
		microseconds = 0x7FFE;

	QueryPerformanceFrequency(&ticksPerSecond);
	QueryPerformanceCounter(&start);

	for (;;)
	{
		QueryPerformanceCounter(&now);
		if (now.QuadPart >= start.QuadPart)
			diff = now.QuadPart - start.QuadPart;
		else
			diff = (MAXLONGLONG - start.QuadPart) + now.QuadPart;

		if ( 1E6 * (double) diff / (double) ticksPerSecond.QuadPart >= microseconds)
			break;
	}
}
#endif

void veWait(un32 microseconds)
{
#ifdef _UNIX_
	usleep(microseconds);
#endif

#ifdef WIN32
	if (microseconds > 5000)
		Sleep(microseconds / 1000);
	else
		veWaitWin32(microseconds);
#endif

}
