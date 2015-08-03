/** @addtogroup	CAN hardware */
//@{

#include <string.h>

#include <LPC17xx.h>

#include <velib/base/base.h>
#include <velib/canhw/canhw_lpc17xx.h>

bool veCanRxEvent(void);

static un32 can1BitRate = BITRATE250K25MHZ_3; // default
static veBool can1MsgCheck;
static RawCanMsg can1MsgTx;
static volatile CanMsgFlags can1MsgTxFlags;

static veBool can2MsgCheck;
static RawCanMsg can2MsgTx;
static volatile CanMsgFlags can2MsgTxFlags;
static un32 can2BitRate = BITRATE250K25MHZ_3; // default

/** @brief	Initialization of both CAN channels
 *
 */
void lpcCanInit(void)
{
	lpcCanInitChannel(0);
	lpcCanInitChannel(1);

	can1MsgTxFlags = 0;
	can2MsgTxFlags = 0;

	// set acceptance filter (receive all messages)
	LPC_CANAF->AFMR = ACCF_BYPASS;

	can1MsgCheck = veFalse;
	can2MsgCheck = veFalse;
}

/** @brief	Initialization of the CAN hardware
 *
 * 	@param	channel	CAN hardware channel
 * 	@return	True if success and False if failed
 */
veBool lpcCanInitChannel(un8 channel)
{
	if (channel >= CAN_MAX_PORTS)
		return veFalse;

	switch (channel)
	{
	case 0:
		LPC_SC->PCONP |= (1<<13);  // Enable CAN1 clock

		// CAN1 is p0.0 and p0.1
		LPC_PINCON->PINSEL1 &= ~0x00003C00;
		LPC_PINCON->PINSEL1 |= 0x00003C00;

		LPC_CAN1->MOD = 1;		// Reset CAN
		LPC_CAN1->IER = 0;		// Disable Receive Interrupt
		LPC_CAN1->GSR = 0;		// Reset error counter when CANxMOD is in reset

		LPC_CAN1->BTR = can1BitRate;	// set CAN bitrate
		LPC_CAN1->MOD = 0x0;			// CAN in normal operation mode

		LPC_CAN1->IER = (/*INT_RX |*/ INT_ERR_WARN | INT_DATA_OVR | INT_ERR_PASS | INT_BUS_ERR /* | INT_ARB_LOST */ ); // Enable interrupts
		break;

	case 1:
		LPC_SC->PCONP |= (1<<14);  // Enable CAN2 clock

		// CAN2 is p2.7 and p2.8
		LPC_PINCON->PINSEL4 &= ~0x0003C000;
		LPC_PINCON->PINSEL4 |= 0x00014000;

		LPC_CAN2->MOD = 1;		// Reset CAN
		LPC_CAN2->IER = 0;		// Disable Receive Interrupt
		LPC_CAN2->GSR = 0;		// Reset error counter when CANxMOD is in reset

		LPC_CAN2->BTR = can2BitRate;	// set CAN bitrate
		LPC_CAN2->MOD = 0x0;			// CAN in normal operation mode

		LPC_CAN2->IER = (INT_TX1 | INT_ERR_WARN | INT_DATA_OVR | INT_ERR_PASS | INT_BUS_ERR /*| INT_ARB_LOST */ ); // Enable interrupts
		break;

	default:
		return veFalse;
	}

	NVIC_EnableIRQ(CAN_IRQn);	// enable interrupts

	return veTrue;
}

typedef struct
{
	un32 busoff;
	un32 errorstat;

	un32 errorWarn;
	un32 dataOverrun;
	un32 errorPassive;
	un32 arbLost;
	un32 busError;
	un32 rxErrCnt;
	un32 txErrCnt;
} tCanHwStatus;

volatile tCanHwStatus canHwStatus1;
volatile tCanHwStatus canHwStatus2;

