#ifndef _VELIB_MK2_FRAME_HANDLER_H_
#define _VELIB_MK2_FRAME_HANDLER_H_

#include <velib/base/types.h>

#define MK2_FRAME_OVERHEAD		2	// <Length> and <Checksum>
#define MK2_MAX_MESSAGE_LENGTH	20
#define MK2_MAX_FRAME_LENGTH	(MK2_MAX_MESSAGE_LENGTH + MK2_FRAME_OVERHEAD)

/** Events passed to the MK2 handler */
typedef enum
{
	MK2_FRAME,		///< a frame arrived
	MK2_TX_SENT,	///< the message has been send
	MK2_TIMEOUT		///< no reply
} Mk2Event;

/**
 * Response for the MK2 event handler (MK2_CONTINUE is the default)
 * @note Exact meaning depends on the event.
 */
typedef enum
{
	/** sending the frame is done and can be removed */
	MK2_DONE,
	/** continue with default behaviour */
	MK2_CONTINUE
} Mk2EventReply;

typedef struct
{
	un32	rxCounter;
	un32	rxLost;
	un32	rxCrc;
	un32	rxTimeout;
	un32	txCounter;
} Mk2Stats;

/** Prototype for handling incoming message from the MK2 */
typedef void Mk2Callback(void);

typedef struct Mk2MsgRxS {
	un8 length;	/**<< The length of the payload, not the entire frame */
	un8 buf[MK2_MAX_MESSAGE_LENGTH];
	un8 checksum;
	struct Mk2MsgRxS* next;
} Mk2MsgRx;

typedef Mk2EventReply Mk2Handler(Mk2Event event, Mk2MsgRx* msg);

typedef struct Mk2MsgTxS
{
	un8 length;	/** The length of the payload, not the entire frame */
	un8 buf[MK2_MAX_MESSAGE_LENGTH];
	un8 checksum;

	/** Points to the next message in the list */
	struct Mk2MsgTxS* next;
	/**
	 * Function callback which checks if the reply is valid and reads the
	 * values for the message. This is typically a velib function.
	 */
	Mk2Handler* handler;
	/**
	 * Function callback which is called after the values are correctly
	 * received. This is typically a function in the application.
	 */
	Mk2Callback* callback;
	/**
	 * Context specific variable, typically the object to store the
	 * received values in.
	 */
	void* ctx;
	/** Used to store the value being requested. */
	un16 id;
	/** Used to check if a response is valid. */
	un16 rsp;
	/** whether to automatically add a checksum */
	veBool addChecksum;
	/** send the message as soon as possible */
	veBool noDelay;
	/** amount of retries left before getting a valid response */
	un8 retries;
} Mk2MsgTx;

typedef struct
{
	/** number of rx messages, at least 2 */
	Mk2MsgRx rxMsgs[CFG_MK2_RX_FRAMES];
	/** message being received */
	Mk2MsgRx *rxMsg;
	/** queue of free rx messages */
	Mk2MsgRx* rxFree;
	/** number of byte received */
	un8 bytesReceived;
	/** calculated checksum of the incoming message */
	un8 checksum;
	/** timer to reset the receiving state */
	un8 rxTimeout;
	/** the arrived message to be processed in the mainloop */
	Mk2MsgRx* rxMsgMainLoop;

	/** reset */
	veBool reset;

	/** number of tx messages (must be 3 at least) */
	Mk2MsgTx txMsgs[CFG_MK2_TX_FRAMES];
	/** queue of free tx messages */
	Mk2MsgTx* txFree;

	/** message being send */
	Mk2MsgTx* msgOut;
	/** position in this message */
	sn8 txPos;
	/** timer to monitor if a response was received for the tx message */
	un8 rspTimeout;

	/** some statistics being kept, @see Mk2Stats */
	Mk2Stats stats;
} Mk2FhVars;

Mk2Callback* mk2Callback(void);

void mk2FhInit(void);
void mk2FhReset(void);
void mk2FhUpdate(void);
void mk2FhTick(void);

void mk2FhQueueFrame(Mk2MsgTx* frm);
Mk2MsgTx* mk2TxAlloc(void);

/*
 * When CFG_MK2_FH_HOOKS is defined this callback will be invoked
 * after receiving / sending a message.
 */
void mk2FhFrameSendEvent(Mk2MsgTx* mk2msg);
void mk2FhFrameReceivedEvent(Mk2MsgRx* mk2msg);

/* Called when no response is received after several attempts */
void mk2NoResponseCallback(void);

/*
 * Callback before the message is adjust, which is for example usefull
 * for logging.
 */
void mk2FhRawFrameReceivedEvent(Mk2MsgRx* mk2msg);

#endif
