#include <velib/canhw/canhw_flexcan1.h>
#include <velib/base/base.h>

#include <platform.h>

typedef union
{
	un32 number;
	un8 bytes[4];
} CanData;

void veFlexCan1Init(void)
{
	un8 n;
	for (n = 6; n < ARRAY_LENGTH(CAN1_BASE_PTR->MB); n++)
		CAN1_BASE_PTR->MB[n].CS = CAN_CS_CODE(FLEXCAN_MB_CODE_TX_INACTIVE);
}

void veFlexCan1BusOn(void)
{
	// De-assert Freeze Mode
	CAN1_MCR &= ~CAN_MCR_HALT_MASK;

	// Wait till exit of freeze mode
	while(CAN1_MCR & CAN_MCR_FRZACK_MASK);

	// Wait till ready
	while(CAN1_MCR & CAN_MCR_NOTRDY_MASK);
}

void veFlexCan1BusOff(void)
{
	// Assert Freeze Mode
	CAN1_MCR |= CAN_MCR_HALT_MASK;

	// Wait till enter freeze mode
	while(!(CAN1_MCR & CAN_MCR_FRZACK_MASK));
}

veBool veFlexCan1Read(VeRawCanMsg * const rawCanMsg)
{
	un8 n;
	CanData data;

	if ((CAN1_IFLAG1 & FLEXCAN_IFLAG1_RXFIFO) == 0)
		return veFalse;

	rawCanMsg->canId = CAN1_ID0;
	rawCanMsg->length = (CAN1_CS0 & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT;

	data.number = CAN1_WORD00;
	for (n=0; n <4; n++)
		rawCanMsg->mdata[n] = data.bytes[3-n];

	data.number = CAN1_WORD10;
	for (n=0; n <4; n++)
		rawCanMsg->mdata[4+n] = data.bytes[3-n];

	CAN1_IFLAG1 = FLEXCAN_IFLAG1_RXFIFO;

	return veTrue;
}

veBool veFlexCan1Send(VeRawCanMsg const * const rawCanMsg)
{
	un8 n;
	CanData data;

	if ((CAN1_BASE_PTR->MB[10].CS & FLEXCAN_MB_CS_CODE_MASK) != FLEXCAN_MB_CS_CODE(FLEXCAN_MB_CODE_TX_INACTIVE))
		return veFalse;

	CAN1_BASE_PTR->MB[10].ID = rawCanMsg->canId;

	for (n=0; n <4; n++)
		data.bytes[3-n] = rawCanMsg->mdata[n];
	CAN1_BASE_PTR->MB[10].WORD0 = data.number;

	for (n=0; n <4; n++)
		data.bytes[3-n] = rawCanMsg->mdata[4+n];
	CAN1_BASE_PTR->MB[10].WORD1 = data.number;

	CAN1_BASE_PTR->MB[10].CS = CAN_CS_DLC(rawCanMsg->length) | CAN_CS_IDE_MASK | CAN_CS_SRR_MASK;
	CAN1_BASE_PTR->MB[10].CS |= CAN_CS_CODE(FLEXCAN_MB_CODE_TX_ONCE);

	return veTrue;
}
