#ifndef _VELIB_MK2_UPDATE_H_
#define _VELIB_MK2_UPDATE_H_

#include <velib/mk2/mk2.h>

// version contained in image
extern un8 const mk2UpdateVersion;

void mk2FirmwareUpgrade(Mk2ResultCallback* callback);
void mk2FirmwareTick(void);

/* Incoming data should be passed here while updating */
void mk2RxUpdateRxByte(un8 inbyte);

/* 50 ms time tick */
void mk2FirmwareTick(void);

veBool mk2UpdateBusy(void);

#endif
