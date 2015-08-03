/*
 * Copyright (c) 2012 All rights reserved.
 * Jeroen Hofstee, Victron Energy, jhofstee@victronenergy.com.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

/**
 * @defgroup Str Str
 * @ingroup utils
 * @{
 * @brief Simple string which dynamically allocates more memory if needed.
 *
 * Str.error will be set on failure and the data be freed.
 * Subsequent calls should not cause problems but silently fail, keeping
 * the error flag set. Checking at the end if the string is valid should
 * suffice.
 */

#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <velib/types/ve_str.h>
#include <velib/utils/ve_assert.h>

/* #define _DEBUG_STR */

#ifdef _DEBUG_STR
# define dbg_memset(dst, val, size)		memset(dst, val, size)
#else
# define dbg_memset(dst, val, size)
#endif

char const *veStrInvalid = "invalid str.";

/** Allocates a larger buffer and copies the existing string.
 *
 * @param str		the string
 * @param newSize	the new size of the buffer, must be larger
 *
 */
static void veStrResize(VeStr *str, size_t newSize)
{
	char *tmp;

	if (str->error)
		return;

	if (str->step == 0) {
		veStrFree(str);
		return;
	}

	tmp = realloc(str->data, newSize);
	if (tmp == NULL) {
		veStrFree(str);
		return;
	}

	str->data = tmp;
	str->bufSize = newSize;
}

/** Makes sure that there is space for lengthNeeded characters.
 *
 * @param str the string
 * @param lengthNeeded the length of the string to append
 */
static void veStrFit(VeStr *str, size_t lengthNeeded)
{
	size_t spaceLeft;

	if (str->error)
		return;

	// if the string does not fit, allocate a multiple of step to make it fit
	spaceLeft = str->bufSize - veStrLen(str) - 1;	// 1 for the ending 0
	if (spaceLeft < lengthNeeded)
	{
		if (str->step == 0)
		{
			veStrFree(str);
			return;
		}

		veStrResize(str, str->bufSize + ((lengthNeeded - spaceLeft) / str->step + 1) * str->step);
	}
}

/** returns used length of the string. */
size_t veStrLen(VeStr const *str)
{
	if (str->error)
		return 0;
	return str->veStrLen;
}

/** Zero ended string or invalid string */
char const *veStrCStr(VeStr const *str)
{
	if (str->error)
		return veStrInvalid;

	return str->data;
}

/**
 * returns the end of the buffer or NULL if none
 * @param str the string to initialise
 */
static char *veStrCur(VeStr const *str)
{
	if (str->data == NULL)
		return NULL;

	return str->data + veStrLen(str);
}

/* internal, call after chars are added */
static void veStrAdded(VeStr *str, size_t chars)
{
	str->veStrLen += chars;
#ifdef _DEBUG_STR
	ve_assert(str->veStrlen == strlen(str->data));
#endif
}

/** Initialises the string.
 *
 * A buffer of length is allocated and zero ended or error flag is set.
 *
 * @param str 		the string to initialise
 * @param length 	initial length of the buffer
 * @param step 		number of bytes to expand the buffer with when needed
 *
 * @note should only be called once.
 * @note error flag is set when out of memory
 */
void veStrNew(VeStr *str, size_t size, size_t step)
{
	if (str == NULL)
		return;
	// create new buffer
	str->data = (char*) malloc(size);
	if (str->data == NULL)
	{
		str->error = veTrue;
		return;
	}
	dbg_memset(str->data, 0, size);
	str->data[0] = 0;
	str->step = step;
	str->bufSize = size;
	str->veStrLen = 0;
	str->error = veFalse;
}

/**
 * sprintf with dynamic memory.
 *
 * @note the return value must be released!
 */
void veStrNewFormatV(VeStr *str, char const *format, va_list varArgs)
{
	size_t length;
	va_list varCpy;

	va_copy(varCpy, varArgs);

	/* get the length of the string */
	length = veStrFormatLength(format, varArgs);
	veStrNew(str, length + 1, 100);
	if (str->error)
		return;

	/* now it can safely be formatted */
	vsprintf(str->data, format, varCpy);
	veStrAdded(str, length);
}

/**
 * sprintf with dynamic memory.
 *
 * @note the return value must be released!
 */
void veStrNewFormat(VeStr *str, char const *format, ...)
{
	va_list varArgs;

	// get the length of the string
	va_start(varArgs, format);
	veStrNewFormatV(str, format, varArgs);
	va_end(varArgs);
}

