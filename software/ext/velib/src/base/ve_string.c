#include <velib/base/base.h>
#include <velib/base/ve_string.h>

#ifdef _MSC_VER
// MSCV secure versions (_s)
size_t ve_strcat(char* dest, size_t size, char const*src)
{
	strcat_s(dest,size,src);
	return strlen(dest);
}
size_t ve_strcpy(char* dest, size_t size, char const*src)
{
	strcpy_s(dest, size, src);
	return strlen(dest);
}

#elif defined(_UNIX_) || defined(__GNUC__)

// todo: check size
// todo: added __GNUC__ for MinGW (not checked though)
size_t ve_strcat(char* dest, size_t size, char const*src)
{
	VE_UNUSED(size);

	strcat(dest, src);
	return strlen(dest);
}

size_t ve_strcpy(char* dest, size_t size, char const*src)
{
	VE_UNUSED(size);

	strcpy(dest, src);
	return strlen(dest);
}

#endif

/* For platforms that do not have stricmp or a 'special' one */
#if BASE_COMPILER_CUSTOM_STRICMP
# ifndef ve_stricmp /* Could also be a macro */

int ve_stricmp(const char *s1, const char *s2)
{
	while (*s1 && (tolower(*s1) == tolower(*s2)))
		s1++, s2++;
	return tolower(*s1) - tolower(*s2);
}

# endif
#endif
