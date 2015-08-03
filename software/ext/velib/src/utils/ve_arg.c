#include <errno.h>
#include <stdio.h>

#include <velib/base/base.h>
#include <velib/utils/ve_arg.h>

// suppress sscanf not secure.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

int veArgInt(char* str)
{
	int ret;

	if (sscanf(str, "0x%X", &ret) == 1)
		return ret;

	if (sscanf(str, "%d", &ret) == 1)
		return ret;

	errno = EINVAL;
	return 0;
}

/** 
 * @brief
 *	Interprets the passed str as a float.
 * @note
 *	Although hex value are supported, the resolution of this type is severly
 *	limited for large values.
 */
float veArgFloat(char* str)
{
	un32 iRet;
	float fRet;

	if (sscanf(str, "0x%X", &iRet) == 1)
		return (float) iRet;

	if (sscanf(str, "%f", &fRet) == 1)
		return fRet;

	errno = EINVAL;
	return 0;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
