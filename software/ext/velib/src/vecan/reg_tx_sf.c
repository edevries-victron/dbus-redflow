#include <velib/velib_inc_J1939.h>

#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939_device.h>
#include <velib/J1939/J1939_nmt.h>
#include <velib/vecan/vecan.h>
#include <velib/vecan/reg.h>
#include <velib/nmea2k/n2k.h>

/// Request one or multiple Victron Register.
veBool veRegRequestMask(J1939Nad tg, VeRegId regId, VeRegId mask)
{
	if (!veRegPrepareSf(tg, VE_REG_REQ))
		return veFalse;
	regId &= mask;
	veOutUn16Le(regId);
	veOutUn16Le(mask);

	if (tg == j1939Device.nad)
		n2kMsgOut->flags |= VE_CAN_DO_LOOP;

	veOutSendSf();
	return veTrue;
}

/// Request a single Victron Register.
veBool veRegRequest(J1939Nad tg, VeRegId regId)
{
	return veRegRequestMask(tg, regId, 0xFFFF);
}

#if CFG_WITH_VECAN_REG_REQUEST_WHEN_ON_BUS

/// Request one or multiple Victron Register.
veBool veRegRequestMaskWhenOnBus(struct J1939Func* dev, VeRegId regId, VeRegId mask)
{
	if (!j1939NmtIsOnBus(dev))
		return veFalse;

	return veRegRequestMask(dev->nmt.nad, regId, mask);
}

/// Request a single Victron Register.
veBool veRegRequestWhenOnBus(struct J1939Func* dev, VeRegId regId)
{
	return veRegRequestMaskWhenOnBus(dev, regId, 0xFFFF);
}

#endif
