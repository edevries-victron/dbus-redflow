#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_tx.h>

/** 
 * @param tg	The network address to send the NACK to (0xFF for J1939). 
 * @param dp	datapage
 * @param pgn	The pgn which could not be fullfilled.
 *
 * @note 
 *	J1939: Allthough the NACK is addressable it must always be send to 
 *	the broadcast address.
 *
 * @note
 *	NMEA 2000: NACK behaviour has changed after version 1.200. NACK should 
 *	now be destination specific.
 */
veBool j1939SendNack(J1939Nad tg, un8 dp, un16 pgn)
{
	J1939Sf *msg;
	
	msg = j1939SfTxAlloc();
	if (msg == NULL)
		return veFalse;
	
	// set pgn to request
	msg->mdata[0] = 0x01;			// Control Byte
	msg->mdata[1] = 0xFF;			// Group Function Value
	msg->mdata[2] =	0xFF;			// Reserved
	msg->mdata[3] =	0xFF;			// Reserved
	msg->mdata[4] =	0xFF;			// Reserved
	msg->mdata[5] = (un8) pgn;
	msg->mdata[6] = (un8) (pgn >> 8);
	msg->mdata[7] = dp;

	// pgn of message itself
	msg->length = 8;
	msg->msgId.w = J1939ID_PRI(6) | J1939ID_PGN(J1939_PGN0_ACK) | J1939ID_TG(tg);

	j1939TxPush((J1939Msg*) msg);

	return veTrue;
}

