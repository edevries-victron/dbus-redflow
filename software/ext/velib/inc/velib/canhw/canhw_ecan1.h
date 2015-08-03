#ifndef _VELIB_CANHW_ECAN1_H_
#define _VELIB_CANHW_ECAN1_H_

/**
 * @ingroup VELIB_CANHW
 * @defgroup VELIB_CANHW_ECAN1	Microchip ECAN
 * @brief 
 *	Standardized calls for Microchip ECan, first channel.
 * @{
 */

#include <velib/canhw/canhw_types.h>

#define veECan1TxFree()				(veECan1TxFlags == 0)

// Modes for veECan1Mode
#define VE_ECAN_MODE_NORMAL			0
#define VE_ECAN_MODE_DISABLED		1
#define VE_ECAN_MODE_LOOPBACK		2
#define VE_ECAN_MODE_LISTEN_ONLY	3
#define VE_ECAN_MODE_CONFIG			4

extern volatile VeCanFlags veECan1TxFlags;

void		veECan1AcceptExt(void);
void		veECan1Init(void);
void		veECan1Deinit(void);
void		veECan1Mode(un8 mode);
veBool		veECan1Read(VeRawCanMsg * const rawCanMsg);
void		veECan1ResetRxFifo(void);
veBool		veECan1Send(VeRawCanMsg const * const frame);

void		veECan1SendAbort(void);
veBool		veECan1TxEmpty(void);

/*
#define		VE_ECAN1_INT_DISABLE()	(IEC2bits.C1IE = 0)
#define		VE_ECAN1_INT_ENABLE()	(IEC2bits.C1IE = 1)

#define		VE_CAN_INT_SAFE(a)			\
{										\
	un8 C1IEStored;						\
	C1IEStored = IEC2bits.C1IE;			\
	a									\
	IEC2bits.C1IE = C1IEStored;			\
}
*/

/// @}

#endif
