#ifndef _VELIB_J1939_J1939_SF_H_
#define	_VELIB_J1939_J1939_SF_H_

/** 
 * @ingroup VELIB_J1939
 * @defgroup VELIB_J1939_SF		SF
 * @{
 */

#include <velib/velib_config.h>
#include <velib/base/base.h>
#include <velib/J1939/J1939_types.h>
#include <velib/canhw/canhw.h>

/** @brief Declaration of a single frame message. 
@extends J1939Msg
*/
typedef struct
{
	J1939Id				msgId;		//< PGN describing data contents
	VeCanLength			length;		//< Length of the data.
	VeCanFlags			flags;		//< flags for special purposes.
	J1939MsgType		type;		//< type of message (sf)
	J1939Msg*			next;		//< next message in list
#if CFG_CAN_MSG_EXTRA
	VeCanMsgExtra extra;			///< additional fields which can be added to a msg
#endif
	un8					mdata[8];	//< data bytes of message
} J1939Sf;


/** @brief Stack variables for single frame messages.
* 
* @details
*	This structure contains the per stack variables needed for Single Frame
*	message processing.
*/
typedef struct
{
	/// begin of free tx J1939Sf list
	J1939Sf*	txFree;
	/// begin of free rx J1939Sf list
	J1939Sf*	rxFree;
} J1939SfVars;
		

VE_DCL void		j1939SfInit(void);
VE_DCL J1939Sf*	j1939SfRxAlloc(void);
VE_DCL void		j1939SfRxFree(J1939Sf *msg);
VE_DCL J1939Sf*	j1939SfTxAlloc(void);
VE_DCL J1939Sf* j1939SfTxAllocNoNad(void);
VE_DCL void		j1939SfTxFree(J1939Sf *msg);
VE_DCL veBool	j1939SfSend(J1939Sf *msg);

/// @}

/**
 * @ingroup VELIB_J1939
 * @defgroup VELIB_J1939_NACK	Nack
 * Queues a NACK for output for pgn.
 */
/// @{
VE_DCL veBool	j1939SendNack(J1939Nad tg, un8 dp, un16 pgn);
/// @}

/**
 * @ingroup VELIB_J1939
 * @defgroup VELIB_J1939_REQ	Request
 * Queues a J1939 request.
 */
/// @{
VE_DCL veBool	j1939RequestPgn(J1939Nad tg, un8 dp, un16 pgn);
/// @}

#endif
