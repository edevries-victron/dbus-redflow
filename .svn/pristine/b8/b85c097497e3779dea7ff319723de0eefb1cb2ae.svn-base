/**
 * @addtogroup VELIB_VECAN_REG_PROD_DATA
 *
 * Broadcast SF Victron proprietary data.
 *
 * Since all nodes on a CAN-bus can gain bus-access and send requests, the total
 * number of simultanious requests cannot be controlled from within a device.
 * In order to make sure the device is always capable to respond to valid request, 
 * the messages are marked for sending, instead of sending them directly.
 *
 * Since this part is also used in bootloaders, functionality must remain limited.
 * This code only allows sending single frames. There is no io, the caller must set
 * the buffers to the correct Endianness etc. There is no support for periodic 
 * broadcast, fastpackets, bit fields etc.
 *
 * <b>Requirements</b>:
 *	- definition of the array:
 *		VeRegProdSfDef veRegProdSfDef[] with messages to send.
 *	- veRegProdSfFlags with equal size of RAM.
 *	- veRegProdSfInit must be called
 *	- veRegProdSfHandler must be added to rx reg handler
 *	- either veRegProdSfUpdate must be called or indirectly by veRegProdSfHandler.
 */

#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_acl.h>
#include <velib/nmea2k/n2k.h>
#include <velib/vecan/vecan.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/reg_rx.h>
#include <velib/vecan/reg_prod_sf.h>
#include <velib/utils/ve_todo.h>

#include <string.h> //	memcpy

#define VE_REG_PROD_SF_REQ					0x01

// proto for actually sending the message.
static veBool veRegProdSfSend(VeRegNr nr);

static VeRegNr regNr;
static VeRegNr regCount;

/// Initialises the sf broadcast not to send by default.
void veRegProdSfInit(void)
{
	regCount = 0;

	while(veRegProdSfDef[regCount].data != NULL)
	{
		veRegProdSfFlags[regCount] = 0;
		regCount++;
	}
	regNr = 0;
}

/// Continue to send broadcast proprietary sf messages.
void veRegProdSfUpdate(void)
{
	if (++regNr >= regCount)
		regNr = 0;

	if ((veRegProdSfFlags[regNr] == VE_REG_PROD_SF_REQ) && veRegProdSfSend(regNr))
	{
		veRegProdSfFlags[regNr] = 0;
		veTodo();
	}
}

/// Mark a message as being requested.
void veRegProdSfReq(VeRegId regId, un16 mask)
{
	VeRegNr n;

	for (n = 0; n < regCount; n++)
	{
		// Mark the message for sending.
		if ((veRegProdSfDef[n].regId & mask) == regId) 
		{
			veRegProdSfFlags[n] = VE_REG_PROD_SF_REQ;
			veRegRxHandled = veTrue;
		}
	}
}

/** 
 * Try to send a messages.
 *
 * @param nr
 *	The index in the definition of the message to send. 
 *	Note: if the message contains multiple field, the first occurence
 *	should be passed.
 */
static veBool veRegProdSfSend(VeRegNr nr)
{
	// Prepare a message.
	(void)veRegPrepareSf(J1939_NAD_BROADCAST, veRegProdSfDef[nr].regId);
	if (n2kMsgOut == NULL)
		return veFalse;

	// Add data.
	(void)memcpy(&n2kStreamOut.buffer[4], veRegProdSfDef[nr].data, 4);
	n2kStreamOut.index = 8;
	
	veOutSendSf();

	return veTrue;
}

/**
 * Handle incoming VREG requests.
 *
 * This function should be added to @ref veRegRxHndlrDef.
 */
void veRegProdSfHandler(VeEvent ev)
{
	if (ev == VE_EV_UPDATE)
	{
		veRegProdSfUpdate();
	} else if (ev == VE_EV_VREG_RX_REQ) 
	{
		veRegProdSfReq(veRegRxId, veRegReqMask);
	}
}
