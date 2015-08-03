#ifndef _VELIB_BASE_VE_STRING_H_
#define _VELIB_BASE_VE_STRING_H_

// While all platforms have some function for string manipulation and safe manipulation,
// they are not per definition part of a standard / nor per definition always available.
//
// ve_string is simply a wrapper around these ambigious functions.

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <velib/base/base.h>
#include <velib/velib_config.h>

VE_DCL	size_t ve_strcpy(char* dest, size_t size, char const*src);
VE_DCL	size_t ve_strcat(char* dest, size_t size, char const*src);

#if defined(_UNIX_) || defined(__GNUC__)
#define ve_snprintf(str,s,...)	snprintf(str,s,__VA_ARGS__)
#elif defined(_MSC_VER)
#define strdup(a)					_strdup(a)
#define ve_snprintf(str,s,f,...)	_snprintf_s(str,s,_TRUNCATE,f,__VA_ARGS__)
#endif

#if BASE_COMPILER_CUSTOM_STRICMP
# if defined(_UNIX_)
#  define ve_stricmp(s1,s2)			strcasecmp(s1,s2)
#  define ve_strnicmp(s1,s2,n)		strncasecmp(s1,s2,n)
# elif defined(_MSC_VER)
#  define ve_stricmp(s1,s2)			_stricmp(s1,s2)
# else
VE_DCL	int ve_stricmp(const char *s1, char const *s2);
# endif
#endif

#endif

