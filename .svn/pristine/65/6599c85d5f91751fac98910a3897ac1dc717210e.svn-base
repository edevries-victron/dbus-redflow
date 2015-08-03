#ifndef _VELIB_UTILS_LOGGER_H_
#define _VELIB_UTILS_LOGGER_H_

#include <velib/velib_config.h>
#include <velib/base/base.h>
#include <stdarg.h>

#if CFG_WITH_LOG_PROTO

veBool logInit(char const* filename);
VE_DCL void veLogLevel(un8 level);
VE_DCL void veLogLimit(sn32 lines);
VE_DCL void veLogBackBufferLines(sn32 lines);
VE_DCL void veLogLinesAfterTrigger(sn32 lines);
VE_DCL void veLogBackBuffer(void);
VE_DCL void logE(char const* module, char const* format, ...);	// ERROR
VE_DCL void logErrorVa(char const* module, char const* format, va_list lst);
VE_DCL void logW(char const* module, char const* format, ...);	// WARNING
VE_DCL void logI(char const* module, char const* format, ...);	// INFO
VE_DCL void logInfoVa(char const* module, char const* format, va_list lst);

#else

# if BASE_COMPILER_MARCRO_NARGS
// preprocessor support variadic marcos, politely remove the traces
#  define logE(...) do {} while(0)
#  define logW(...) do {} while(0)
#  define logI(...) do {} while(0)
# else
// no varargs support, brutally remove the entries by commenting them
#  define logE BASE_COMPILER_MACRO_REMOVE
#  define logW BASE_COMPILER_MACRO_REMOVE
#  define logI BASE_COMPILER_MACRO_REMOVE
# endif // BASE_COMPILER_MARCRO_NARGS
#endif // CFG_LOG_DISABLE

#endif
