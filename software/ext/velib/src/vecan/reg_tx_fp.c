#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/io/ve_stream_n2k.h>
#include <velib/nmea2k/n2k.h>
#include <velib/nmea2k/n2k_fp.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/vecan.h>

un8 veRegFpSeq;

/** 
 * Prepare a message for VE.Reg format.
 * 
 * When succesfull message is available as current output message, n2kMsgOut.
 * NULL otherwise.
 */
void veRegPrepareFp(J1939Nad tg, VeRegId regId)
{
	veOutMsg((J1939Msg*) n2kFpTxAlloc());
	if (n2kMsgOut == NULL)
		return;

	n2kMsgOut->msgId.w = J1939ID_PRI(7) | J1939ID_DP(1) | J1939ID_PGN(PGN1_VECAN_VREG | tg);

	veOutUn16Le(N2K_MAN_TAG_VICTRON);
	veOutUn16Le(regId);
}

