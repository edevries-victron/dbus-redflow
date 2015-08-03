#include <velib/canhw/canhw_driver.h>
#include <velib/io/ve_remote_device_vecan.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939_stack.h>
#include <velib/J1939/J1939_tx.h>
#include <velib/nmea2k/n2k.h>
#include <velib/types/ve_str.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_events.h>
#include <velib/utils/ve_todo.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/reg_rx.h>
#include <velib/vecan/vecan.h>

VeEventHndlr const veRegRxHndlrDef[] =
{
	veCanRemoteDevRegHandler,
	NULL
};

void j1939TxFree(J1939Msg* msg)
{
	switch (msg->type.p.kind)
	{
	case J1939_SF:
		j1939SfTxFree((J1939Sf*) msg);
		break;
	case N2K_FP:
		n2kFpTxFree((N2kFp*) msg);
		break;
	default:
		veAssert(veFalse);
		break;
	}
}

J1939Msg* j1939TxAlloc(un8 type)
{
	switch (type)
	{
	case J1939_SF:
		return (J1939Msg*) j1939SfTxAlloc();

	case N2K_FP:
		return (J1939Msg*)  n2kFpTxAlloc();
	}

	veAssert(veFalse);
	return NULL;
}

void j1939RxFree(J1939Msg *msg)
{
	switch (msg->type.p.kind)
	{
	case J1939_SF:
		j1939SfRxFree( (J1939Sf*) msg);
		break;
	case N2K_FP:
		n2kFpRxFree( (N2kFp*) msg);
		break;
	default:
		veAssert(veFalse);
		break;
	}
}

void veMsgRouterUpdate(void)
{
	J1939Msg *msg;

	for (;;) {
		msg = veCanReadN2k();

		if (!msg)
			break;

		if (j1939MsgForDevice(&msg->msgId))
		{
			veInMsg(msg);	/* All data is read from this message.. */

			switch (J1939PgnFromIdNoTg(msg->msgId))
			{
			case PGN0_VECAN_VREG:
				if (veInUn16Le() != N2K_MAN_TAG_VICTRON)
					return;
				veRegRxData();
				break;
			}
		}

		j1939RxFree(msg);
	}

	while ((msg = j1939TxPop()) != NULL)
	{
		if (veCanSendN2k(msg)) {
			j1939TxFree(msg);
		} else {
			j1939TxPushFront(msg);
			break;
		}
	}
}
