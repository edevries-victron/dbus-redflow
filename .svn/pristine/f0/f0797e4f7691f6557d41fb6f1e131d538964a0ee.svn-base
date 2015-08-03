#ifndef _UTILS_VE_EVENTS_H_
#define _UTILS_VE_EVENTS_H_

typedef enum
{
	VE_EV_INIT,
	VE_EV_UPDATE,
	VE_EV_TICK,
	VE_EV_MSG_RAW_RX,			//< just a received message
	VE_EV_MSG_RAW_FP,
	VE_EV_NAD_LOST,
	VE_EV_MSG_RX	= 0x80,
	VE_EV_VREG_RX,
	VE_EV_VREG_RX_ACK,
	VE_EV_VREG_RX_REQ,
	VE_EV_VREG_RX_CMD,
	VE_EV_VUP_RX,
	VE_EV_VUP_TX,
	VE_EV_DEV_INIT,
	VE_EV_DEV_UPDATE
} VeEvent;

typedef void (*VeEventHndlr)(VeEvent);

extern VeEventHndlr const veEventHndlrDef[];

void veEventInit(void);
void veEventUpdate(void);

#define veEventTick()	veEventInit()

#endif
