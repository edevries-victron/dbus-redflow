/** 
 * @addtogroup VELIB_CANHW_ECAN1
 *
 * CAN bus interface Microchip ECAN1 pheriperal with 29bit identifiers.
 *
 * Interrupt version. Single shot mode is implemented.
 *
 * Notifications of outgoing messages is implemented for successfull transmission,
 * and for error during single shot mode. Notification are only returned if the
 * outgoing message is marked with the VE_CAN_DO_LOOP flag.
 *
 * The following constants must be defined:
 * 
 * CFG_CANHW_ECAN_MSG_NUM
 * 	Number of buffers used by ECAN. 1 is used for sending. The remaining
 *  are used as a RX FIFO queue. The hardware must support the number of
 *  buffers.
 *
 * CFG_CANHW_ECAN_DMA_TX
 * 	DMA channel to use for outgoing messages, e.g. DMA0.
 * 
 * CFG_CANHW_ECAN_DMA_RX
 *	DMA channel to use for outgoing messages, e.g. DMA1.
 * 
 * CFG_CANHW_ECAN1_PRE_SCALER
 * CFG_CANHW_ECAN1_PROP_TQ
 * CFG_CANHW_ECAN1_SEG1_TQ
 * CFG_CANHW_ECAN1_SEG2_TQ
 * CFG_CANHW_ECAN1_SJW_TQ (optional)
 *	CAN-bus timing related fields in time quantas. 
 *
 * The following errata is NOT taken into account:
 *
 * Errata PIC24HJ32GP302/304, PIC24HJ64GPX02/X04 and PIC24HJ128GPX02/X04
 * ---------------------------------------------------------------------
 * Do not use the DMA with ECAN in Peripheral Indirect mode. Use the DMA in 
 * Register Indirect mode, Continuous mode enabled and Ping Pong mode disabled.
 * 
 *  .... Since the workaround does not work. There are new silicon releases 
 * available which do not contain this bug.
 *
 * The following errata is taken into account:
 *
 * Errata PIC24HJXXXGPX06/X08/X10 A4
 * ---------------------------------
 * If any buffers other than Buffer 0 are enabled as transmit buffers (i.e., if 
 * the TXEN bits other than TXEN0 are set to '1'), incorrect ID and data 
 * transmissions will occur intermittently.
 * 
 * Work around: Enable only Buffer 0 for transmission.
 */

#include <velib/canhw/canhw.h>
#include <velib/utils/ve_todo.h>
#include <velib/utils/ve_assert.h>

#include "string.h"
#include "ecan.h"
#include "dma.h"


// Use maximum SJW, when not defined.
#ifndef CFG_CANHW_ECAN1_SJW_TQ
#define CFG_CANHW_ECAN1_SJW_TQ	(CFG_CANHW_ECAN1_SEG2_TQ - 1)
#endif

#if CFG_CANHW_ECAN1_SEG2_TQ == 1 || CFG_CANHW_ECAN1_SJW_TQ >= CFG_CANHW_ECAN1_SEG2_TQ
#error "SJW should be less then SEG2."
#endif

#if CFG_CANHW_ECAN1_PROP_TQ + CFG_CANHW_ECAN1_SEG1_TQ < CFG_CANHW_ECAN1_SEG2_TQ
#error "PROP + SEQ1 must be greater or equal to SEG2"
#endif

// DMA selection
#define DMAECAN1TX(f)	_DEFINE_CONCAT(CFG_CANHW_ECAN1_DMA_TX, f)
#define DMAECAN1RX(f)	_DEFINE_CONCAT(CFG_CANHW_ECAN1_DMA_RX, f)

//=== Some additions to ecan.h ===

// the SID fields also contain the last 2bits of the EID
#define CAN_FILTER_SID_EIDBITS(x)	(0xFFFC | ((un32)(x) >> 16))

// if unused bits are set, the value is set incorrectly
#define C1FCTRL_USED				0xE01F

// set SJW by number of TQ	
#define CAN_SYNC_JUMP_WIDTH(x)		((((x-1) & 0x3) << 6) | 0x3F)

#define IDE			1		// extended identifier
#define SRR 		2		// 11-bit remote request, 29bit, must be set

