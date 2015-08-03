#ifndef _VELIB_CANHW_FLEXCAN1_H_
#define _VELIB_CANHW_FLEXCAN1_H_

/**
 * @ingroup VELIB_CANHW
 * @defgroup VELIB_CANHW_MSCAN	FreeScale FlexCan
 * @brief 
 *  Standardized calls for FreeScale FlexCan
 * @{
 */

#include <velib/canhw/canhw_types.h>

/* Bit definitions and macros for FLEXCAN_IFLAG1 */
#define FLEXCAN_IFLAG1_BUF0I           	0x00000001U
#define FLEXCAN_IFLAG1_BUF1I           	0x00000002U
#define FLEXCAN_IFLAG1_BUF2I           	0x00000004U
#define FLEXCAN_IFLAG1_BUF3I           	0x00000008U
#define FLEXCAN_IFLAG1_BUF4I           	0x00000010U
#define FLEXCAN_IFLAG1_BUF5I           	0x00000020U
#define FLEXCAN_IFLAG1_BUF6I           	0x00000040U
#define FLEXCAN_IFLAG1_BUF7I           	0x00000080U
#define FLEXCAN_IFLAG1_BUF8I           	0x00000100U
#define FLEXCAN_IFLAG1_BUF9I           	0x00000200U
#define FLEXCAN_IFLAG1_BUF10I          	0x00000400U
#define FLEXCAN_IFLAG1_BUF11I          	0x00000800U
#define FLEXCAN_IFLAG1_BUF12I          	0x00001000U
#define FLEXCAN_IFLAG1_BUF13I          	0x00002000U
#define FLEXCAN_IFLAG1_BUF14I          	0x00004000U
#define FLEXCAN_IFLAG1_BUF15I          	0x00008000U
#define FLEXCAN_IFLAG1_BUF16I          	0x00010000U
#define FLEXCAN_IFLAG1_BUF17I          	0x00020000U
#define FLEXCAN_IFLAG1_BUF18I          	0x00040000U
#define FLEXCAN_IFLAG1_BUF19I          	0x00080000U
#define FLEXCAN_IFLAG1_BUF20I          	0x00100000U
#define FLEXCAN_IFLAG1_BUF21I          	0x00200000U
#define FLEXCAN_IFLAG1_BUF22I          	0x00400000U
#define FLEXCAN_IFLAG1_BUF23I          	0x00800000U
#define FLEXCAN_IFLAG1_BUF24I          	0x01000000U
#define FLEXCAN_IFLAG1_BUF25I          	0x02000000U
#define FLEXCAN_IFLAG1_BUF26I          	0x04000000U
#define FLEXCAN_IFLAG1_BUF27I          	0x08000000U
#define FLEXCAN_IFLAG1_BUF28I          	0x10000000U
#define FLEXCAN_IFLAG1_BUF29I          	0x20000000U
#define FLEXCAN_IFLAG1_BUF30I          	0x40000000U
#define FLEXCAN_IFLAG1_BUF31I          	0x80000000U

#define FLEXCAN_IFLAG1_RXFIFO			FLEXCAN_IFLAG1_BUF5I
#define FLEXCAN_IFLAG1_RXFIFO_WARNING	FLEXCAN_IFLAG1_BUF6I
#define FLEXCAN_IFLAG1_RXFIFO_OVERFLOW	FLEXCAN_IFLAG1_BUF7I

/* Bit definitions and macros for FLEXCAN_MB_CS */
#define FLEXCAN_MB_CS_TIMESTAMP(x)    	(((x)&0x0000FFFF)<<0)
#define FLEXCAN_MB_CS_TIMESTAMP_MASK  	(0x0000FFFFL)
#define FLEXCAN_MB_CS_LENGTH(x)       	(((x)&0x0000000F)<<16)
#define FLEXCAN_MB_CS_RTR             	(0x00100000)
#define FLEXCAN_MB_CS_IDE             	(0x00200000)
#define FLEXCAN_MB_CS_SRR             	(0x00400000)
#define FLEXCAN_MB_CS_CODE(x)         	(((x)&0x0000000F)<<24)
#define FLEXCAN_MB_CS_CODE_MASK			(0x0F000000L)
#define FLEXCAN_MB_CS_DLC_MASK			(0x000F0000L)
#define FLEXCAN_MB_CODE_RX_INACTIVE		(0)
#define FLEXCAN_MB_CODE_RX_EMPTY		(4)
#define FLEXCAN_MB_CODE_RX_FULL			(2)
#define FLEXCAN_MB_CODE_RX_OVERRUN		(6)
#define FLEXCAN_MB_CODE_RX_BUSY			(1)

#define FLEXCAN_MB_CS_IDE_BIT_NO        (21)
#define FLEXCAN_MB_CS_RTR_BIT_NO        (20)
#define FLEXCAN_MB_CS_DLC_BIT_NO        (16)

#define FLEXCAN_MB_CODE_TX_INACTIVE		(8)
#define FLEXCAN_MB_CODE_TX_ABORT		(9)
#define FLEXCAN_MB_CODE_TX_ONCE			(0x0C)
#define FLEXCAN_MB_CODE_TX_RESPONSE		(0x0A)
#define FLEXCAN_MB_CODE_TX_RESPONSE_TEMPO	(0x0E)

#define CFG_CANHW_NO_IS_LOOP_FLAG		1

void 		veFlexCan1Init(void);
veBool		veFlexCan1Read(VeRawCanMsg * const rawCanMsg);
veBool		veFlexCan1Send(VeRawCanMsg const * const frame);
void 		veFlexCan1BusOff(void);
void 		veFlexCan1BusOn(void);

/// @}

#endif