#define CAN_HW_INT_ERROR_CNT1(i,reg) 	do { if (canIcr1 & i) canHwStatus1.reg++ } while(0)
#define CAN_HW_INT_ERROR_CNT2(i,reg) 	do { if (canIcr2 & i) canHwStatus2.reg++ } while(0)

typedef struct
{
	__IO un32 rbs:1;
	__IO un32 dos:1;
	__IO un32 tbs:1;
	__IO un32 tcs:1;
	__IO un32 rs:1;
	__IO un32 ts:1;
	__IO un32 es:1;
	__IO un32 bs:1;
	__IO un32 dummy:8; // 16
	__IO un32 rxerr:8; // 24
	__IO un32 txerr:8; // 32
} tCanHwGsr;

#define canGsr1 ((tCanHwGsr *) 0x40044008)
#define canGsr2 ((tCanHwGsr *) 0x40048008)

/**
 * CAN interrupt handler
 */
static void CAN_IRQHandler(void)
{
	un32 canIcr1 = LPC_CAN1->ICR; // read is clearing interrupt
	un32 canIcr2 = LPC_CAN2->ICR;

	// receive interrupt channel 1
	if (canIcr1 & INT_RX)
	{
		//veCanRxEvent();
	}

	// channel 1
	if (canIcr1 & (INT_ERR_WARN | INT_DATA_OVR | INT_ERR_PASS | INT_ARB_LOST | INT_BUS_ERR))
	{
		CAN_HW_INT_ERROR_CNT1(INT_ERR_WARN,errorWarn);
		CAN_HW_INT_ERROR_CNT1(INT_DATA_OVR,dataOverrun);
		CAN_HW_INT_ERROR_CNT1(INT_ERR_PASS,errorPassive);
		CAN_HW_INT_ERROR_CNT1(INT_ARB_LOST,arbLost);
		CAN_HW_INT_ERROR_CNT1(INT_BUS_ERR,busError);

		if (canGsr1->bs)
		{
			canHwStatus1.busoff = 1;

			// clear buss-off
			canGsr1->txerr = 0;
			LPC_CAN1->MOD &= ~(1 << 0); // reset CAN Hardware (clear RM bit in the MOD register)
		}

		if (canGsr1->es)
			canHwStatus1.errorstat = 1;

	}

	if (canIcr2 & INT_TX1)
		canHwStatusEvent(CAN_HW_ERROR_NONE); // reset hardware errors

	// channel 2
	if (canIcr2 & (INT_ERR_WARN | INT_DATA_OVR | INT_ERR_PASS | INT_ARB_LOST | INT_BUS_ERR))
	{
		if (canIcr2 & INT_ERR_WARN)
		{
			canHwStatus2.errorWarn++;
			canHwStatusEvent(CAN_HW_ERROR_WARNING_LIMIT);
		}

		if (canIcr2 & INT_ERR_PASS)
		{
			canHwStatus2.errorPassive++;
			canHwStatusEvent(CAN_HW_ERROR_WARNING_LIMIT);
		}

		CAN_HW_INT_ERROR_CNT2(INT_ARB_LOST,arbLost);
		CAN_HW_INT_ERROR_CNT2(INT_BUS_ERR,busError);
		CAN_HW_INT_ERROR_CNT2(INT_DATA_OVR,dataOverrun);

		if (canGsr2->bs)
		{
			canHwStatus2.busoff = 1;
			canHwStatusEvent(CAN_HW_ERROR_BUSOFF);

			// clear buss-off
			canGsr2->txerr = 0;
			LPC_CAN2->MOD &= ~(1 << 0); // reset CAN Hardware (clear RM bit in the MOD register)
		}

		if (canGsr2->es)
			canHwStatus2.errorstat = 1;
	}
}

/**
 *
 */
veBool veLpcCanSend(RawCanMsg const *rawCanMsg)
{
	// assert on flags != 0

	// get channel from the highest 2 bits of the flags
	switch(canGetChannel(rawCanMsg->flags))
	{
		case 0:
			return lpcCanSend1(rawCanMsg);

		case 1:
			return lpcCanSend2(rawCanMsg);

		default: // channel not existing
			;
	}

	return veFalse;
}

