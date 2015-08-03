#ifndef _VELIB_CANHW_CANHW_KVASER_H_
#define _VELIB_CANHW_CANHW_KVASER_H_

#define CAN_BASE 0xEF00			// base address of CAN memory

// memory mapped CAN messages
#define canhwBuffer ((CanhwBuffer volatile sdata *) 0xEF10)

///----------------------- Intel 82527 CAN controller definitions ------------------------
#define I527_CONTROL			*(un8*)(CAN_BASE + 0x00)		 // control register

#define I527_CONTROL_IE 		0x02
#define I527_CONTROL_SIE 		0x04
#define I527_CONTROL_EIE 		0x08

#define I527_STATUS				*(un8*)(CAN_BASE + 0x01)		 // status register
#define I527_CPU_IF				*(un8*)(CAN_BASE + 0x02)		 // CPU interface register
#define I527_BIT_TIME			*(un16*)(CAN_BASE + 0x04)		 // bit timing register 0
#define I527_GLOBMASK_STD		*(un16*)(CAN_BASE + 0x06)		 // global mask - STANDARD register
#define I527_GLOBMASK_EXT		*(un32*)(CAN_BASE + 0x08)		 // global mask - EXTENDED register
#define I527_MESS15_MSK			*(un32*)(CAN_BASE + 0x0c)		 // message 15 mask register

#define CAN_IE 	1
#define INIT	0

#define EnableCanInterrupt()	(I527_CONTROL |= (1<<CAN_IE))
#define DisableCanInterrupt()	(I527_CONTROL &= ~(1<<CAN_IE))

#define CAN_STATUS_CHANGED	0x01

/// control register 0
#define MSGVAL_R 0x40			 // message valid
#define MSGVAL_S 0x80
#define MSGVAL_U 0xc0

#define TXIE_R	 0x10			// tx interrupt
#define TXIE_S	 0x20
#define TXIE_U	 0x30

#define RXIE_R	 0x04			// recieve interrupt
#define RXIE_S	 0x08
#define RXIE_U	 0x0c

#define INTPND_R 0x01			// interrupt pending
#define INTPND_S 0x02
#define INTPND_U 0x03

/// control register 1 
#define RMTPND_R 0x40			// remote frame pending
#define RMTPND_S 0x80
#define RMTPND_U 0xc0

#define TXRQST_R 0x10			// transmit request
#define TXRQST_S 0x20
#define TXRQST_U 0x30

#define CPUUPD_R 0x04			// write - cpu update - same bits as message lost
#define CPUUPD_S 0x08
#define CPUUPD_U 0x0c

#define MSGLST_R	CPUUPD_R	// read - message lost - same bits as cpu update
#define MSGLST_S	CPUUPD_S
#define MSGLST_U	CPUUPD_U

#define NEWDAT_R	0x01		// new data
#define NEWDAT_S	0x02
#define NEWDAT_U	0x03

/// message configuration register
#define STD			0x00
#define XTD			0x04
#define DIR_TX		0x08

/// status register
#define BOFF		0x80
#define EWARN		0x40
#define WAKE		0x20
#define RXOK		0x10
#define TXOK		0x08
#define LEC_MASK	0x07

#include <velib/base/pack_push1.h>

typedef struct
{
	un8 cntrl0;
	un8 cntrl1;
	un8 arbitrator[4];
	un8 mess_config;	///< dlc + direction
	un8 data[8];
	un8 reserved;
} CanhwBuffer;

#include <velib/base/pack_pop.h>

#endif
