#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <velib/base/types.h>
#include <velib/base/ve_string.h>
#include <velib/platform/console.h>
#include <velib/types/ve_stamp.h>
#include <velib/types/ve_str.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_timer.h>

static char *logFile;
static un8 logLevel = 1;

static sn32 logLimit = -1;	/* default to no limit */
static sn32 logLimitLevel;
static sn32 logLimitLinesAfter;
static char **backLog;
static char **backLogEnd;	/* Entry _after_ the backlog! */
static char **backLogLine;	/* Next log line, *line can be NULL */

veBool logInit(char const* filename)
{
	if (logFile)
		free(logFile);
	logFile = NULL;
	if (filename)
		logFile = strdup(filename);
	return veTrue;
}

void veLogLevel(un8 level)
{
	logLevel = level;
}

/* Limit logging to the next n lines */
void veLogLimit(sn32 lines)
{
	logLimit = lines;
}

#if 0
static char **copy;
static size_t copyLength;

static void checkInit(sn32 lines)
{
	copyLength = lines * sizeof(const char*);
	copy = calloc(lines, sizeof(const char*));
}

static void check(void)
{
	if (memcmp(backLog, copy, copyLength) != 0) {
		printf("corrupt buffer!\n");
		pltExit(-1);
	}
}

static void checkSync(void)
{
	memcpy(copy, backLog, copyLength);
}
#else

static void checkInit(sn32 lines) {VE_UNUSED(lines); }
static void check(void) {}
static void checkSync(void) {}

#endif

/*
 * Keep the last log lines in memory. This is usefull to
 * only show many details in case some interesting occured.
 */
void veLogBackBufferLines(sn32 lines)
{
	/* destroy existing if any */
	if (backLog) {
		char **p = backLog;

		while (*p && p != backLogEnd)
			free(*p++);
		free(backLog);
		backLog = backLogLine = backLogEnd = NULL;
	}

	/* zero disable back logging */
	if (lines == 0)
		return;

	/* force log level up to keep a backlog of all traces */
	logLimitLevel = logLevel;
	logLevel = 3;
	logLimit = 0;

	backLog = calloc(lines, sizeof(const char*));
	if (!backLog)
		return;
	backLogEnd = backLog + lines - 1;
	backLogLine = backLog;

	checkInit(lines);
}

/* set the number of log lines display after a error or warning */
void veLogLinesAfterTrigger(sn32 lines)
{
	logLimitLinesAfter = lines + 1;
}

void veLogTriggerDump(void)
{
	pltInterruptAssertDisabled(1);

	veLogBackBuffer();
	veLogLimit(logLimitLinesAfter);
}

/*
 * This method will dump and reset the existing backbuffer.
 * Combined with veLogLimit this allows to verbose around an
 * event of interest.
 */
void veLogBackBuffer(void)
{
	char **line;

	pltInterruptAssertDisabled(1);

	if (backLog == NULL || *backLog == NULL)
		return;

	printf("\n-------- dumping backlog -------\n");

	/* find a decent start line, depending on whether the buffer wrapped around */
	line = (*backLogEnd ? backLogLine : backLog);
	while (*line != NULL)
	{
		/* print and free */
		printf("%s", *line);
		free(*line);
		*line = NULL;

		/* next line */
		if (++line > backLogEnd)
			line = backLog;
	}

	backLogLine = backLog;
}

static void	strAddTime(VeStr *s)
{
	VE_UNUSED(s);

#if defined(CFG_LOG_STAMP_DEC)
	{
		VeStampDec now;

		/* The stamp might be synchronized to an external device */
		veStampDecGet(&now);
		veStrAddFormat(s, "%03d.%03d ", now.us / 1000000, (now.us / 1000) % 1000);

# if defined(CFG_LOG_STAMP_UNCORRECTED_TOO)
		{
			/* For debugging it can be usefull to compare it with the local clock */
			VeStamp pc;
			veStampGetAbs(&pc);
			veStampToDec(&pc, &now);
			veStrAddFormat(s, "- %03d.%03d ", now.us / 1000000, (now.us / 1000) % 1000);
		}
# endif
	}
#endif
}

static void	logit(char const* module, char const* type,	char const* format, va_list argp)
{
	VeStr s;

	if (logLimit == 0 && backLog == NULL)
		return;

	pltInterruptSuspend();

	veStrNew(&s, 1024, 1024);
	strAddTime(&s);
	veStrAddFormat(&s, "%s.%s: ", type, module);
	veStrAddFormatV(&s, format, argp);
	veStrAdd(&s, "\n");

	/* show the log message if requested */
	if (logLimit != 0) {
		FILE *fh = 0;

		if (logFile)
			fh = fopen(logFile, "a");
		if (fh == 0)
			fh = stdout;

		fprintf(fh, "%s", veStrCStr(&s));
		fflush(fh);
		veStrFree(&s);

		if (fh != stdout)
			fclose(fh);

	} else {
		if (!s.error) {
			check();

			/* replace current line */
			free(*backLogLine);
			*backLogLine = s.data;

			/* wrap around if needed */
			if (++backLogLine > backLogEnd)
				backLogLine = backLog;

			checkSync();
		}
	}

	if (logLimit > 0)
		logLimit--;

	pltInterruptResume();
}

// ERROR
VE_DCL void logErrorVa(char const* module, char const* format, va_list lst)
{
	if (logLevel < 1)
		return;

	logit(module, "ERR", format, lst);
}

// ERROR
#if CFG_WITH_LOG_PROTO
VE_DCL void logE(char const* module, char const* format, ...)
{
	va_list	argp;

	if (logLevel < 1)
		return;

	/* only trigger a dump, when not logging unconditionally */
	if (logLimit != -1)
		veLogTriggerDump();

	va_start(argp, format);
	logit(module, "ERR", format, argp);
	va_end(argp);
}
#endif

// WARNINGS
#if CFG_WITH_LOG_PROTO
VE_DCL void logW(char const* module, char const* format, ...)
{
	va_list	argp;

	if (logLevel < 2)
		return;

	if (logLimitLevel >= 2)
		veLogTriggerDump();

	va_start(argp, format);
	logit(module, "WRN", format, argp);
	va_end(argp);
}
#endif

VE_DCL void logInfoVa(char const* module, char const* format, va_list lst)
{
	if (logLevel < 3)
		return;

	logit(module, "INFO", format, lst);
}

// INFO
#if CFG_WITH_LOG_PROTO
VE_DCL void logI(char const* module, char const* format, ...)
{
	va_list	argp;

	if (logLevel < 3)
		return;

	va_start(argp, format);
	logit(module, "INF", format, argp);
	va_end(argp);
}
#endif
