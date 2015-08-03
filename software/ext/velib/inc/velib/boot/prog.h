#ifndef _VELIB_BOOT_PROG_H_
#define _VELIB_BOOT_PROG_H_

#include <velib/vecan/reg.h>

// can be used to monitor progress but not always needed.
extern un8 progProgress;

void			progBegin(void);
VeRegAckCode	progData(void);
VeRegAckCode	progUpdate(void);

// can be used to monitor progress but not always needed.
extern			un8 progProgress;

#endif
