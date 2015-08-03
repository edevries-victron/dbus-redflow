#ifndef _VELIB_UTILS_VE_ASSERT_H_
#define _VELIB_UTILS_VE_ASSERT_H_

#include <velib/velib_config.h>
#include <velib/base/base.h>

/** 
 * @addtogroup VELIB_UTILS
 * @details
 * <b>veAssert</b>
 *
 * When @ref CFG_ASSERT_ENABLED is defined veAssert will check debug assertion.
 * On Windows and unixes this will call the regular assert. On other platform 
 * @ref veAssertFailed will be called.
 */
/// @{

#if CFG_ASSERT_ENABLED

# if defined(WIN32) || defined(_UNIX_)

#  include <assert.h>
#  define veAssert(expr) assert(expr)

# elif defined(CFG_ASSERT_FILE)
// embedded can define any method to output the assertions
#  define veAssert(expr) 													\
	if (!(expr))															\
		veAssertFailedNum(__FILE__, __LINE__);
void veAssertFailedNum(char const* file, un16 line);

# else
// embedded can define any method to output the assertions
#  define veAssert(expr) 													\
	if (!(expr))															\
		veAssertFailedNum(0, __LINE__);

void veAssertFailedNum(un16 file, un16 line);
# endif

#else
#  define	veAssert(expr)
#endif

/// @}

#endif
