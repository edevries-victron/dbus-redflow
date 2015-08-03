#ifndef _VELIB_CANHW_LPC17XX_H_
#define _VELIB_CANHW_LPC17XX_H_

#include <canhw/canhw.h>

// interrupt enable bits
#define INT_RX						(1 << 0)
#define INT_TX1						(1 << 1)
#define INT_ERR_WARN				(1 << 2)
#define INT_DATA_OVR				(1 << 3)
#define INT_WAKE_UP					(1 << 4)
#define INT_ERR_PASS				(1 << 5)
#define INT_ARB_LOST				(1 << 6)
#define INT_BUS_ERR					(1 << 7)
#define INT_ID_RDY					(1 << 8)
#define INT_TX2						(1 << 9)
#define INT_TX3						(1 << 10)

#define CAN_WAKEUP					0
#define ACCEPTANCE_FILTER_ENABLED	1

#define CAN_MAX_PORTS				2		// Number of CAN port on the chip

/*
 * BRP+1 = Fpclk/(CANBitRate * QUANTAValue)
 * QUANTAValue = 1 + (Tseg1+1) + (Tseg2+1)
 * QUANTA value varies based on the Fpclk and sample point
 *
 * e.g. (1) sample point is 87.5%, Fpclk is 48Mhz
 * the QUANTA should be 16
 *		(2) sample point is 90%, Fpclk is 12.5Mhz
 * the QUANTA should be 10
 * Fpclk = Fclk /APBDIV
 *  or
 * BitRate = Fcclk/(APBDIV * (BRP+1) * ((Tseg1+1)+(Tseg2+1)+1))
 */

/*
 * Here are some popular bit timing settings for LPC23xx, google on "SJA1000"
 * CAN bit timing, the same IP used inside LPC2000 CAN controller. There are several
 * bit timing calculators on the internet.
 * http://www.port.de/engl/canprod/sv_req_form.html
 * http://www.kvaser.com/can/index.htm
 */

/**
 * Bit Timing Values for 16MHz clk frequency
 */
#define BITRATE100K16MHZ			0x001C0009
#define BITRATE125K16MHZ			0x001C0007
#define BITRATE250K16MHZ			0x001C0003
#define BITRATE500K16MHZ			0x001C0001
#define BITRATE1000K16MHZ			0x001C0000

/*
 * Bit Timing Values for 24MHz clk frequency
 */
#define BITRATE100K24MHZ			0x001C000E
#define BITRATE125K24MHZ			0x001C000B
#define BITRATE250K24MHZ			0x001C0005
#define BITRATE500K24MHZ			0x001C0002
#define BITRATE1000K24MHZ			0x00090001

/*
 * Bit Timing Values for 48MHz clk frequency
 */
#define BITRATE100K48MHZ			0x001C001D
#define BITRATE125K48MHZ			0x001C0017
#define BITRATE250K48MHZ			0x001C000B
#define BITRATE500K48MHZ			0x001C0005
#define BITRATE1000K48MHZ			0x001C0002

/*
 * Bit Timing Values for 60MHz clk frequency
 */
#define BITRATE100K60MHZ			0x00090031
#define BITRATE125K60MHZ			0x00090027
#define BITRATE250K60MHZ			0x00090013
#define BITRATE500K60MHZ			0x00090009
#define BITRATE1000K60MHZ			0x00090004

/*
 * Bit Timing Values for 28.8MHz pclk frequency, 1/2 of 57.6Mhz CCLK
 */
#define BITRATE100K28_8MHZ			0x00090017

/*
 * Bit Timing Values for 18MHz pclk frequency, 1/4 of 72Mhz CCLK
 */
#define BITRATE125K18MHZ			0x001C0008

/*
 * When Fcclk is 50Mhz and 60Mhz and APBDIV is 4,
 * so Fpclk is 12.5Mhz and 15Mhz respectively.
 * when Fpclk is 12.5Mhz, QUANTA is 10 and sample point is 90%
 * when Fpclk is 15Mhz, QUANTA is 10 and sample point is 90%
 */

/* Common CAN bit rates for 12.5Mhz(50Mhz CCLK) clock frequency */
#define BITRATE125K12_5MHZ			0x00070009
#define BITRATE250K12_5MHZ			0x00070004

/* Common CAN bit rates for 25Mhz(100Mhz CCLK) clock frequency */

#define SJW_VALUE	((1-1) << 14) // synchonization jump value 1-4

#define BITRATE125K25MHZ			0x00070013
#define BITRATE250K25MHZ			0x00070009 // 90% - 10 time quanta

#define BITRATE250K25MHZ_1			(0x004D0004 | SJW_VALUE) // 75% - 20 time quanta
#define BITRATE250K25MHZ_2			(0x007F0003 | SJW_VALUE) // 68% - 25 time quanta
#define BITRATE250K25MHZ_3			(0x00160009 | SJW_VALUE) // 80% - 10 time quanta
#define BITRATE250K25MHZ_4			(0x002F0004 | SJW_VALUE) // 85% - 20 time quanta

/**
 * Bit Timing Values for 15MHz(60Mhz CCLK) clk frequency
 */
#define BITRATE100K15MHZ			0x0007000E
#define BITRATE125K15MHZ			0x0007000B
#define BITRATE250K15MHZ			0x00070005
#define BITRATE500K15MHZ			0x00070002

/* Acceptance filter mode in AFMR register */
#define ACCF_OFF					0x01
#define ACCF_BYPASS					0x02
#define ACCF_ON						0x00
#define ACCF_FULLCAN				0x04

/* This number applies to all FULLCAN IDs, explicit STD IDs, group STD IDs,
explicit EXT IDs, and group EXT IDs. */
#define ACCF_IDEN_NUM				4

/* Identifiers for FULLCAN, EXP STD, GRP STD, EXP EXT, GRP EXT */
#define FULLCAN_ID					0x100
#define EXP_STD_ID					0x100
#define GRP_STD_ID					0x200
#define EXP_EXT_ID					0x100000
#define GRP_EXT_ID					0x200000

#define LPC_CAN_DLC_SHIFT			16

#define LPC_CAN_RTR					(1<<30)	// remote request
#define LPC_CAN_FF					(1<<31)	// extended 29-bit ID

// CAN commands
#define LPC_CAN_CMD_TR				(1<<0)	// transmit message
#define LPC_CAN_CMD_AT				(1<<1)	// abort transmission
#define LPC_CAN_CMD_SRR				(1<<4)	// self reception request
#define LPC_CAN_CMD_SEL_STB1		(1<<5)	// select transmit buffer 1
#define LPC_CAN_CMD_SEL_STB2		(1<<6)	// select transmit buffer 2
#define LPC_CAN_CMD_SEL_STB3		(1<<7)	// select transmit buffer 3

void veLpcCanInit(void);
veBool veLpcCanSend(RawCanMsg const *rawCanMsg);
veBool veLpcCanRead(RawCanMsg *rawCanMsg);

veBool veLpcCanInitChannel(un8 channel);
veBool veLpcCanSetBitrate(un8 channel, un32 bitRate);
veBool veLpcCanSend1(RawCanMsg const *rawCanMsg);
veBool veLpcCanSend2(RawCanMsg const *rawCanMsg);
veBool veLpcCanRead1(RawCanMsg *rawCanMsg);
veBool veLpcCanRead2(RawCanMsg *rawCanMsg);
void veLpcCanSetACCFLookup(void);
void veLpcCanSetACCF(un32 ACCFMode);

#endif /* CANHW_LPC1768_H_ */