// Fields of CiTRmCON (note the  m, 0, 2 ,4 etc)
#define CTRMCON_TXPRI		0x0003
#define CTRMCON_RTREN		0x0004
#define CTRMCON_TXREQ		0x0008
#define CTRMCON_TXERR		0x0010
#define CTRMCON_TXLARB		0x0020
#define CTRMCON_ABT			0x0040
#define CTRMCON_TXEN		0x0080

// CxINTE flags (for OR-ing)
#define MASK_TBIE           0x0001 /* TX buffer interrupt */
#define MASK_RBIE           0x0002 /* RX buffer interrupt */
#define MASK_RBOVIE         0x0004 /* RX buffer interrupt */
#define MASK_FIFOIE         0x0008 /* FIFO almost full */
/*                          0x0010     UNIMPLEMENTED */
#define MASK_ERRIE          0x0020 /* Error */
#define MASK_WAKIE          0x0040 /* Wakeup */
#define MASK_IVRIE          0x0080 /* Invalid message */

//=== CAN buffer definition ===

// get the field value corresponding to the number of buffers used
#define CAN_DMA_BUF_SIZE	_DEFINE_CONCAT(CAN_DMA_BUF_SIZE_, CFG_CANHW_ECAN1_MSG_NUM)

#if !CAN_DMA_BUF_SIZE
#error "Number of ECAN message is not supported"
#endif

#if CFG_CANHW_ECAN1_DMA_TX == DMA1
#define _DMATxInterrupt 	_DMA1Interrupt
#define DMATXIE				IEC0bits.DMA1IE
#define DMATXIF				IFS0bits.DMA1IF
#elif CFG_CANHW_ECAN1_DMA_TX == DMA2
#define _DMATxInterrupt 	_DMA2Interrupt
#define DMATXIE				IEC0bits.DMA2IE
#define DMATXIF				IFS0bits.DMA2IF
#else
#error "CAN DMA tx interrupt flags unknown"
#endif

// The ECAN pheriperal transfers 8 words for incoming messages.
#define ECAN_MSG_BUF_WSIZE 			8

typedef un16 ECanBuf[ECAN_MSG_BUF_WSIZE];

// Flags of the current message being transmitted..
volatile VeCanFlags 	veECan1TxFlags;

// DMA buffers for ECAN ,aligned(16)
ECanBuf	eCan1Buf[CFG_CANHW_ECAN1_MSG_NUM]__attribute__((space(dma)));

/** 
 * Setup the basic parameters for the ECAN pheriheral. 
 * 
 * Timing / DMA and FIFO is initialised. Filtering is not set.
 *
 * @note assumes that it is only called during startup.
 */
