#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_device.h>
#include <velib/J1939/J1939_request.h>
#include <velib/J1939/J1939_tx.h>

/** get message to request a PGN from a device */
J1939Msg* j1939RequestPgnMsg(J1939Nad tg, un8 dp, un16 pgn)
{
	J1939Msg *msg;

	if ((msg = (J1939Msg*) j1939SfTxAlloc()) == NULL)
		return NULL;

	// PGN message itself
	msg->msgId.w = J1939ID_PRI(6) | J1939ID_PGN(J1939_PGN0_REQUEST);
	msg->msgId.p.ps = tg;
	
	// set payload
	msg->mdata[0] = (un8) pgn;
	msg->mdata[1] = (un8) (pgn >> 8);
	msg->mdata[2] = dp;
	msg->length = 3;

	return msg;
}

/** Request a PGN from a device */
veBool j1939RequestPgn(J1939Nad tg, un8 dp, un16 pgn)
{
	J1939Msg *msg;

	if ((msg = j1939RequestPgnMsg(tg, dp, pgn)) == NULL)
		return veFalse;

	if (tg == J1939_NAD_BROADCAST || tg == j1939Device.nad)
		msg->flags |= VE_CAN_DO_LOOP;

	j1939TxPush(msg);

	return veTrue;
}
