#ifndef _VELIB_MK2_VSC_H_
#define _VELIB_MK2_VSC_H_

#include <velib/io/ve_stream.h>

#define VSC_FOOTER_LEN		23
#define VSD_FOOTER_LEN		15
#define VSI_FOOTER_LEN		18

/// Versions of VS* files
#define VSC_FILE_VERSION	1
#define VSD_FILE_VERSION	8
#define VSI_FILE_VERSION	4

/// Number of cells 	
#define VSC_10080_CELL_SYS	10080
#define VSC_12V_SYS			6
#define VSC_24V_SYS			12
#define VSC_48V_SYS			24

/// Default voltages
#define DEFAULT_VOLTAGE_US	120
#define DEFAULT_VOLTAGE_EU	230

typedef enum {
	VE_VSC_FILE_TOO_SHORT,
	VE_VSC_VSD_END_NOT_WITHIN_FILE,
	VE_VSC_NO_COMBINED_FILE,
	VE_VSC_NO_DATA_FILE,
	VE_VSC_VERSION_UNKNOWN,
	VE_VSD_VERSION_UNKNOWN,
	VE_VSC_OK
} VebusVscErrNo;

typedef struct {
	struct VebusDeviceS* dev;
	un16 vsdFooter;
	un16 vsiBegin;	/* vsdFooterEnd */
	un16 vsiFooter;
	un16 vscFooter; /* vsiFooterEnd */
	un8 workaRound;
} VebusVsc;

VebusVscErrNo vebusVscOpenStream(VebusVsc* vsc, struct VebusDeviceS* dev, VeStream* stream, un8 *data, un16 size);
void vebusVscReadData(VebusVsc const* vsc, un16* data, un8 n);
void vebusVscReadSettingInfo(VebusVsc const* vsc, VebusSettingInfo* info, un8 n);
un32 vebusVsdVersion(VebusVsc const* vsc);
un32 vebusVscVersion(VebusVsc const* vsc);

void vebusCreateVsc(VeStream* stream, struct VebusDeviceS *dev, un8* buf, un16 len);

#endif