/**
 *
 * @note The CAN hardware has 3 transmit buffers we only use one for now
 *
 */
veBool veLpcCanSend1(RawCanMsg const *rawCanMsg)
{
	un32 CANStatus;
	un32 infoReg;

	CANStatus = LPC_CAN1->SR; // read status

	if (CANStatus & 0x00000004) // check Tx 1 status
	{
		infoReg = ((un32) rawCanMsg->length << LPC_CAN_DLC_SHIFT); /* set length */

		if (rawCanMsg->flags & CAN_MSG_RTR)
			infoReg |= LPC_CAN_RTR;

		if (rawCanMsg->flags & CAN_MSG_EXT)
			infoReg |= LPC_CAN_FF;

		LPC_CAN1->TFI1 = infoReg & 0xC00F0000;
		LPC_CAN1->TID1 = rawCanMsg->canId;

		// copy data
		memcpy((void *) &LPC_CAN1->TDA1, &rawCanMsg->mdata, 8);

		if (rawCanMsg->flags & CAN_DO_LOOP)
		{
			memcpy(&can1MsgTx, rawCanMsg, sizeof(RawCanMsg)); // save message
			can1MsgCheck = true;

			if (rawCanMsg->flags & CAN_SHOOT)
				LPC_CAN1->CMR = (LPC_CAN_CMD_TR | LPC_CAN_CMD_AT | LPC_CAN_CMD_SEL_STB1);	// send message with self reception
			else
				LPC_CAN1->CMR = (LPC_CAN_CMD_SRR | LPC_CAN_CMD_SEL_STB1);	// send message with self reception
		} else {
			LPC_CAN1->CMR = (LPC_CAN_CMD_TR | LPC_CAN_CMD_SEL_STB1);	// send message
		}

		return veTrue;
	}

	return veFalse; // no transmit buffers free
}

/**
 *
 * @note The CAN hardware has 3 transmit buffers we only use one for now
 *
 */
veBool veLpcCanSend2(RawCanMsg const * rawCanMsg)
{
	un32 CANStatus;
	un32 infoReg;

	CANStatus = LPC_CAN2->SR; // read status

	if (CANStatus & 0x00000004) // check Tx 1 status
	{
		infoReg = ((un32)rawCanMsg->length << LPC_CAN_DLC_SHIFT); // set length

		if (rawCanMsg->flags & CAN_MSG_RTR) // check and set remote request
			infoReg |= LPC_CAN_RTR;

		if (rawCanMsg->flags & CAN_MSG_EXT) // check and set remote request
			infoReg |= LPC_CAN_FF;

		LPC_CAN2->TFI1 = infoReg & 0xC00F0000;
		LPC_CAN2->TID1 = rawCanMsg->canId;

		// copy data
		memcpy((void *) &LPC_CAN2->TDA1, &rawCanMsg->mdata, 8);

		if (rawCanMsg->flags & CAN_DO_LOOP)
		{
			memcpy(&can2MsgTx, rawCanMsg, sizeof(RawCanMsg)); // save message
			can2MsgCheck = veTrue;

			if(rawCanMsg->flags & CAN_SHOOT)
				LPC_CAN2->CMR = (LPC_CAN_CMD_TR | LPC_CAN_CMD_AT | LPC_CAN_CMD_SEL_STB1);	// send message with self reception
			else
				LPC_CAN2->CMR = (LPC_CAN_CMD_SRR | LPC_CAN_CMD_SEL_STB1);	// send message with self reception
		} else {
			LPC_CAN2->CMR = (LPC_CAN_CMD_TR | LPC_CAN_CMD_SEL_STB1);	// send message
		}

		return veTrue;
	}

	return veFalse; // no transmit buffers free
}

/**
 *
 */
