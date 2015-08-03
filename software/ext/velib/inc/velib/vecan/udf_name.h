#ifndef _VELIB_VECAN_UDF_NAME_H_
#define _VELIB_VECAN_UDF_NAME_H_

#include <velib/utils/ve_events.h>

/**
 * @ingroup VELIB_VECAN
 * @defgroup VELIB_VECAN_UDF_NAME UDF Name
 */
/// @{

typedef struct
{
	veBool marked;
} VeUdfVars;

void	veUdfNameInit(void);
void	veUdfNameReq(void);
veBool	veUdfNameSend(void);
void	veUdfNameUpdate(void);
void	veUdfNameHandler(VeEvent ev);

/// @}

#endif