void veECan1Init(void)
{
	veECan1TxFlags = 0;

	// Configure CAN baud rate.
	C1CFG1 = 	CAN_BAUD_PRE_SCALE(CFG_CANHW_ECAN1_PRE_SCALER) & 
				CAN_SYNC_JUMP_WIDTH(CFG_CANHW_ECAN1_SJW_TQ);

	// Configure CAN bit timing.
	C1CFG2 = 	CAN_WAKEUP_BY_FILTER_DIS & CAN_SAMPLE1TIME & CAN_SEG2_FREE_PROG &
			 	CAN_PROPAGATIONTIME_SEG_TQ(CFG_CANHW_ECAN1_PROP_TQ) &
				CAN_PHASE_SEG1_TQ(CFG_CANHW_ECAN1_SEG1_TQ) & 
				CAN_PHASE_SEG2_TQ(CFG_CANHW_ECAN1_SEG2_TQ);

	// 1st buffer is TX, others are FIFO rx.
	veECan1ResetRxFifo();

	// buffer 0 is TX
	C1TR01CON = CAN_BUFFER0_IS_TX & CAN_AUTOREMOTE_DISABLE_BUFFER0 & 
				CAN_TX_HIGH_PRI_BUFFER0 & CAN_ABORT_REQUEST_BUFFER0 &
				CAN_BUFFER1_IS_RX & CAN_AUTOREMOTE_DISABLE_BUFFER1 & 
				CAN_TX_HIGH_PRI_BUFFER1 & CAN_ABORT_REQUEST_BUFFER1 &
				CAN_BUFFER2_IS_RX & CAN_AUTOREMOTE_DISABLE_BUFFER2 &
				CAN_TX_HIGH_PRI_BUFFER2 & CAN_ABORT_REQUEST_BUFFER2 &
				CAN_BUFFER3_IS_RX & CAN_AUTOREMOTE_DISABLE_BUFFER3 & 
				CAN_TX_HIGH_PRI_BUFFER3 & CAN_ABORT_REQUEST_BUFFER3;
	
	// Configure DMA for TX, the register are expended by the preprocessor
	// e.g. DMATX(PAD) becomes DMA0PAD when CFG_CANHW_ECAN_DMA_TX is DMA0
	DMAECAN1TX(PAD) = (volatile un16) &C1TXD;			// address to write to
 	DMAECAN1TX(CNT) = ECAN_MSG_BUF_WSIZE - 1;
	DMAECAN1TX(REQ) = 0x0046;							// transfer on TXREQ
	DMAECAN1TX(STA) = __builtin_dmaoffset(eCan1Buf);	// address of the data
	DMAECAN1TX(CON) = 	DMA0_MODULE_ON & DMA0_SIZE_WORD & DMA0_TO_PERIPHERAL & 
						DMA0_INTERRUPT_HALF & DMA0_NORMAL & 
						DMA0_PERIPHERAL_INDIRECT & DMA0_CONTINUOUS;

	// Configure DMA for RX
	// The buffers are stored contiguesly from buffer 1, looping after the last
	// buffer has been written to.
	DMAECAN1RX(PAD) = (volatile un16) &C1RXD;
	DMAECAN1RX(CNT) = ECAN_MSG_BUF_WSIZE - 1;
	DMAECAN1RX(REQ) = 0x0022;
	DMAECAN1RX(STA) = __builtin_dmaoffset(eCan1Buf);
	DMAECAN1RX(CON) = 	DMA0_MODULE_ON & DMA0_SIZE_WORD & PERIPHERAL_TO_DMA0 & 
						DMA0_INTERRUPT_BLOCK & DMA0_NORMAL & 
						DMA0_PERIPHERAL_INDIRECT & DMA0_CONTINUOUS;

	C1INTE = MASK_ERRIE | MASK_RBOVIE | MASK_RBIE | MASK_TBIE;
	C1INTF = 0;
	IEC2bits.C1IE = 1;	
}

#ifndef CFG_CANHW_ECAN1_DEINIT_DISABLE
void veECan1Deinit(void)
{
	IEC2bits.C1IE = 0;	
	veECan1Mode(VE_ECAN_MODE_CONFIG);
	DMAECAN1TX(CONbits).CHEN = 0;
	DMAECAN1RX(CONbits).CHEN = 0;
	IFS0 = 0;
	IFS1 = 0;
	IFS2 = 0;
	IFS3 = 0;
	IFS4 = 0;
}
#endif

/** 
 * @brief
 *	Init / Reset FIFO queue pointers.
 * @details
 *	Used during initialisation, but also to recover from an overrun.
 * @note 
 *	Do not call when the CAN module is active.
 */
void veECan1ResetRxFifo(void)
{
	C1RXOVF1 = 0;
	C1RXFUL1 = 0;
	C1RXFUL2 = 0;
	C1INTFbits.RBOVIF = 0;
	C1INTFbits.RBIF = 0;

	// 1st buffer is TX, others are FIFO rx (resets pointers as well)
	C1FCTRL = C1FCTRL_USED & CAN_DMA_BUF_SIZE & CAN_FIFO_AREA_TRB1;
}

/** 
 * Creates a filter to allow all extended messages.
 */
void veECan1AcceptExt(void)
{
	C1CTRL1bits.WIN = 1;						// enable filter init

	// only filter on extended bit, not on the identifier itself
	C1RXM0SID = CAN_FILTER_SID(0) & CAN_MATCH_FILTER_TYPE & CAN_FILTER_SID_EIDBITS(0);
	C1RXM0EID = 0;
	C1RXF0SID = CAN_RX_EID_EN;
	C1BUFPNT1 = CAN_FILTER0_RX_BUFFER_FIFO;		// transfer matches to FIFO queue
	C1FEN1 = 0b1;								// only filter 0 is enabled

	C1CTRL1bits.WIN	= 0;						// end of filter init
}

/** 
 * @brief
 * 	Change operational mode and wait for switch.
 * @details
 * 	See header / documentation for available modes.
 */
