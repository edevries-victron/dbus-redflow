#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939.h>
#include <velib/nmea2k/n2k.h>
#include <velib/nmea2k/n2k_complex.h>

/**
 * @addtogroup VELIB_NMEA2K_COMPLEX
 * @details
 *
 * <b>Sending Complex Commands and Requests</b>
 *
 * Complex Command and Requests can be constructed as below. 
 * (@ref n2kCmAllocCmd must be replaced by @ref n2kCmAllocReq to construct a
 * Complex Request). The parameter count and fast packet sequence are updated
 * automatically.
 * 
 * @code
 *	// Create and send a Complex Command with 2 parameters attached.
 *	VeVariant const param3 = {10, VE_UN8};
 *	VeVariant const param6 = {-1, VE_SN16};
 *
 *	n2kCmAllocCmd(0x50, 0, J1939_PGN0_ACL);
 *	if (n2kMsgOut == NULL)
 *		return;
 *	n2kCmParam(3, &param3);
 *	n2kCmParam(6, &param6);
 *	n2kCmSend();
 *
 * @endcode
 *
 * Or by using the io functions directly (parameter count must then be set).
 * @code
 *	// Create and send a Complex Command with 2 parameters attached.
 *	un8* counter;
 *
 *	n2kCmAllocCmd(0x50, 0, J1939_PGN0_ACL);
 *	if (n2kMsgOut == NULL)
 *		return;
 *	veOutUn8(3);		// param ID
 *	veOutUn8(10);		// param Value
 *	veOutUn8(6);		// param ID
 *	veOutSn16Le(-1);	// param Value
 *	*counter = 2;		// two parameters attached.
 *	n2kCmSend();
 * @endcode
 */

/// pointer to the parameter counter in the constructed message.
static un8* paramCount;

/**
 * Allocates and sets the general part of a Complex Command.
 *
 * @param tg		NAD where the message must be addresses to.
 * @param dp		datapage
 * @param pgn		parameter group number
 * @return			pointer to the parameter count in the msg (set to 0)
 *
 * The message is set as output message. @ref n2kMsgOut will be NULL when no
 * message could be allocated. The priority field is set to unchanged.
 */
un8* n2kCmAllocCmd(J1939Nad tg, un8 dp, un16 pgn)
{
	n2kCmAlloc(tg, dp, pgn, N2K_CM_COMMAND);
	if (n2kMsgOut == NULL)
		return NULL;

	veOutUn8(0xFF);		// do not change priority

	// keep track of the param counter
	paramCount = veOutPtr();
	veOutUn8(0);	/* paramCount */
	return paramCount;
}

/**
 * Allocates and sets the general part of a Complex Request.
 *
 * @param tg		NAD where the message must be addresses to.
 * @param dp		datapage
 * @param pgn		parameter group number
 * @return			pointer to the parameter count in the msg (set to 0)
 *
 * The message is set as output message. @ref n2kMsgOut will be NULL when no
 * message could be allocated. The transmission interval and offset are set
 * to leave as is.
 */
un8* n2kCmAllocReq(J1939Nad tg, un8 dp, un16 pgn)
{
	n2kCmAlloc(tg, dp, pgn, N2K_CM_REQUEST);
	if (n2kMsgOut == NULL)
		return NULL;

	veOutUn32Le(0xFFFFFFFF);			// no interval change
	veOutUn16Le(0xFFFF);				// no offset

	// keep track of the param counter
	paramCount = veOutPtr();
	veOutUn8(0);	/* paramCount */

	return paramCount;
}

/**
 * Adds a parameter to a Complex Request or Complex Command.
 *
 * The parameter is added and byte aligned again. The parameter count is
 * also updated.
 */
void n2kCmParam(un8 paramNr, VeVariant* variant)
{
	(*paramCount)++;
	veOutUn8(paramNr);				// field of interest
	veOutVariantLe(variant);		// value of interest

	// byte align again if bits are left
	if (veStreamOut->bitsUsed)
		veOutBits8Le(0, 8 - veStreamOut->bitsUsed);
}

