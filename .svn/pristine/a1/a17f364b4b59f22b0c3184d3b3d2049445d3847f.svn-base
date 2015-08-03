#ifndef _VELIB_VECAN_DEV_UDF_H_
#define _VELIB_VECAN_DEV_UDF_H_

#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_function.h>
#include <velib/utils/ve_events.h>

#define VE_DEV_VUP_PROD_ID		0x01
#define VE_DEV_VUP_VER_MIN		0x02
#define VE_DEV_VUP_VER			0x04

// Properties of the product (part) being updated.
typedef struct _VeDevVup
{
	struct _VeDevVup* next;

	un8				instance;
	J1939Name		udfName;
	J1939Nad		nadUdf;				//< device function which handles the update.
	un32			appVersion;
	un32			appVersionMin;
	un16			prodId;
	un8				prodFlags;
	un8				flags;
} VeDevVup;


#define VE_DEV_UDF_REQ			0x01
#define VE_DEV_UDF_VER_VALID	0x02	// and the udfFlags
#define VE_DEV_GROUP_VALID		0x04

typedef struct
{
	un32		udfVersion;
	un8			udfFlags;
	un8			group;
	VeDevVup*	vup;
	un8			flags;
} VeDevUdfVars;

void	veDevUdfUpdate(struct J1939Func* veDev);
void	veDevUdfDataVeRegs(VeEvent ev);
#define veDevUdfReq(dev) {dev->udf.flags |= VE_DEV_UDF_REQ;}

VeDevVup* veDevVupAlloc(void);
VeDevVup* veDevUdfFindVup(struct J1939Func* veDev, un8 instance);

#endif
