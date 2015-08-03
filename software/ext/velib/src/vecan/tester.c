#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939.h>
#include <velib/nmea2k/n2k.h>
#include <velib/vecan/vecan.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/reg_rx.h>
#include <velib/vecan/reg_prod_sf.h>
#include <velib/vecan/tester.h>
#include <velib/vecan/prod_data.h>
#include <velib/velib_inc_vecan.h>

#include <string.h>	// memcmp

// Not in test mode after boot up
static veBool veTestMode = veFalse;

/**
 * Event handler for tester messages
 *
 * @param ev The event
 */
void veTestModeHandler(VeEvent ev)
{
	char const *pass;
	veBool passOk;

	// Only handle commands
	if (ev != VE_EV_VREG_RX_CMD)
		return;

	switch (veRegRxId)
	{
	// Request test mode
	case VE_REG_TEST_MODE:

		pass = veInFixedString(VE_TESTMODE_PASSWORD_SIZE);
		passOk = pass && (memcmp(VE_TESTMODE_PASSWORD, pass, VE_TESTMODE_PASSWORD_SIZE) == 0);

		// Only set test mode
		if (passOk)
			veTestMode = passOk;

		(void)veRegSendAck(n2kMsgIn->msgId.p.src, VE_REG_TEST_MODE, passOk ? VACK_OK : VACK_ERR_INVALID);
		break;

	default:

		// Test mode callback
		if (veTestMode)
			veTestModeCallback();
		break;
	}
}

/**
 * Check if test mode is enabled
 *
 * @return Test mode enabled
 */
veBool veTestModeIsEnabled(void)
{
	return veTestMode;
}
