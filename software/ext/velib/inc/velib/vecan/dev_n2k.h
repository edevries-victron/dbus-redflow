#ifndef _VELIB_VECAN_DEV_N2K_H_
#define _VELIB_VECAN_DEV_N2K_H_

#include <velib/J1939/J1939_function.h>

#define VE_DEV_N2K_PROD_INFO			0x0001

#define CFG_VE_DEV_N2K_PROD_STR_LEN		32
#define CFG_VE_DEV_N2K_PROD_STR_SIZE	(CFG_VE_DEV_N2K_PROD_STR_LEN+1)

typedef struct
{
	un16 db;
	un16 code;
	char modelId[CFG_VE_DEV_N2K_PROD_STR_SIZE];
	char firmwareVersion[CFG_VE_DEV_N2K_PROD_STR_SIZE];
	char modelVersion[CFG_VE_DEV_N2K_PROD_STR_SIZE];
	char serial[CFG_VE_DEV_N2K_PROD_STR_SIZE];
	un8 level;
	un8 load;
} VeDevProdInfo;

typedef struct
{
	VeDevProdInfo prod;
	un16 request;
	un16 valid;
} VeDevN2kInfo;

void	veDevN2kUpdate(struct J1939Func* veDev);
void	veDevN2kDataProdInfo(void);
#define veDevN2kReq(dev,f)		(dev->n2k.request |= f)
#define veDevN2kValid(dev,f)	(dev->n2k.valid & f)

#endif
