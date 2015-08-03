/** 
 * @addtogroup VELIB_CANHW_ECAN1
 *
 * CAN bus interface Microchip ECAN1 pheriperal with 29bit identifiers.
 *
 * Interrupt free version. Single shot mode is attempted to be implemented, but
 * fails to abort a message in time on a PIC24HJ64GP502. (this seems a limit of the 
 * silicon, since even a single loop is not able to do it).
 *
 * Notifications of outgoing messages is implemented for successfull transmission,
 * and for error during single shot mode. Notification are only returned if the
 * outgoing message is mark with the VE_CAN_NOTIFY flag.
 *
 * The CAN peripheral does survive a buffer underrun but will disregard all pending 
 * frames. A empty message with the flag VE_CAN_MSG_OVERRUN will be returned to indicate
 * such an event.
 *
 * The following constants must be defined:
 * 
 * CFG_CANHW_ECAN_MSG_NUM
 * 	Number of buffers used by ECAN. 1 is used for sending. The remaining
 *  are used as a RX FIFO queue. The hardware must support the number of
 *  buffers. Max 16.
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
 @verbatim
  The following errata is NOT taken into account:
 
  Errata PIC24HJ32GP302/304, PIC24HJ64GPX02/X04 and PIC24HJ128GPX02/X04
  ---------------------------------------------------------------------
  Do not use the DMA with ECAN in Peripheral Indirect mode. Use the DMA in 
  Register Indirect mode, Continuous mode enabled and Ping Pong mode disabled.
  The receive DMA channel count should be set to 8 words. The transmit DMA
  channel count should be set for the actual message size (maximum of 7 words
  for Extended CAN messages and 6 words for Standard CAN Messages). To 
  simplify application error handling while using this mode, only one TX 
  buffer should be used. While message filtering is not affected, messages
  will not be stored at distinct RX buffers. Instead all messages are stored
  contiguously in memory. The start of this memory is pointed to by the 
  receive DMA channel. The application must still clear RXFUL flags and other
  interrupt flags. The application must manage the RX buffer memory.
 
  The following errata is taken into account:
 
  Errata PIC24HJXXXGPX06/X08/X10 A4
  ---------------------------------
  If any buffers other than Buffer 0 are enabled as transmit buffers (i.e., if 
  the TXEN bits other than TXEN0 are set to '1'), incorrect ID and data 
  transmissions will occur intermittently.
  
  Work around: Enable only Buffer 0 for transmission.
 @endverbatim
 */

#include <velib/canhw/canhw.h>
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
#define CTRMCON_TXPRI			0x0003
#define CTRMCON_RTREN			0x0004
#define CTRMCON_TXREQ			0x0008
#define CTRMCON_TXERR			0x0010
#define CTRMCON_TXLARB			0x0020
#define CTRMCON_ABT				0x0040
#define CTRMCON_TXEN			0x0080

//=== CAN buffer definition ===

// get the field value corresponding to the number of buffers used
#define CAN_DMA_BUF_SIZE	_DEFINE_CONCAT(CAN_DMA_BUF_SIZE_, CFG_CANHW_ECAN1_MSG_NUM)

#if !CAN_DMA_BUF_SIZE || CFG_CANHW_ECAN1_MSG_NUM > 16
#error "Number of ECAN message is not supported"
#endif

// The ECAN pheriperal transfers 8 words for incoming messages.
// 7 words are actually used. 7 words should be transferred to
// the ECAN pheriperal for outgoing messages.
#define ECAN_MSG_BUF_WSIZE 			8
#define ECAN_MSG_BUF_WSIZE_USED 	7

typedef un16 ECanBuf[ECAN_MSG_BUF_WSIZE];

volatile VeCanFlags veECan1TxFlags;

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
						DMA0_INTERRUPT_BLOCK & DMA0_NORMAL & 
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
	C1INTFbits.RBOVIF = 0;

	// 1st buffer is TX, others are FIFO rx (resets pointers as well)
	C1FCTRL = C1FCTRL_USED & CAN_DMA_BUF_SIZE & CAN_FIFO_AREA_TRB1;
}