void veECan1Mode(un8 mode)
{
	C1CTRL1bits.REQOP = mode;
	while(C1CTRL1bits.OPMODE != mode);
}

/**
 * Sets the ECAN contents of buffer n to msg.
 */
void veECan1BufToMsg(un8 n, VeRawCanMsg * msg)
{
	un16 id;
	un16* ecanBuf;

	// Get the data belonging to buffer n.
	ecanBuf = eCan1Buf[n];

	// Get identifier	
	id = ecanBuf[2] >> 10;					// EID 5:0
	id |= ecanBuf[1] << 6;					// EID 15:6
	((un16*) &msg->canId)[0] = id;

	id = (ecanBuf[1] >> 10) & 0b11;			// EID 17:16
	id |= ecanBuf[0] & 0x1FFC;				// SID 10:0
	((un16*) &msg->canId)[1] = id;

	// Get data and data length
	msg->length = (un8)(ecanBuf[2] & 0x000F);
	// Do not allow lengths longer then 8 bytes.
	if (msg->length > 8)
		msg->length = 8;
	memcpy(msg->mdata, &ecanBuf[3], 8);
}

/** 
 * Read a message from the CAN bus.
 */
veBool veECan1Read(VeRawCanMsg * const msg)
{
	// return loopback message.
	if (veECan1TxFlags & VE_CAN_IS_LOOP)
	{
		veECan1BufToMsg(0, msg);
		msg->flags = veECan1TxFlags;
		veECan1TxFlags = 0;
		return veTrue;
	}

	// nothing todo if all buffers empty.
	if (!C1RXFUL1)
	{
#if CFG_CANHW_ECAN1_MSG_NUM == 32
		if (!C1RXFUL2)
#endif
		{
			return veFalse;
		}
	}

	veECan1BufToMsg(C1FIFObits.FNRB, msg);
	msg->flags = VE_CAN_EXT;

#if CFG_CANHW_ECAN1_MSG_NUM == 32
	// short for between 16 up to and including 31
	if (C1FIFObits.FNRB & 0x30)
	{
		// & 0xF = short for -16
		register un16 mask asm("w0");
		mask = ~(1 << (C1FIFObits.FNRB & 0xF));
		__asm__ __volatile__("and.w C1RXFUL2" : : "r"(mask) : "cc");		// force atomic AND
		return veTrue;
	}
#endif

	{
		register un16 mask asm("w0");
		mask = ~(1 << C1FIFObits.FNRB);
		__asm__ __volatile__("and.w C1RXFUL1" : : "r"(mask) : "cc");			// force atomic AND
	}
	return veTrue;
}

/// Stop sending the current tx message.
void veECan1SendAbort(void)
{
	// Abort sending the message.
	C1TR01CONbits.TXREQ0 = 0;
	DMATXIE = 0;
	C1INTEbits.IVRIE = 0;

	if (veECan1TxFlags & VE_CAN_DO_LOOP)
	{
		// notify when requested...
		veECan1TxFlags = VE_CAN_EXT | VE_CAN_IS_LOOP | VE_CAN_TX_ERR;
	} else {
		// otherwise just release the message.
		veECan1TxFlags = 0;
	}
}

/** 
 * Send a CAN message. 
 *
 * @note 
 * 	During single shot mode veECan1Read must be called on a regular basis.
 */
veBool veECan1Send(VeRawCanMsg const * const frame)
{
	un16 id;

	if (veECan1TxFlags != 0)
	{
		// Hardware is busy sending a message.
		return veFalse;
	}

	// set identifier and DLC
	id = ((un16*) &frame->canId)[1];
	eCan1Buf[0][0] = (id & 0x1FFC) | IDE | SRR;
	eCan1Buf[0][1] = id << 10;

	id = ((un16*) &frame->canId)[0];
	eCan1Buf[0][1] |= id >> 6;
	eCan1Buf[0][2] = (id << 10) | frame->length;

	// set data.
	memcpy(&eCan1Buf[0][3], frame->mdata, 8);

	// Remember flags of message (locks tx buffer as well).
	veECan1TxFlags = frame->flags | VE_CAN_EXT;
	
	// Enable DMA interrupt to catch moment when transmission actually start,
	// when there is a need to monitor progress.
	if (veECan1TxFlags & VE_CAN_SHOOT)
	{
		DMATXIF = 0;
		DMATXIE = 1;
	}

	C1TR01CONbits.TXREQ0 = 1;

	return veTrue;
}