/* Init with a fixed string known to be of length len */
veBool veStrNewFixed(VeStr *str, char const* buf, size_t len)
{
	veStrNew(str, len + 1, len + 1);
	if (str->error)
		return veFalse;
	memcpy(str->data, buf, len);
	str->data[len] = 0;
	veStrAdded(str, len);
	return veTrue;
}

/** Releases the resources
 *
 * @param str the string to release
 */
void veStrFree(VeStr *str)
{
	free(str->data);
	str->data = NULL;
	str->bufSize = 0;
	str->veStrLen = 0;
	str->error = veTrue;
}

/** Sets the string to a value.
 *
 * @param str the string
 * @param value the string to set
 */
size_t veStrSet(VeStr *str, char const *value)
{
	if (str->data == NULL)
		return 0;

	str->data[0] = 0;
	str->veStrLen = 0;
	dbg_memset(str->data, 0, str->bufSize);

	return veStrAdd(str, value);
}

/** Appends a string.
 *
 * @param str the string
 * @param value the string to append
 */
size_t veStrAdd(VeStr *str, char const *value)
{
	size_t len;
	char * p;

	if (value == NULL)
		return 0;

	len = strlen(value);
	veStrFit(str, len);

	if (str->error)
		return 0;

	p = veStrCur(str);
	memcpy(p, value, len);
	p[len] = 0;
	veStrAdded(str, len);
	return len;
}

/** Appends a char.
 *
 * @param str the string
 * @param value the char to append
 */
void veStrAddChr(VeStr *str, char value)
{
	char *p;

	veStrFit(str, 1);
	if (str->error)
		return;

	p = veStrCur(str);
	p[0] = value;
	p[1] = 0;
	veStrAdded(str, 1);
}

/**
 * Appends an integer to the string with specified format.
 *
 * @param str the string
 * @param val the integer to add
 * @param len the length of the char of the integer, e.g. 3 for 100. (max 10, 2147483647)
 * @param fmt the formatter for sprintf
 *
 * @note one digit is added to the length to always allow negative numbers
 * @note if the number doesn't fit ### will be added instead.
 */
size_t veStrAddIntFmt(VeStr *str, int val, size_t len, char const *fmt)
{
	char invalid[] = "###########";
	float max = 1;
	size_t n;
	size_t i;

	if (len > 10)
		return 0;

	n = len;
	while (n--)
		max *= 10;

	if (abs(val) >= max)
	{
		invalid[len] = 0;
		return veStrAdd(str, invalid);
	}

	veStrFit(str, len + 1);	// 1 for sign

	if (str->error)
		return 0;

	i = sprintf(veStrCur(str), fmt, val);
	veStrAdded(str, i);
	return i;
}

/**
 * Appends an integer to the string.
 *
 * @param str the string
 * @param val the integer to add
 * @param len the length of the char of the integer, e.g. 3 for 100. (max 9)
 *
 * @note one digit is added to the length to always allow negative numbers
 * @note if the number doesn't fit ### will be added instead.
 */
size_t veStrAddIntExt(VeStr *str, int val, size_t len)
{
	return veStrAddIntFmt(str, val, len, "%d");
}

/**
 * Appends an integer to the string.
 *
 * @param str the string
 * @param val integer to append
 *
 * @note one digit is added to the length to always allow negative numbers
 */
void veStrAddInt(VeStr *str, int val)
{
	veStrAddIntExt(str, val, 10);
}

/**
 * Appends an integer and string to the string.
 *
 * @param str the string
 * @param val integer to append
 * @param post the string to append
 */
void veStrAddIntStr(VeStr *str, int val, char const *post)
{
	veStrAddInt(str, val);
	veStrAdd(str, post);
}

/** add string number of times */
void veStrRepeat(VeStr *str, char const *value, int number)
{
	int n;
	for (n=0; n<number; n++)
		veStrAdd(str, value);
}

/**
 * Appends a float to the string.
 *
 * @param str the string
 * @param val the float to add
 * @param len the length whole number part in char, e.g. 3 for 100. (max 9)
 * @param dec the number of decimals
 * @param fixed forces the specified size by padding with spaces
 *
 * @note one digit is added to the length to always allow negative numbers
 * @note if the number doesn't fit ### will be added instead.
 */