veBool veLpcCanRead(RawCanMsg *rawCanMsg)
{
	un32 CANStatus;

	// check if message available
	CANStatus = LPC_CANCR->CANRxSR;

	if ((CANStatus & (1 << 8)) || can1MsgCheck) 			// check if channel 1 no errors
		return lpcCanRead1(rawCanMsg);
	else if ((CANStatus & (1 << 9)) || can2MsgCheck) 		// check if channel 2 no errors
		return lpcCanRead2(rawCanMsg);

	return veFalse;
}

veBool veLpcCanRead1(RawCanMsg *rawCanMsg)
{
	un32 CANStatus;

	if (can1MsgCheck)
	{
		// do not copy the notused2 (next) and notused1 it can cause a
		// loop in the buffer queue of tx ad rx

		rawCanMsg->canId = can1MsgTx.canId;
		rawCanMsg->length = can1MsgTx.length;

		memcpy(rawCanMsg->mdata,&can1MsgTx.mdata,8); // copy data

		// check and set message if loopback
		if (can1MsgTx.flags & CAN_DO_LOOP)
			rawCanMsg->flags = (CAN_IS_LOOP | CAN_MSG_EXT);

		can1MsgCheck = veFalse;

		return veTrue;
	}

	CANStatus = LPC_CAN1->SR; // read status

	if (CANStatus & 0x00000001) // check if received
	{
		rawCanMsg->flags = 0;

		// check and set message if loopback
		if (can1MsgTxFlags & CAN_DO_LOOP)
		{
			rawCanMsg->flags = CAN_IS_LOOP;
			can1MsgTxFlags = 0;
		}

		// check if extended or standard id
		if (LPC_CAN1->RFS & LPC_CAN_FF)
			rawCanMsg->flags |= CAN_MSG_EXT;
		else
			rawCanMsg->flags |= CAN_MSG_STD;

		// check if remote request
		if (LPC_CAN1->RFS & LPC_CAN_RTR)
			rawCanMsg->flags |= CAN_MSG_RTR;

		// set channel number 0
		rawCanMsg->flags |= (0 << CAN_CHANNEL_SHIFT);

		// get dlc
		rawCanMsg->length = (un16)((LPC_CAN1->RFS >> LPC_CAN_DLC_SHIFT) & 0xF);
		rawCanMsg->canId = LPC_CAN1->RID; // id

		// copy data
		memcpy((void *) rawCanMsg->mdata, (void *) &LPC_CAN1->RDA, 8);

		LPC_CAN1->CMR = 0x01 << 2; // release receive buffer
		return veTrue;
	}

	return veFalse;
}

veBool veLpcCanRead2(RawCanMsg *rawCanMsg)
{
	un32 CANStatus;

	CANStatus = LPC_CAN2->SR; // read status

	if (can2MsgCheck)
	{
		// do not copy the notused2 (next) and notused1 it can cause a
		// loop in the buffer queue of tx ad rx

		rawCanMsg->canId = can2MsgTx.canId;
		rawCanMsg->length = can2MsgTx.length;

		memcpy(rawCanMsg->mdata,&can2MsgTx.mdata,8); // copy data

		// check and set message if loopback
		if (can2MsgTx.flags & CAN_DO_LOOP)
			rawCanMsg->flags = (CAN_IS_LOOP | CAN_MSG_EXT);

		can2MsgCheck = veFalse;
		canHwStatusEvent(CAN_HW_ERROR_NONE); // reset hardware errors

		return veTrue;
	}

	if (CANStatus & 0x00000001) // check if received
	{
		rawCanMsg->flags = 0;

		// check and set message if loopback
		if (can2MsgTxFlags & CAN_DO_LOOP)
		{
			rawCanMsg->flags = CAN_IS_LOOP;
			can2MsgTxFlags = 0;
		}

		// check if extended or standard id
		if (LPC_CAN2->RFS & LPC_CAN_FF)
			rawCanMsg->flags |= CAN_MSG_EXT;
		else
			rawCanMsg->flags |= CAN_MSG_STD;

		// check if remote request
		if (LPC_CAN2->RFS & LPC_CAN_RTR)
			rawCanMsg->flags |= CAN_MSG_RTR;

		// set channel number 1
		rawCanMsg->flags |= (1 << CAN_CHANNEL_SHIFT);

		// get dlc
		rawCanMsg->length = (un16)((LPC_CAN2->RFS >> LPC_CAN_DLC_SHIFT) & 0xF);

		rawCanMsg->canId = LPC_CAN2->RID; // id

		// copy data
		memcpy((void *) rawCanMsg->mdata, (void*) &LPC_CAN2->RDA, 8);

		canHwStatusEvent(CAN_HW_ERROR_NONE); // reset hardware errors
		LPC_CAN2->CMR = 0x01 << 2; // release receive buffer

		return veTrue;
	}

	return veFalse;
}

