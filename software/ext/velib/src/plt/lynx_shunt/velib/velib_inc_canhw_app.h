#ifndef __VELIB_INC_CANHW_APP_H
#define __VELIB_INC_CANHW_APP_H

// Freescale
#if defined (__MWERKS__)
#include <velib/canhw/canhw_mscan.h>

#define veCanRead(msg)		veMsCanRead(msg)
#define veCanSend(msg)		veMsCanSend(msg)
#define veCanReadTx(msg)	veMsCanReadTx(msg)

// QT
#elif defined(QT)

#else
#error No CAN hardware
#endif

#endif
