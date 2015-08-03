#include <string.h>
#include <stdio.h>

#include <velib/velib_config.h>
#include <velib/platform/plt.h>
#include <velib/utils/ve_assert.h>
#include <velib/canhw/canhw.h>
#include <velib/canhw/canhw_i527.h>

/* leaking some platform details, but this is old anyway */
#include <C164CxSx.H>
#include <intrins.h>

#define XP0INT				0x40	//CAN

/* The last mailbox is double buffered */
#define RX_MSG				14

/// Intel 82527 - common
VeCanBusStats veCanBusStats;

// Tx message being monitored
static VeRawCanMsg txMonMsg;

/// init CAN
VE_DCL void veCanInit(void)
{
	un8 n;

	txMonMsg.flags = 0;

	// set the control register for:
	//   - change configuration enable
	//   - initialisation bit set
	I527_CONTROL = 0x41;

	/*  ------------ Bit Timing Register ---------------------
	 * baudrate = 250.000 KBaud
	 * nibbles as TSEG2 TSEG1 SJW BRP, used value is set value + 1
	 */
	I527_BIT_TIME = 0x2F41;
	
	// set global mask to recieve all messages 
	// note: left aligned, right bits are reserved.
	I527_GLOBMASK_STD = 0xFFE0;
	I527_GLOBMASK_EXT = 0xFFFFFFF8;

	// set the message channel 1 (can use tx interrupt)
	canhwBuffer[0].cntrl0 = MSGVAL_R | TXIE_S | RXIE_R | INTPND_R;
	canhwBuffer[0].cntrl1 = RMTPND_R | TXRQST_R | CPUUPD_R | NEWDAT_R;
	canhwBuffer[RX_MSG].mess_config = XTD | DIR_TX;

	// set the message channel 2-14 as disabled, by setting the mesg to invalid (MSGVAL_R)
	for (n = 1; n < 14; n++)
	{
		//canhwBuffer[RX_MSG].mess_config = 0;
		canhwBuffer[n].cntrl0 = MSGVAL_R | TXIE_R | RXIE_R | INTPND_R;
		canhwBuffer[n].cntrl1 = RMTPND_R | TXRQST_R | CPUUPD_R | NEWDAT_R;
	}

	// set up message channel 15 to recieve all 29 bit messages
	I527_MESS15_MSK = 0L;
	canhwBuffer[RX_MSG].mess_config = XTD; /* Excludes 11 bit */
	canhwBuffer[RX_MSG].cntrl0 = MSGVAL_S | TXIE_R | RXIE_S | INTPND_R;
	canhwBuffer[RX_MSG].cntrl1 = RMTPND_R | TXRQST_R | CPUUPD_R | NEWDAT_R;

	// specificy which interrupt flags should be enabled
	I527_CONTROL = I527_CONTROL_IE | I527_CONTROL_EIE | I527_CONTROL_SIE | (1 << INIT);

#if CFG_CANHW_RX_INTERRUPT || CFG_CANHW_TX_INTERRUPT
	// enable CAN interrupt, priority level(ILVL) = 15, group level (GLVL) = 3
	XP0IC = (1 << 6) /* enabled */ | (1 << 2) /* ILVL */ |  1 /* glvl */;
#else
	XP0IC = 0;
#endif
}

void veCanDeInit(void)
{
	XP0IC = 0;			// deinit CAN int.
	I527_CONTROL = 0x41; /* config + init */
}

veBool veCanBusOn(void)
{
	I527_CONTROL &= ~(1 << INIT);
	return veTrue;
}

/**
 * @param buffer the buffer to copy the data from
 * @param out rawCanMsg the message containing the data of buffer
 */
static void veCanRawMsgFromBuf(VeRawCanMsg *rawCanMsg, CanhwBuffer* buffer)
{
	memcpy(rawCanMsg->mdata, buffer->data, 8);			// copy data bytes
	rawCanMsg->length = buffer->mess_config >> 4;		// copy length

	rawCanMsg->canId = buffer->arbitrator[0];			// copy id
	rawCanMsg->canId <<= 8;
	rawCanMsg->canId |= buffer->arbitrator[1];
	rawCanMsg->canId <<= 8;
	rawCanMsg->canId |= buffer->arbitrator[2];
	rawCanMsg->canId <<= 5;
	rawCanMsg->canId |= (buffer->arbitrator[3] >> 3);
}

/// send a message to the bus
VE_DCL veBool veCanSend(VeRawCanMsg *rawCanMsg)
{
	un8 *rd_ptr;
	un8 *wr_ptr;
	un32 canId;

	// check if buffer is empty
	if (canhwBuffer[0].cntrl1 & (TXRQST_S | NEWDAT_S))
		return veFalse;

	// only a single tx msg can be monitored.
	if ((rawCanMsg->flags & VE_CAN_DO_LOOP) != 0)
	{
		if (txMonMsg.flags != 0)
			return veFalse;
		memcpy(&txMonMsg, rawCanMsg, sizeof(txMonMsg));
		if (txMonMsg.flags & VE_CAN_SHOOT) {
			I527_STATUS = 0;
			I527_CONTROL |= I527_CONTROL_SIE;		// enable status interrupt
		}
	}

	// set new data and cpu update flag
	canhwBuffer[0].cntrl1 = RMTPND_R | TXRQST_R | CPUUPD_S | NEWDAT_U;
	wr_ptr = (un8*) &canhwBuffer[0].arbitrator;

	// The CAN-identifier is left aligned, Big Endian in the CAN memory.
	// It is right aligned Little Endian (C166) in the message passed.
	canId = rawCanMsg->canId << 3;
	rd_ptr = (un8 *) &canId + 3;

	*wr_ptr++ = *rd_ptr--;
	*wr_ptr++ = *rd_ptr--;
	*wr_ptr++ = *rd_ptr--;
	*wr_ptr++ = *rd_ptr;

	// set channel to tx and dlc
	*wr_ptr++ = (rawCanMsg->length << 4) | XTD | DIR_TX;
	
	// copy the data of the message
	memcpy(wr_ptr, rawCanMsg->mdata, 8);

	// mark for sending
	canhwBuffer[0].cntrl0 = MSGVAL_S | TXIE_S | RXIE_U | INTPND_R;
	canhwBuffer[0].cntrl1 = RMTPND_U | TXRQST_S | CPUUPD_R | NEWDAT_S;

	return veTrue;
}

