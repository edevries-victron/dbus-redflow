#ifndef _VELIB_MK2_HAL_H_
#define _VELIB_MK2_HAL_H_

#include <velib/base/types.h>
#include <velib/types/variant.h>

/* baudrates needed for mk2 communication and update */
typedef enum
{
	MK2_BAUD_2400,
	MK2_BAUD_19200,
	MK2_BAUD_115200
} Mk2BaudRate;

void mk2HalInit(void);

/** Must be called on byte arrival */
void mk2RxByte(un8 byte);

/** Seperate init which can be used to setup the gpio pins to the mk2 */
void mk2IoInit(void);

/**
 * These must be implemented by the application.
 * When inReset is true, the mk2 must be powered down.
 * When false it must be released from reset.
 */
void mk2HoldInReset(veBool inReset);

/**
 * with CFG_MK2_TX_BYTES the frame handler will send messages byte
 * byte. mk2FhTxDone should be called after outbyte is send to hw.
 */
void mk2TxByte(un8 outbyte);

/**
 * without CFG_MK2_TX_BYTES the frame handler will send the whole
 * message. mk2FhTxDone should be called after outbyte is send to hw.
 * The buffer remains valid till mk2FhTxDone is called.
 */
void mk2TxBuf(un8 const* buf, un8 len);

/* must be called after a byte or buffer is tranfered */
void mk2FhTxDone(void);

/** start breaking the serial line to the mk2 */
void mk2Break(void);

/** stop breaking the serial line to the mk2 */
void mk2BreakClear(void);

/** change the baudrate of the mk2 communication, @see Mk2BaudRate */
void mk2SetBaudRate(Mk2BaudRate rate);

/**
 * When update is true, incoming data should go to mk2RxUpdateRxByte
 * instead of the default mk2RxByte. When false normal behaviour should
 * be resumed.
 */
void mk2EnableUpdate(veBool update);

/**
 * Communication with a multi is only possible if the mk2 is powered.
 * Hardware lines control this.
 */
void mk2EnsureVebusPower(veBool power);

/**
 * Open the mk2 port.
 * Actually a platform (pc specific call. Embedded device don't have it..
 */
VE_DCL veBool mk2Open(char const* dev);
VE_DCL veBool mk2Close(void);

VeVariant *mk2Instance(VeVariant *var);

#endif
