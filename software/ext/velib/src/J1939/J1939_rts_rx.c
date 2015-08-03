/**
* @todo
*	The outgoing messages should better be queued with j1939TxAlloc 
*	instead of using veCanSend.
*/

#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/canhw/canhw.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_stack.h>
#include <velib/J1939/J1939_tp.h>

/** 
 * Helper to set the common part of the messages when receiving.
 *
 * @param		dataId	
 *	The can bus identifier being transported.
 * @param[out]	sf		
 *	The single frame being constructed as part of the transport.
 * @param		receiver 
 *	The receiver must be explicitly passed since an abort might be send in case there
 *	is no TP buffer available. In such a case the value from the incomming frame must be
 *	used. In case of a timeout, an abort must also be send; the stored value must then be
 *	used since there is no incomming frame.
 *
 * @note 
 *	the receiver is the intended receiver, i.o.w. normallly the address of one of the
 *	devices on this node.
 */
static void j1939TpRxPrepare(J1939Id dataId, J1939Sf* sf, J1939Nad receiver)
{
	j1939TpPrepare(dataId, sf);

	sf->msgId.p.ps = dataId.p.src;	// target is the sender of the TP
	sf->msgId.p.src = receiver;				// a device in this node sends it
}

/** 
 * @brief Creates and sends a TP.CTS.
 *
 * @details
 *	Sends a Transport Protocol Clear To Send message (TP.CM) with the desired 
 *	source address and specified destination addres. The information for the
 *	message is pulled from the provided message based on the nextSeqId value 
 *	and total bytes to be received.
 *
 * @param tp
 *	the J1939Tp to send the CTS for.
 */

veBool j1939TpSendCTS(J1939Tp const *tp)
{
	J1939Sf sf;

	j1939TpRxPrepare(tp->msgId, &sf, tp->receiver);
	sf.mdata[0] = J1939_TP_CM_CTS;
	sf.mdata[1] = tp->dataFrameCount;
	sf.mdata[2] = tp->nextSeqId;
	
	if (!veCanSend((VeRawCanMsg*) &sf))
		return veFalse;

	return veTrue;
}

/** 
 * @brief Send a Connection Abort.
 *
 * @param dataId 
 *	The CAN identifier as extracted from the payload of a incoming TP
 *	message. The source should be correctly set the sender of the TP.
 * @param receiver
 *	The intended receiving device in this node.
 * @retval
 *	Whether the EOM was succesfully send.
 */

veBool j1939TpRxSendABORT(J1939Id dataId, J1939Nad receiver)
{
	J1939Sf sf;

	j1939TpRxPrepare(dataId, &sf, receiver);
	// sf.mdata[0] = J1939_TP_CM_CONN_ABORT;	// default set by prepare

	return veCanSend((VeRawCanMsg*) &sf);
}

/** 
 * @brief
 *	sends a TP.EOM for the TP message tp
 *
 * @details 
 *	The End of message is send by the receiver after all data has been received.
 * @param tp
 *	The transport message to send a EOM for.
 * @retval
 *	Whether the EOM was succesfully send.
 */

veBool j1939TpSendEOM(J1939Tp const *tp)
{
	J1939Sf sf;

	j1939TpRxPrepare(tp->msgId, &sf, tp->receiver);

	sf.mdata[0] = J1939_TP_CM_EOM;
	sf.mdata[1] = (un8) tp->length;
	sf.mdata[2] = (un8) (tp->length >> 8);
	sf.mdata[3] = tp->nextSeqId;
	
	return veCanSend((VeRawCanMsg*) &sf);
}

/** 
 *	@brief handles an incoming connection abort.
 */
void j1939TpRxRecABORT(J1939Sf const *sf)
{
	J1939Tp* tp;

	// return if not found
	if ((tp = j1939TpRxFind(sf)) == NULL)
		return;

	j1939TpDequeue(&j1939Stack.tp.rx, tp);
	j1939TpRxFree(tp);
}