/** 
 * poll for a new message.
 *
 * @note 
 *	Should only be called from rx interrupt and therefore always succeeds.
 */

VE_DCL veBool veCanRead(VeRawCanMsg *rawCanMsg)
{
	if (txMonMsg.flags & VE_CAN_IS_LOOP)
	{
		un8 type = rawCanMsg->notused1;
		memcpy(rawCanMsg, &txMonMsg, sizeof(txMonMsg));
		rawCanMsg->notused1 = type;
		txMonMsg.flags = 0;
		return veTrue;
	}

	// the previously stored message is lost.
	if (canhwBuffer[RX_MSG].cntrl1 & MSGLST_S)
		veCanBusStats.hwRxOverflow++;

	/* recover from bus off without status interrupt */
	if (I527_STATUS & BOFF) {
		veCanBusStats.errorBusOff++;
		I527_CONTROL &= ~(1 << INIT);
	}

	if (!(canhwBuffer[RX_MSG].cntrl1 & NEWDAT_S))
		return veFalse;

	/*
	 * note errata AP2924 advices to reset intpnd as early as possible.
	 * so lets do so...
	 */
	canhwBuffer[RX_MSG].cntrl0 = MSGVAL_U | TXIE_U | RXIE_U | INTPND_R;

	veCanRawMsgFromBuf(rawCanMsg, &canhwBuffer[RX_MSG]);
	rawCanMsg->flags = VE_CAN_EXT;

	canhwBuffer[RX_MSG].cntrl1 = RMTPND_R | TXRQST_U | MSGLST_R | NEWDAT_R;

	return veTrue;
}

veBool veCanSendPossible(void)
{
	return (canhwBuffer[0].cntrl1 & TXRQST_S) == 0;
}

// Status and message interrupt.
// Status interrupt is only enable when monitoring of progress is requested.
void CAN_viIsr(void) interrupt XP0INT
{
	un8 kind;

	while ((kind = I527_CPU_IF))
	{
		switch (kind)
		{
			
		case 1:  // Status Change Interrupt
			// The CAN1 controller has updated (not necessarily changed) the status 
			// in the Control Register.
			
			/* reading the status value clears the interrupt flag */

			/*
			 * @note this does obey the 128 idle frames:
			 * After INIT has been cleared, the module will then wait for 129 
			 * occurrences of Bus idle before resuming normal operation. This keeps
			 * the messages in the hardware.
			 */
			if (I527_STATUS & BOFF) {
				veCanBusStats.errorBusOff++;
				I527_CONTROL &= ~(1 << INIT);
			}

			switch (I527_STATUS & LEC_MASK)  // LEC (Last Error Code)
			{
			case 3: // tx: Ack Error
			case 4: // tx: Bit1 Error
			case 5: // tx: Bit0 Error

				// stop trying to send if singleshot mode.
				if (txMonMsg.flags & VE_CAN_SHOOT)
				{
					canhwBuffer[0].cntrl1 = RMTPND_R | TXRQST_R | CPUUPD_R | NEWDAT_R;
					if (txMonMsg.flags & VE_CAN_DO_LOOP)
					{
						txMonMsg.flags |= VE_CAN_IS_LOOP | VE_CAN_TX_ERR;
						I527_CONTROL &= ~I527_CONTROL_SIE;
						veCanRxEvent();
					} else {
						txMonMsg.flags = 0;
					}
				}

				break;

			//case 1: // rx: Stuff Error
			//case 2: // rx: Form Error
			//case 6: // rx: CRC Error
			default:
				break;
			}

			break;				

		case 2: // Message Object 15 Interrupt: Rx
			if (!veCanRxEvent()) {
				canhwBuffer[RX_MSG].cntrl0 = MSGVAL_U | TXIE_U | RXIE_U | INTPND_R;
				canhwBuffer[RX_MSG].cntrl1 = RMTPND_R | TXRQST_U | MSGLST_R | NEWDAT_R;
			}

			break;

		case 3: // Message object 1, Tx
			/* Invalidate the msg, so it cannot be requested by an RTR */
			canhwBuffer[0].cntrl0 = MSGVAL_R | TXIE_U | RXIE_U | INTPND_R;

			if (txMonMsg.flags & VE_CAN_DO_LOOP) {
				I527_CONTROL &= ~I527_CONTROL_SIE;
				txMonMsg.flags |= VE_CAN_IS_LOOP;
				veCanRxEvent();
			}

			veCanTxEvent();
			break;

		default:
			veAssert(veFalse);
			break;		
		}
	}
}
