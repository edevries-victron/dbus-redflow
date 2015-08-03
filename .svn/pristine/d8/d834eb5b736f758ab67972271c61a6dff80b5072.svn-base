#include <stdio.h>
#include <velib/base/base.h>
#include <velib/types/ve_stamp.h>

static VeStamp startup;

/* Clock skewness, 0 is the local clock */
static VeStampDiff clockDiffs[10];

/* system time */
#ifdef _WIN32
static void veStampGetPlt(VeStamp *time)
{
	LARGE_INTEGER count, freq;

	QueryPerformanceCounter(&count);
	QueryPerformanceFrequency(&freq);
	time->us = count.QuadPart * 1000000 / freq.QuadPart;
}
#elif defined(_UNIX_)
#include <sys/time.h>

static void veStampGetPlt(VeStamp *time)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	time->us = 1000000 * tv.tv_sec + tv.tv_usec;
}
#endif

/* absolute us since startup */
void veStampGetAbs(VeStamp *time)
{
	veStampGetPlt(time);
	time->us -= startup.us;
}

/* Time which can be sync to another clock */
void veStampGet(VeStamp *time)
{
	veStampGetAbs(time);
	time->us += clockDiffs[0].us;

	//printf("get %lld + %lld = %lld\n", time->us - clockDiffs[0].us, clockDiffs[0].us, time->us);
}

/* Startup with 0 clock value */
void veStampInit(void)
{
	veStampGetPlt(&startup);
}

/* time = time - diff */
void veStampSubstract(VeStampDiff const *diff, VeStamp *time)
{
	time->us -= diff->us;
}

/* diff = a - b */
void veStampDiff(VeStamp const *a, VeStamp const *b, VeStampDiff *diff)
{
	diff->us = a->us - b->us;
}

/* Absolute difference between to stamps */
void veStampDiffAbs(VeStamp const *a, VeStamp const *b, VeStamp *diff)
{
	diff->us = a->us - b->us;
	if (diff->us > INT64_MAX)
		diff->us = a->us - b->us;
}

/* decimal looping counter, for log routines etc */
void veStampToDec(VeStamp const *in, VeStampDec* out)
{
	VeStamp tmp = *in;

	tmp.us %= VE_STAMP_MAX_US;
	out->us = (un32) tmp.us;
}

void veStampSync(un8 src, VeStamp* time, un8 tg)
{
	VeStamp now;
	VE_UNUSED(tg);

	/* The src clock is per definition in sync */
	clockDiffs[src].us = 0;

	/* get the target clock, for now only local */
	veStampGetAbs(&now);
	veStampDiff(time, &now, &clockDiffs[0]);

	//printf("src %lld tg %lld diff %lld\n", time->us, now.us, clockDiffs[0]);
}

void veStampDecGet(VeStampDec *out)
{
	VeStamp now;

	veStampGet(&now);
	veStampToDec(&now, out);
}
