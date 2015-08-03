#include <string.h>

#include <velib/canhw/canhw.h>
#include <velib/base/base.h>
#include <velib/utils/ve_assert.h>

#include <platform.h>	// application header -> must include mc9s08dxxx.h etc.

/**
 * MSCAN, FreeScale CAN support for (part of) the
 * HC08 / HC12 / S12X / MPC5200 families.
 *
 * Initialisation code is not included since the Freescale tool MMCInit can
 * be used to easily set it up. It is assumed that appropriate masks are set up 
 * to only allow non RTR frames. (will add assertion for this)
 *
 * CFG_CANHW_MSCAN_INTERRUPTS 0 -> polling.
 *	Only veMsCanRead needs to be polled, will also loopback (veMsCanReadTx).
 *
 * CFG_CANHW_MSCAN_INTERRUPTS 1 -> interrupt driven.
 *	veCanRxEvent MUST call veMsCanRead or return veFalse (dropping the message).
 *	veCanTxEvent can call veMsCanReadTx to read a transmitted message back.
 *
 * - Time stamping is not implemented.
 * - No notifications of error passive / bus off states yet.
 * - No error counters (but seems tricky, manual state it is limited to some mode)
 * - Single shot / progress monitoring is not implemented yet, since there is no
 *   straightforward way to do it. The error interrupt is not for a single message,
 *   only for the general CAN bus state, which will only trigger once.
 * - todo: add interrupt attribute..
 * - todo: test
 */

/// For convenience, only set CFG_CANHW_MSCAN_INTERRUPTS to 0 or 1
#define CFG_CANHW_MSCAN_POLL (CFG_CANHW_MSCAN_INTERRUPTS==0)

#if CFG_CANHW_MSCAN_POLL
veBool txIsLoop;
#endif

/**
 * Queue a CAN message for transmission. Only a single TX message is used
 * so message to make sure messages are send in order and the tx interrupt
 * can know which message was sent. */
veBool veMsCanSend(VeRawCanMsg const * const frame)
{
	/* Make sure the TX interrupt is not enabled so the TX message
	 * can be safely written to and the buffer is empty. */
	if (!CANTFLG_TXE0 || CANTIER_TXEIE0)
		return veFalse;
	
#if CFG_CANHW_MSCAN_POLL
	if (txIsLoop)
		return veFalse;	/* do not overwrite message to be read back */
#endif
	
	/* Note: will use only the lowest bit set and read back accordingly. */
	CANTBSEL = 1;

	/* Some bit fiddling to get the CAN identifier in the appropriate format. */
	CANTIDR0 = (un8) ((*(un16*) &frame->canId) >> 5); 		/* ID28 - ID21 */
	CANTIDR1 = ((un8*) &frame->canId)[1] << 3; 		 		/* ID20 - ID18 */
	CANTIDR1 |= (((un8*) &frame->canId)[1] & 0x03) << 1; 	/* ID17 - ID16 */
	if ((((un8*) &frame->canId)[2]) & 0x80)
		CANTIDR1 |= 1;										/* ID15 */
	*((un16*) &CANTIDR2) = ((un16*) &frame->canId)[1] << 1; /* ID14 - ID0 */

	/* Data */
	(void) memcpy(&CANTDSR0, frame->mdata, 8);

	/* DLC */
	CANTDLR_DLC = (un8) frame->length;

	/* Mark message as 29bit extended identifier */
	CANTIDR1 |= CANTIDR1_IDE_MASK | CANTIDR1_SRR_MASK;

	/* Clear the TX-Empty flag to queue for transmission
	 *  note: Writing a 1 clears the flag, 0 is ignored! */
	CANTFLG = 1;
	
#if CFG_CANHW_MSCAN_INTERRUPTS
	/* Enable the TX interrupt to perform local loopback */
	if (frame->flags & VE_CAN_DO_LOOP)
		CANTIER_TXEIE0 = 1;
#else
	/* no interrupts -> Poll the TX empty in read */
	if (frame->flags & VE_CAN_DO_LOOP)
		txIsLoop = veTrue;
#endif

	return veTrue;
}

void veMsCanTxInterrupt(void)
{
	CANTIER_TXEIE0 = 0;
	veCanTxEvent();
}

/**
 * Read a message from the CAN hardware.
 *
 * @retval veTrue if a message was available.
 */
veBool veMsCanReadTx(VeRawCanMsg * const frame)
{
	if (!CANTFLG_TXE0)
		return veFalse;

	/* Note: will use only the lowest bit set and read back accordingly. */
	CANTBSEL = 1;

	/* ID14 - ID0 */
	((un16*) &frame->canId)[1] = (((un16*) &CANTIDR)[1]) >> 1;

	/* ID 15 */
	if (CANTIDR1_ID15)
		((un8*) &frame->canId)[2] |= 0x80;

	/* ID28 - ID18 */
	((un16*) &frame->canId)[0] = (((un16*) &CANTIDR)[0]) >> 3;

	/* ID17 - ID16 */
	((un8*) &frame->canId)[1] &= ~0x03;
	((un8*) &frame->canId)[1] |= (CANTIDR1 >> 1) & 0x03;

	/* Data */
	(void) memcpy(frame->mdata, &CANTDSR0, 8);

	/* DLC */
	frame->length = CANTDLR_DLC;
	if (frame->length > 8)
		frame->length = 8;

	/* Mark message as 29bit if so.. */
	frame->flags = VE_CAN_EXT | VE_CAN_IS_LOOP;

	/* note: Time of acknowledgement can also be enabled.
	 * ...
	 */

	return veTrue;
}

void veMsCanRxInterrupt(void)
{
	/* If the handler cannot read the message, the message is dropped! */
	if (!veCanRxEvent())
		CANRFLG_RXF = 1;
}

/**
 * Read a message from the CAN hardware.
 *
 * @retval veTrue if a message was available.
 */
veBool veMsCanRead(VeRawCanMsg * const frame)
{
	/* Receiver overrun */
	if (CANRFLG_OVRIF==1)
	{
		frame->flags |= VE_CAN_RX_OVERRUN;
	}	
	
	// Check for loopback messages in polling mode. The tx interrupt
#if CFG_CANHW_MSCAN_POLL
	if (txIsLoop && veMsCanReadTx(frame))
	{	
		txIsLoop = veFalse;
		return veTrue;
	}
#endif
	
	if (!CANRFLG_RXF)
		return veFalse;

	/* ID14 - ID0 */
	((un16*) &frame->canId)[1] = (((un16*) &CANRIDR)[1]) >> 1;

	/* ID 15 */
	if (CANRIDR1_ID15)
		((un8*) &frame->canId)[2] |= 0x80;

	/* ID28 - ID18 */
	((un16*) &frame->canId)[0] = (((un16*) &CANRIDR)[0]) >> 3;

	/* ID17 - ID16 */
	((un8*) &frame->canId)[1] &= ~0x03;
	((un8*) &frame->canId)[1] |= (CANRIDR1 >> 1) & 0x03;

	/* Data */
	(void) memcpy(frame->mdata, &CANRDSR0, 8);

	/* DLC */
	frame->length = CANRDLR_DLC;
	if (frame->length > 8)
		frame->length = 8;

	/* Mark message as 29bit if so.. */
	frame->flags = (CANRIDR1_IDE ? VE_CAN_EXT : 0);

	/* note: Time of acknowledgement can also be enabled.
	 * ...
	 */

	/* Done, clear the rx full flag.
	 * note: Writing a 1 clears the flag, 0 is ignored!
	 */
	CANRFLG_RXF = 1;

	return veTrue;
}
