#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939_acl.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/reg_rx.h>

#include <string.h> // memcpy

/**
 * Handle incoming VREG ping.
 *
 * This function should be added to @ref veRegRxHndlrDef.
 */
void veRegPingHandler(VeEvent ev)
{
	if (ev == VE_EV_VREG_RX_CMD && veRegRxId == VE_REG_PING)
	{
		// Prepare a message.
		if (!veRegPrepareSf(J1939_NAD_BROADCAST, VE_REG_PING))
			return;

		// Add data.
		(void) memcpy(&n2kStreamOut.buffer[4], &n2kStreamIn.buffer[4], 4);
		n2kStreamOut.index = 8;
		veOutSendSf();

		veRegRxHandled = veTrue;
	}
}
