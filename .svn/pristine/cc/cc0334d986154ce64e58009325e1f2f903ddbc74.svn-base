#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939.h>
#include <velib/nmea2k/n2k.h>
#include <velib/vecan/vecan.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/reg_rx.h>
#include <velib/vecan/reg_prod_sf.h>
#include <velib/vecan/reg_prod_cmd.h>
#include <velib/vecan/prod_data.h>
#include <velib/utils/ve_todo.h>
#include <velib/utils/ve_assert.h>

/*
 * note: normally NACKS are not sent if the target is a broadcast address
 * Since we are dealing with vreg commands here the destionation is per
 * definition not broadcast, so there is no need to for it.
 */
static void handleResult(VeRegNr regNr, VeRegAckCode ackCode)
{
	/* internal return code, to supress this automatic responses */
	if (ackCode == VACK_DO_NOT_SEND_VREG)
		return;

	if (ackCode == VACK_OK)
		veRegProdSfReqVreg(veRegProdSfTypedDef[regNr].regId);
	else
		veRegSendAckNoRetry(n2kMsgIn->msgId.p.src, veRegProdSfTypedDef[regNr].regId, ackCode);
}

static void veRegProdCmdSet(VeRegNr regNr)
{
	VeVariant variant;
	VeProdSgnSpanDef const *span;
	VeRegAckCode ackCode;

	variant.type.tp = veRegProdSfTypedDef[regNr].datatype;

	/* Read data */
	veInVariantLe(&variant);

	/* Getter / setter callback */
	if (veRegProdSfTypedDef[regNr].flags & V_FUN) {
		VeRegProdValueCallback *callback = (VeRegProdValueCallback*) veRegProdSfTypedDef[regNr].data;

		ackCode = callback(V_SET, &veRegProdSfTypedDef[regNr], &variant);
		handleResult(regNr, ackCode);
		return;
	}

	veVariantToN32(&variant);
	span = veRegProdCmdGetSpan(regNr);
	if (!span)
		return;

	/* Check boundaries */
	if (variant.value.SN32 < span->min || variant.value.SN32 > span->max) {
		(void) veRegSendAckNoRetry(n2kMsgIn->msgId.p.src, veRegRxId, VACK_ERR_CMD);
		return;
	}

	/* Convert back to original type */
	veVariantFromN32(&variant, veRegProdSfTypedDef[regNr].datatype);

	ackCode = veRegProdCmdCallback(veRegProdSfTypedDef[regNr].data, &variant); 
	handleResult(regNr, ackCode);
}

// Handler for VREG commands
void veRegProdCmdHandler(VeEvent ev)
{
	VeRegNr n = 0;
	
	// Only handle commands
	if (ev != VE_EV_VREG_RX_CMD)
		return;

	do
	{
		if (veRegProdSfTypedDef[n].regId == veRegRxId &&
			(veRegProdSfTypedDef[n].flags & V_WRITE)) {
			veRegProdCmdSet(n);
			veRegRxHandled = veTrue;
		}
	} while (veRegProdSfTypedDef[++n].regId);
}
