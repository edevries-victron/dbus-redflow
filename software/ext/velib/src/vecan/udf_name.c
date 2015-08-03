#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/base/base.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939_device.h>
#include <velib/nmea2k/n2k.h>
#include <velib/vecan/udf_name.h>
#include <velib/vecan/vecan.h>
#include <velib/vecan/udf_name.h>

/**
 * @addtogroup VELIB_VECAN_UDF_NAME
 *
 * A single product can contain multiple bus device. To match them one device
 * is pointed the primary device. This device should also accept all updates.
 * Hence it is called the Update Device Function (UDF). The unique part of the
 * NAME should be send by all device in the node (even if there is only one)
 *
 * @ref CFG_UDF_DEVICE
 */

/// Init not to send UDF NAME.
void veUdfNameInit(void)
{
	j1939Device.udf.marked = veFalse;
}

/// Actually attempt to send the UDF.
veBool veUdfNameSend(void)
{
	(void)veOutAllocSf();
	if (n2kMsgOut == NULL)
		return veFalse;

	n2kMsgOut->msgId.w = J1939ID_PRI(7) | J1939ID_PGN(PGN0_VECAN_UDF_NAME);

	veOutUn16Le(N2K_MAN_TAG_VICTRON);
	veOutUn32Le(CFG_UDF_DEVICE.name.low);
	veOutUn8(CFG_UDF_DEVICE.name.high.fields.function);
	veOutUn8(CFG_UDF_DEVICE.name.high.fields.devClass << 1);

	veOutSendSf();
	return veTrue;
}

/// Try to send the UDF NAME when requested.
void veUdfNameUpdate(void)
{
	if (j1939Device.udf.marked && veUdfNameSend())
		j1939Device.udf.marked = veFalse;
}

/// Mark the NAME as requested.
void veUdfNameReq(void)
{
	j1939Device.udf.marked = veTrue;
}

/// optional: Stub to round robin modules.
void veUdfNameHandler(VeEvent ev)
{
	VE_UNUSED(ev);
	veUdfNameUpdate();
}