/**
 *
 */
veBool veLpcCanSetBitrate(un8 channel, un32 bitRate)
{
	if (channel >= CAN_MAX_PORTS)
		return veFalse;

	switch(channel)
	{
		case 0:
			can1BitRate = bitRate;
			break;

		case 1:
			can2BitRate = bitRate;
			break;

		default:
			return veFalse;
	}

	return veTrue;
}

void veLpcCanSetACCFLookup(void)
{
	un32 address = 0;
	un32 i;
	un32 ID_high, ID_low;

	/* Set explicit standard Frame */
	LPC_CANAF->SFF_sa = address;
	for (i = 0; i < ACCF_IDEN_NUM; i += 2)
	{
		ID_low = (i << 29) | (EXP_STD_ID << 16);
		ID_high = ((i+1) << 13) | (EXP_STD_ID << 0);
		*((volatile un32 *)(LPC_CANAF_RAM_BASE + address)) = ID_low | ID_high;
		address += 4;
	}

	/* Set group standard Frame */
	LPC_CANAF->SFF_GRP_sa = address;
	for (i = 0; i < ACCF_IDEN_NUM; i += 2)
	{
		ID_low = (i << 29) | (GRP_STD_ID << 16);
		ID_high = ((i+1) << 13) | (GRP_STD_ID << 0);
		*((volatile un32 *)(LPC_CANAF_RAM_BASE + address)) = ID_low | ID_high;
		address += 4;
	}

	/* Set explicit extended Frame */
	LPC_CANAF->EFF_sa = address;
	for ( i = 0; i < ACCF_IDEN_NUM; i++  )
	{
		ID_low = (i << 29) | (EXP_EXT_ID << 0);
		*((volatile un32 *)(LPC_CANAF_RAM_BASE + address)) = ID_low;
		address += 4;
	}

	/* Set group extended Frame */
	LPC_CANAF->EFF_GRP_sa = address;
	for ( i = 0; i < ACCF_IDEN_NUM; i++  )
	{
		ID_low = (i << 29) | (GRP_EXT_ID << 0);
		*((volatile un32 *)(LPC_CANAF_RAM_BASE + address)) = ID_low;
		address += 4;
	}

	/* Set End of Table */
	LPC_CANAF->ENDofTable = address;
}

void veLpcCanSetACCF(un32 ACCFMode)
{
	switch(ACCFMode)
	{
	case ACCF_OFF:
		LPC_CANAF->AFMR = ACCFMode;
		LPC_CAN1->MOD = LPC_CAN2->MOD = 1;	// Reset CAN
		LPC_CAN1->IER = LPC_CAN2->IER = 0;	// Disable Receive Interrupt
		LPC_CAN1->GSR = LPC_CAN2->GSR = 0;	// Reset error counter when CANxMOD is in reset
		break;

	case ACCF_BYPASS:
		LPC_CANAF->AFMR = ACCFMode;
		break;

	case ACCF_ON:
	case ACCF_FULLCAN:
		LPC_CANAF->AFMR = ACCF_OFF;
		lpcCanSetACCFLookup();
		LPC_CANAF->AFMR = ACCFMode;
		break;

	default:
		;
	}
}

//@}