// CAN bus interrupt.
void __attribute__ ((interrupt,auto_psv)) _C1Interrupt(void)
{
	IFS2bits.C1IF = 0;

	/* RX buffer overflow */
	if (C1INTFbits.RBOVIF)
	{
		veCanErrorEvent(VE_CAN_RX_OVERRUN);
		veECan1ResetRxFifo();

		// C1INTFbits.RBOVIF = 0; // already reset by veECan1ResetRxFifo ...
	}

	/* Reception */
	else if(C1INTFbits.RBIF)
	{
		C1INTFbits.RBIF = 0;

		veCanRxEvent();
		veTodo();
	}
	/* Transmission */
	else if(C1INTFbits.TBIF)
	{
		C1INTFbits.TBIF = 0;

		if (veECan1TxFlags & VE_CAN_DO_LOOP)
		{
			veECan1TxFlags = VE_CAN_EXT | VE_CAN_IS_LOOP;
			veCanTxEvent();
		} else {
			veECan1TxFlags = 0;
		}
		
		veTodo();
	}
	/* Invalid frame received (including own frames) */
	else if (C1INTFbits.IVRIF)
	{
		C1INTFbits.IVRIF = 0;

		// Normally the CAN handles transmission errors by itself and this can 
		// be ignored. In single shot mode automatic retransmission is canceled 
		// though and a notification might have been requested by setting the 
		// VE_CAN_DO_LOOP flag. Otherwise the frame is simply ignored.

		if (veECan1TxFlags & VE_CAN_SHOOT)
		{
			C1TR01CONbits.TXREQ0 = 0;				// stop retransmission
			if (veECan1TxFlags & VE_CAN_DO_LOOP)
			{
				// notify the caller that sending was not succesfull.
				veECan1TxFlags = VE_CAN_EXT | VE_CAN_IS_LOOP | VE_CAN_TX_ERR;
				veCanTxEvent();
			} else {
				// just ignore the failure to send
				veECan1TxFlags = 0;
			}

			veTodo();
		}
	}

	// BUS off and friends
	else if (C1INTFbits.ERRIF)
	{
		C1INTFbits.ERRIF = 0;

#if 0
		/* note: not all flags can be cleared */

		/* Transmitter Bus Off */
		if (C1INTFbits.TXBO) {
			C1INTFbits.TXBO = 0;
 		} else if (C1INTFbits.TXWAR) {
			C1INTFbits.TXWAR = 0;
		/* Transmitter Error passive */
		} else if (C1INTFbits.TXBP) {
			//veCanErrorEvent(VE_CAN_ERROR_PASSIVE);
			C1INTFbits.TXBP = 0;
		}

		/* Receiver Error passive */
		if (C1INTFbits.RXBP) {
			C1INTFbits.RXBP = 0;
		/* Receiver Bus Off warning */
		} else if (C1INTFbits.RXWAR) {
			C1INTFbits.RXWAR = 0;
		}
#endif

	}

	/* Wake */
	else if (C1INTFbits.WAKIF)
	{
		C1INTFbits.WAKIF = 0;
	}
	/* FIFO almost full */
	else if (C1INTFbits.FIFOIF)
	{
		C1INTFbits.FIFOIF = 0;

	/* No interrupt flag set... should not occur */
	} else {
		veAssert(veFalse);
		IFS2bits.C1IF = 0;
	}
	
	// Trigger the interrupt again if an interrupt source is still active.
	if ((C1INTF & 0x7F) != 0)
		IFS2bits.C1IF = 1;
}

/** 
 * Support for Single Shot mode...
 *
 * The ECAN interface does not support single shot mode and will retransmit the
 * message by itself. In order to disable the automatic retranmission the 
 * transmission is actively aborted.
 */
void __attribute__((interrupt,auto_psv)) _DMATxInterrupt(void) 
{
	// Currently sending, enable the error interrupt to monitor..
	C1INTFbits.IVRIF = 0;
	C1INTEbits.IVRIE = 1;

	DMATXIE = 0;
	DMATXIF = 0;
}