/** 
 *	Creates a filter to allow all extended messages.
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
 * Read a message from the CAN bus.
 */
veBool veECan1Read(VeRawCanMsg * const frame)
{
	un16 id;
	un16* ecanMsg;

	// If an underflow is hit, overflow flags are set per message buffer.
	// The write pointer keeps increasing though (and will thus pass
	// the begin of the round buffer). Especially when all buffers are
	// on overflow it is nearly impossible to get the messages out and
	// reliably synchronize. (the write pointer keeps spinning and there
	// is no way to set it, nor to reliably calculate with it [it might
	// have changed in the meantime]). 
	//
	// The resolution is brutal but easy; all frame are always disregarded 
	// on the first (seen) overflow and message pointers reset. To disable
	// any changes by the module to the buffers, the pheriperal is brought
	// offline during the reset.
	//
	// Any normal operation should not cause underflows anyway!
	if (C1INTFbits.RBOVIF)
	{
		// todo: what happens to the TX message if this happens?
		veECan1Mode(VE_ECAN_MODE_DISABLED);
		veECan1ResetRxFifo();
		DMAECAN1RX(CONbits).CHEN = 0;
		DMAECAN1RX(CONbits).CHEN = 1;
		veECan1Mode(VE_ECAN_MODE_NORMAL);
		frame->flags = VE_CAN_RX_OVERRUN;
		return veTrue;
	}

	// return new rx messages first.
	if (C1RXFUL1)
	{
		// A new message has arrived. Get its index.
 		ecanMsg = eCan1Buf[C1FIFObits.FNRB];
		frame->flags = VE_CAN_EXT;
	} else 
	{
		// tx message queued?
		if (veECan1TxFlags == 0)
			return veFalse;

		// Sending successful
		if (!C1TR01CONbits.TXREQ0)
		{	
			frame->flags = VE_CAN_EXT | VE_CAN_IS_LOOP;
		} 
		
		// stop sending on error if requested to do so.
		else if (veECan1TxFlags & VE_CAN_SHOOT && C1TR01CONbits.TXERR0) 
		{
			frame->flags = VE_CAN_EXT | VE_CAN_IS_LOOP | VE_CAN_TX_ERR;
			C1TR01CONbits.TXREQ0 = 0;
		} 
		
		// sending in progress.
		else 
		{
			return veFalse;
		}

		// Just unlock if no notification is requested.
		if (!(veECan1TxFlags & VE_CAN_DO_LOOP))
		{
			veECan1TxFlags = 0;
			return veFalse;
		}

		// add contents of tx buffer
		ecanMsg = eCan1Buf[0];
	}

	// Get identifier	
	id = ecanMsg[2] >> 10;					// EID 5:0
	id |= ecanMsg[1] << 6;					// EID 15:6
	((un16*) &frame->canId)[0] = id;

	id = (ecanMsg[1] >> 10) & 0b11;			// EID 17:16
	id |= ecanMsg[0] & 0x1FFC;				// SID 10:0
	((un16*) &frame->canId)[1] = id;

	// Get data and data length
	frame->length = (un8)(ecanMsg[2] & 0x000F);
	memcpy(frame->mdata, &ecanMsg[3], 8);

	// All data retrieved, release the buffer.
	if (frame->flags & VE_CAN_IS_LOOP)
		veECan1TxFlags = 0;
	else {
		register un16 mask asm("w0");
		mask = ~(1 << C1FIFObits.FNRB);
		__asm__ __volatile__("and.w C1RXFUL1" : : "r"(mask));		// force atomic AND
	}
	
	return veTrue;
}

/// Stop sending the current tx message.
void veECan1SendAbort(void)
{
	// Abort sending the message.
	C1TR01CONbits.TXREQ0 = 0;
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

	if (C1TR01CONbits.TXREQ0 == 1)
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

	// Mark for transmission.
	veECan1TxFlags = frame->flags | VE_CAN_EXT;	// note: ext should already be set..
	C1TR01CONbits.TXREQ0 = 1;

	return veTrue;
}
