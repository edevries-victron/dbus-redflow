#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/canhw/canhw.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_tp.h>
#include <velib/J1939/J1939_stack.h>
#include <velib/utils/ve_assert.h>

/** 
 * Init the queues to empty.
 *
 * @note Messages must be provided by the free functions!
 */
void j1939TpInit(void)
{
	j1939Stack.tp.rxFree = NULL;
	j1939Stack.tp.txFree = NULL;
	j1939Stack.tp.rx = NULL;
	j1939Stack.tp.tx = NULL;
}

/**
* @internal
*	Prepare the identical part for RX and TX Transport Message.
*
* @param dataId		the PGN being transferred
* @param sf			the message which must be prepared for sending
*
* @note 
*	The src and target are not set, since in depends on direction!
*	@see j1939TpRxPrepare and @see j1939TpTxPrepare.
*/
void j1939TpPrepare(J1939Id dataId, J1939Sf* sf)
{
	un8 n;

	// BAM, ABORT, RTS, CTS and EOM message share a common format
	sf->flags = VE_CAN_EXT;
	sf->length = 8;

	sf->msgId.p.pg.w = 0;
	sf->msgId.p.pg.p.pri = 6;
	sf->msgId.p.pf = (un8) (J1939_PGN0_TP_CTSG >> 8);
	sf->msgId.p.ps = (un8) J1939_PGN0_TP_CTSG;

	for (n = 0; n <= 4; n++)
		sf->mdata[n] = 0xFF;

	sf->mdata[5] = dataId.p.ps;
	sf->mdata[6] = dataId.p.pf;
	sf->mdata[7] = dataId.p.pg.w;
}

/** 
 * Returns the canId as contained in the payload of a TP message.
 *
 * The source part of the pgn is set to the source of the transport 
 * message. No filtering is performed in order to make it possible
 * to send an ERROR with the exact same (invalid) PGN as received.
 */
J1939Id j1939TpDataPgn(J1939Sf const *sf)
{
	J1939Id ret;

	ret.p.pg.w = 0;
	ret.p.ps = sf->mdata[5];
	ret.p.pf = sf->mdata[6];
	ret.p.pg.p.dp = sf->mdata[7];
	ret.p.src = sf->msgId.p.src;

	return ret;
}

/**
 * Remove a message from the rx or tx Tp queue.
 */
void j1939TpDequeue(J1939Tp **queue, J1939Tp *tp)
{
	J1939Tp *previous = NULL;
	J1939Tp *current = *queue;
	
	veAssert(queue != NULL && tp != NULL);

	while (current != NULL)
	{
		if (current == tp)
		{
			// found, remove from queue
			if (previous)
				previous->next = current->next;
			else
				*queue = current->next;

			current->next = NULL;
			return;
		}

		previous = current;
	}

	veAssert(veFalse);
}

/** 
 * Handle the receival of a message from the RTS-group
 *
 * @details
 *	Handles the incoming message for PGN 0xEC00 for the TP protocol (rx and tx)
 * @return
 *	veFalse: If the message could temporarily not be handled. veTrue otherwise.
 */
veBool j1939TpRecRTSGroup(J1939Sf const *sf)
{
	veAssert(sf != NULL && sf->msgId.p.pg.p.dp == 0 && sf->msgId.p.pf == 0xEC);

	switch(sf->mdata[0])
	{
		case J1939_TP_CM_CTS:
			j1939TpRecCTS(sf);
			break;

		case J1939_TP_CM_RTS:
		case J1939_TP_CM_BAM:
			return j1939TpRecBAM(sf);

		case J1939_TP_CM_EOM:
			j1939TpRecEOM(sf);
			break;

		case J1939_TP_CM_CONN_ABORT:
			j1939TpTxRecABORT(sf);
			j1939TpRxRecABORT(sf);
			break;
	}

	return veTrue;
}