void veStrAddFloatExt(VeStr *str, float val, size_t len, size_t dec, veBool fixed)
{
	char format[20];
	float frac, absval;
	int fracint;
	size_t n;
	int maxFrac = 1;
	size_t numchars = 0;
	size_t totalLength;

	totalLength = (size_t) (len + dec + 1);

	// determine whole number and decimals
	absval = (float) fabs(val);
	frac = (float) fabs(val);
	frac -= (int) absval;
	for (n=0; n<dec; n++)
	{
		frac *= 10;
		maxFrac *= 10;
	}

	// then round the decimal itself, and if necessary the whole number
	// e.g. 0.999999... should always become 1.0
	fracint = (int) (frac + 0.5);
	if (fracint >= maxFrac)
	{
		absval += 1;
		fracint = 0;
	}

	// only then display... force minus when negative, for e.g. -0.95
	// but not -0.00
	if (val < 0 &&  !((int) absval == 0 && fracint == 0) )
	{
		numchars++;
		veStrAdd(str, "-");
	}

	// report whole number
	numchars += veStrAddIntExt(str, (int) absval, len);
	if (dec == 0)
	{
		if (fixed)
			veStrRepeat(str, " ", (int) (totalLength - numchars));
		return;
	}

	// report decimals as well
	veStrAdd(str, ".");
	sprintf(format, "%%0%" F_SZ "d", dec);
	numchars += veStrAddIntFmt(str, fracint, dec, format);

	if (fixed)
		veStrRepeat(str, " ", (int) (totalLength - numchars));
}

/**
 * Appends a float to the string.
 *
 * @param str the string
 * @param val the float to add
 * @param dec the number of decimals
 *
 * @note one digit is added to the length to always allow negative numbers
 */
void veStrAddFloat(VeStr *str, float val, size_t dec)
{
	veStrAddFloatExt(str, val, 10, dec, veFalse);
}

/**
 * Appends a float to the string.
 *
 * @param str the string
 * @param val the float to add
 * @param dec the number of decimals
 * @param post string to append after the number (e.g. it unit)
 *
 * @note one digit is added to the length to always allow negative numbers
 */
void veStrAddFloatStr(VeStr *str, float val, size_t dec, char const *post)
{
	veStrAddFloatExt(str, val, 10, dec, veFalse);
	veStrAdd(str, post);
}

/**
 * This function behaves like sprintf, except that it appends to a Str.
 *
 * @param str		The Str to which the formatted string will be appended.
 * @param format	The printf style format string for the output.
 * @param ...		Any additional args required by format.
 *
 * @retval	TRUE	The output was appended.
 * @retval	FALSE	It was not possible to append the output.
 */
void veStrAddFormatV(VeStr *str, char const *format, va_list varArgs)
{
	size_t length;
	va_list varCpy;

	va_copy(varCpy, varArgs);
	/* get the length of the string */
	length = veStrFormatLength(format, varCpy);
	va_end(varCpy);

	/* make sure there is memory enough */
	veStrFit(str, length);
	if (str->error)
		return;

	/* now it can safely be added */
	va_copy(varCpy, varArgs);
	vsprintf(veStrCur(str), format, varCpy);
	va_end(varCpy);

	veStrAdded(str, length);
}

void veStrAddFormat(VeStr *str, char const *format, ...)
{
	va_list varArgs;
	va_start(varArgs, format);
	veStrAddFormatV(str, format, varArgs);
	va_end(varArgs);
}

/**
 * Add char urlencoded
 */
void veStrAddUrlEncChr(VeStr *str, char chr)
{
	/* This is not complete, just added when needed */
	if (chr == '#' || chr == '/' || chr == '-' || chr == '_' || chr == '.' || isalnum(chr)) {
		veStrAddChr(str, chr);
		return;
	}

	veStrAddFormat(str, "%%%02X", (unsigned int) chr);
}

/**
 * Add string urlencoded
 */
void veStrAddUrlEnc(VeStr *str, char const *string)
{
	size_t n;

	for (n = 0; n < strlen(string); n++)
		veStrAddUrlEncChr(str, string[n]);
}

/**
 * Adds a variable with a numeric suffix and its value to
 * the string (&varN=value)
 *
 * @note var is not urlencoded, value is
 */
void veStrAddQVarNrStr(VeStr *str, char const *var, int n, char const *value)
{
	veStrAddChr(str, '&');
	veStrAdd(str, var);
	veStrAddInt(str, n);
	veStrAddChr(str, '=');
	veStrAddUrlEnc(str, value);
}

/**
 * Adds a variable with a numeric suffix and its value to
 * the string (&varN=value)
 *
 * @note var and value are not urlencoded
 */
void veStrAddQVarNrInt(VeStr *str, char const *var, int n, sn32 value)
{
	veStrAddChr(str, '&');
	veStrAdd(str, var);
	veStrAddInt(str, n);
	veStrAddChr(str, '=');
	veStrAddInt(str, value);
}

