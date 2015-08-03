#include <stdio.h>
#include <string.h>

#include <velib/base/base.h>
#include <velib/base/ve_string.h>
#include <velib/utils/ve_logger.h>
#include <velib/canhw/canhw_console.h>
#include <velib/canhw/canhw_kvaser.h>

/**
 * @addtogroup VELIB_CANHW_CONSOLE
 *
 * @ref veCanShowRead and veCanShowSend can be used as @ref veCanRead and
 * @ref veCanSend respectively. When @ref veCanShowTrace is then enabled
 * message will be shown on a console which eases debugging. The actual read
 * and send routines can be set by:
 *
 * - @ref CFG_VE_CAN_SHOW_READ
 * - @ref CFG_VE_CAN_SHOW_SEND
 */

// When true message should be printed.
static veBool trace = veFalse;

/// Enables / disables console dumps of the send / received messages.
veBool veCanShowTrace(veBool enableTrace)
{
	veBool ret = trace;
	trace = enableTrace;
	return ret;
}

/// Prints a message to the console.
void veCanShow(VeRawCanMsg const *canMsg, char *module, char *str)
{
	int j;
	char line[1024], tmp[20];

	// short version, hex code only
	//sprintf(line, "%s, 0x%08X - ", str, canMsg->canId);

	// CAN identifier in J1939 format
	ve_snprintf(line, sizeof(line), "%s %d.%d.%d.%02X.%02X.%02X|",
		str,
		(canMsg->canId >> 26) & 0x07,
		(canMsg->canId >> 25) & 0x01,
		(canMsg->canId >> 24) & 0x01,
		(un8) (canMsg->canId >> 16),
		(un8) (canMsg->canId >> 8),
		(un8) canMsg->canId);


	for (j = 0; j < canMsg->length; j++)
	{
		ve_snprintf(tmp, sizeof(tmp), " 0x%2.2X", canMsg->mdata[j]);
		ve_strcat(line, sizeof(line), tmp);
	}

	logI(module, line);
}

/// Will print received message to the console when enabled.
veBool veCanShowRead(VeRawCanMsg *rawCanMsg)
{
	veBool ret;
	ret = CFG_VE_CAN_SHOW_READ(rawCanMsg);

	if (ret && trace)
	{
		if (rawCanMsg->flags & VE_CAN_IS_LOOP)
		{
			if (rawCanMsg->flags & VE_CAN_TX_ERR)
				veCanShow(rawCanMsg, "CAN", "!OK");
			else
				veCanShow(rawCanMsg, "CAN", "=OK");
		} else
			veCanShow(rawCanMsg, "CAN", "<<<");
	}

	return ret;
}

/// Will print send message to the console when enabled.
veBool veCanShowSend(VeRawCanMsg *rawCanMsg)
{
	veBool ret;
	ret = CFG_VE_CAN_SHOW_SEND(rawCanMsg);

	if (ret && trace)
		veCanShow(rawCanMsg, "CAN", ">>>");

	return ret;
}
