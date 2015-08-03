#ifndef _VELIB_BOOT_HEADER_H_
#define _VELIB_BOOT_HEADER_H_

#include <velib/base/types.h>

#define BOOT_APP_IMAGE		0

#define BOOT_KEYLOADER		0xFFFE
#define BOOT_BOOTLOADER		0xFFFD

typedef struct
{
	un16 manufacturer;
	un16 length;         /* Of this struct */
	un16 productId;
	un16 type;
	un32 version;
	un32 minUdfVersion;
} VupCommonHeader;

#endif