/**
 * Adds a variable with a numeric suffix and its value to
 * the string (&varN=value)
 *
 * @note var is not urlencoded
 */
void veStrAddQVarNrFloat(VeStr *str, char const *var, int n, float value, int dec)
{
	veStrAddChr(str, '&');
	veStrAdd(str, var);
	veStrAddInt(str, n);
	veStrAddChr(str, '=');
	veStrAddFloat(str, value, dec);
}

/**
 * Adds an array variable with a numeric suffix and its value to
 * the string (&varN[]=value)
 *
 * @note var is not urlencoded
 */
void veStrAddQVarArrNrFloat(VeStr *str, char const *var, int n, float value, int dec)
{
	veStrAddChr(str, '&');
	veStrAdd(str, var);
	veStrAddInt(str, n);
	veStrAdd(str, "[]=");
	veStrAddFloat(str, value, dec);
}

/** Adds a variable and value to the string (&var=value)
 *
 * @note var is not urlencoded, value is
 */
void veStrAddQVarStr(VeStr *str, char const *var, char const *value)
{
	veStrAddChr(str, '&');
	veStrAdd(str, var);
	veStrAddChr(str, '=');
	veStrAddUrlEnc(str, value);
}

/** Adds a variable and value to the string (&var=value)
 *
 * @note var is not urlencoded
 */
void veStrAddQVarInt(VeStr *str, char const *var, sn32 value)
{
	veStrAddChr(str, '&');
	veStrAdd(str, var);
	veStrAddChr(str, '=');
	veStrAddInt(str, value);
}

/** Adds a variable and value to the string (&var=value)
 *
 * @note var is not urlencoded
 */
void veStrAddQVarFloat(VeStr *str, char const *var, float value, int dec)
{
	veStrAddChr(str, '&');
	veStrAdd(str, var);
	veStrAddChr(str, '=');
	veStrAddFloat(str, value, dec);
}

/**
 * Returns the number of characters in the string after formatting.
 * Not including the trailing zero.
 *
 * @param format The format string that will be used for the output.
 * @param argptr The arguments to format.
 *
 * @retval The length of the printf output (excluding null terminator).
 */
size_t veStrFormatLength(char const *format, va_list argptr)
{
#if defined(__MSVCRT__) || defined(_MSC_VER)
	/* The MSVC runtime provides the _vscprintf for this purpose. */
	return _vscprintf(format, argptr);
#else
	/*
	 * The ARM_ELF_GCC compiler will return the number for characters
	 * required for the full output of the *nprintf functions, even if the
	 * maximum number of characters to write is specified too low.  However,
	 * the null terminator is always written, so a valid buffer must be
	 * provided even when specifying that 0 characters should be written.
	 */
	char dummy;
	return vsnprintf(&dummy, 0, format, argptr);
#endif
}

void veStrAddChrEscaped(VeStr *str, char c)
{
	switch(c)
	{
	case '\n':
		veStrAdd(str, "\\n");
		break;
	case '\r':
		veStrAdd(str, "\\r");
		break;
	case '"':
		veStrAdd(str, "\\\"");
		break;
	case '\'':
		veStrAdd(str, "\\'");
		break;
	case '\\':
		veStrAdd(str, "\\\\");
		break;
	default:
		veStrAddChr(str, c);
		break;
	}
}

void veStrAddEscaped(VeStr *str, char const* s)
{
	while(*s && !str->error)
		veStrAddChrEscaped(str, *s++);
}

void veStrAddUnescaped(VeStr *str, char const *s)
{
	veStrFit(str, strlen(s));

	while(*s != 0 && !str->error)
	{
		if (*s == '\\')
		{
			switch (*++s)
			{
			case 'n':
				veStrAddChr(str, '\n');
				break;
			case 'r':
				veStrAddChr(str, '\r');
				break;
			case '"':
				veStrAddChr(str, '"');
				break;
			case '\'':
				veStrAddChr(str, '\'');
				break;
			case '\\':
				veStrAddChr(str, '\\');
				break;
			case 0:
				return;
			default:
				veStrAddChr(str, *s);
				break;
			}
		} else {
			veStrAddChr(str, *s);
		}

		s++;
	}
}

/* rm last char */
void veStrRmChr(VeStr *str)
{
	if (!str->veStrLen)
		return;
	str->veStrLen--;
	str->data[str->veStrLen] = 0;
}

// @}
