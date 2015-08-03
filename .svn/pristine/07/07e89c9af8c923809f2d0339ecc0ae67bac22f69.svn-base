#ifndef _VELIB_VECAN_REG_H_
#define _VELIB_VECAN_REG_H_

#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_function.h>
#include <velib/vecan/regs.h>

/**
 * @ingroup VELIB_VECAN
 * @defgroup VELIB_VECAN_REG Vreg
 */
/// @{

/// Fast Packet sequence counter for 0x1EF00
extern un8		veRegFpSeq;

/*
 * Use veRegPrepareSfNoRetry when the caller is not retrying by itself,
 * veRegPrepareSf otherwise (normal case)
 */
veBool		veRegPrepareSfRetryArg(J1939Nad tg, VeRegId regId, veBool noRetry);
#define		veRegPrepareSfNoRetry(tg, regId)	veRegPrepareSfRetryArg(tg, regId, veTrue)
#define		veRegPrepareSf(tg, regId)			veRegPrepareSfRetryArg(tg, regId, veFalse)
veBool		veRegPrepareSfWhenOnBus(struct J1939Func* tg, VeRegId regId);

void		veRegPrepareFp(J1939Nad tg, VeRegId regId);

/* see veRegPrepareSfNoRetry */
veBool 		veRegSendAckRetryArg(J1939Nad tg, VeRegId regId, VeRegAckCode code, veBool noRetry);
#define		veRegSendAckNoRetry(tg, regId, cd)	veRegSendAckRetryArg(tg, regId, cd, veTrue)
#define		veRegSendAck(tg, regId, cd)			veRegSendAckRetryArg(tg, regId, cd, veFalse)

veBool		veRegRequestMask(J1939Nad tg, VeRegId regId, VeRegId mask);
veBool		veRegRequest(J1939Nad tg, VeRegId regId);

veBool		veRegRequestMaskWhenOnBus(struct J1939Func* dev, VeRegId regId, VeRegId mask);
veBool		veRegRequestWhenOnBus(struct J1939Func* dev, VeRegId regId);
/// @}

#endif
