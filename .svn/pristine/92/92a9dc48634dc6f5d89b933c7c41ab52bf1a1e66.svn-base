#ifndef _VELIB_VECAN_PROD_DATA_H_
#define _VELIB_VECAN_PROD_DATA_H_

#include <velib/vecan/prod_types.h>
#include <velib/J1939/J1939.h>
#include <velib/types/variant.h>

/**
 * @ingroup VELIB_VECAN
 * @defgroup VELIB_VECAN_PROD_DATA Data Producer
 */
/// @{

/// flag that the message must be send.
#define MESSAGE_REQUESTED 		b00000001
/** 
 * The explicit request flags is set when an request is made for the message,
 * this can be for example be used to suppress periodic sending because a feature
 * is not supported, but stil allow device it to be requested.
 */
#define MESSAGE_REQ_EXPLICIT	b00000010
/** Disables periodic sending */
#define MESSAGE_TMR_OFF			b00000100
/* The reason of the request is that a value changed */
#define MESSAGE_ON_CHANGE		b00001000
/* Rate limit */
#define MESSAGE_SENT_DELAYED	b00010000
/* Loop the messages if the request was looped. */
#define MESSAGE_LOOPBACK		b00100000

#if !defined(CFG_PROD_MESSAGE_NUMBER) && defined(CFG_VECAN_MESSAGE_NUMBER)
#pragma warning use CFG_PROD_MESSAGE_NUMBER instead!
/* This is to avoid confusion with CFG_CONS_MESSAGE_NUMBER */
#define CFG_PROD_MESSAGE_NUMBER	CFG_VECAN_MESSAGE_NUMBER
#endif

#if CFG_J1939_DEVICES==1
// If there is a single device function, all messages are for this device.
#define VE_PROD_MSG_FIRST	0
#define VE_PROD_MSG_LAST	(CFG_PROD_MESSAGE_NUMBER-1)
#else
// If there are multiple, act on the messages for the active device.
#define VE_PROD_MSG_FIRST	j1939DeviceDef.prod.msgFirst
#define VE_PROD_MSG_LAST	j1939DeviceDef.prod.msgLast
#endif

// 50ms increasing sequence identifier.
extern un8 veProdDataSid;

// Create and queue message for the values of the registers
void	veProdDataInit(void);
veBool	veProdDataIsMsg(un8 dp, J1939Pgn pgn, VeMsgNr* msgNr);
void	veProdDataMarkMsg(VeMsgNr msgNr);
veBool	veProdDataMarkPgn(un8 dp, J1939Pgn pgn);
void	veProdDataOutSignal(VeProdSgnDef const * const signal);
void	veProdDataOnChange(VeMsgNr msgNr);
void	veProdDataPeriodic(VeMsgNr msgNr, veBool enable);
veBool	veProdDataSendMsgNr(VeMsgNr msgNr);
VE_DCL void	veProdDataSync(void);
void	veProdDataTick(void);
void	veProdDataUpdate(void);

/**
 * Callback to get the bus value for a signal which needs to be scaled.
 *
 * @param signal	
 * 	The signal whos bus values is requested.
 * @param variant[out]
 * 	The value and type corresponding to this signal.
 */
void veProdScaledValueCallback(VeProdSgnDef const * signal, VeVariant* variant);

/** Fast packets need a sequence number per CAN identifier, but multiple 
 * messages might use the same -> ask the application for the correct sequence..
 */
un8* veProdSequenceCallback(VeMsgNr nr);

/// Gets the minimum and maximum allowed values for signals which can be set.
VeProdSgnSpanDef const* veProdSgnSpanCallback(VeProdSgnDef const * signal);

/**
 * Hook for application to update signals or ignore the message
 * if it contains no valid data.
 */
veBool 	veProdSendMsgEvent(VeMsgNr nr);

/// @}

#endif
