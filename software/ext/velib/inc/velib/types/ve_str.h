#ifndef _VELIB_TYPES_VE_STR_H_
#define _VELIB_TYPES_VE_STR_H_

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

#include <stdarg.h>
#include <velib/base/base.h>

/// Simple string (builder) which dynamically allocates memory
typedef struct
{
	char *	data;			///< the actual string
	size_t	bufSize;		///< the buffer size
	size_t	step;			///< step size to increase the buffer with
	size_t	veStrLen;		///< length of the string in the buffer
	veBool	error;			///< indicates a failure of a operation on the string
} VeStr;

extern char const *veStrInvalid;

void veStrNew(VeStr *str, size_t size, size_t step);
void veStrNewFormat(VeStr *str, char const *format, ...);
void veStrNewFormatV(VeStr *str, char const *format, va_list varArgs);
veBool veStrNewFixed(VeStr *str, char const* buf, size_t len);
size_t veStrSet(VeStr *str, char const* value);
char const * veStrCStr(VeStr const *str);
size_t veStrLen(VeStr const *str);
void veStrFree(VeStr *str);

size_t veStrAdd(VeStr *str, char const *value);
void veStrAddChr(VeStr *str, char value);
void veStrAddInt(VeStr *str, int val);
void veStrAddIntStr(VeStr *str, int val, char const *post);
size_t veStrAddIntFmt(VeStr *str, int val, size_t len, char const *fmt);
size_t veStrAddIntExt(VeStr *str, int val, size_t dec);
void veStrAddFormat(VeStr *str, char const *format, ...);
void veStrAddFormatV(VeStr *str, char const *format, va_list varArgs);
void veStrAddFloat(VeStr *str, float val, size_t dec);
void veStrAddFloatExt(VeStr *str, float val, size_t len, size_t dec, veBool fixed);
void veStrAddFloatStr(VeStr *str, float val, size_t dec, char const *post);
void veStrAddUrlEnc(VeStr *str, char const *string);
void veStrAddUrlEncChr(VeStr *str, char chr);

void veStrAddQVarNrStr(VeStr *str, char const *var, int n, char const* value);
void veStrAddQVarNrInt(VeStr *str, char const *var, int n, sn32 value);
void veStrAddQVarNrFloat(VeStr *str, char const *var, int n, float value, int dec);
void veStrAddQVarArrNrFloat(VeStr *str, char const *var, int n, float value, int dec);
void veStrAddQVarStr(VeStr *str, char const *var, char const *value);
void veStrAddQVarInt(VeStr *str, char const *var, sn32 value);
void veStrAddQVarFloat(VeStr *str, char const *var, float value, int dec);

void veStrAddChrEscaped(VeStr *str, char c);
void veStrAddEscaped(VeStr *str, char const* s);
void veStrAddUnescaped(VeStr *str, char const *s);

void veStrRmChr(VeStr *str);

size_t veStrFormatLength(char const *format, va_list argptr);

#endif
