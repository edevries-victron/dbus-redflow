/**
 * @addtogroup VELIB_NMEA2K_PRODUCT
 * @brief Product info without FP out.
 * @details
 * The product information is a long message describing model, firmware
 * etc. There are 4 such fields containing 32 characters each. These
 * string are not zero ended!. These field are therefore not used to search
 * or check if a device is up to date, since it would necessitate string 
 * manipulation functions and to check which is newer is rather ambivalent. 
 * Furthermore it would need outgoing fast packet support in the bootloaders
 * as well. Bootloaders therefore don't need to send this frame.
 *
 * That said, the product information is a required pgn by NMEA 2000. It is
 * therefore polite to send it if possible. This code tries to minimize the
 * needed code for that by not requiring the fast packet protocol, the counter
 * are set here. The following constants must be defined:
 *
 * - @ref CFG_N2K_CERT_LEVEL
 * - @ref CFG_N2K_LOAD_EQUIV
 * - @ref CFG_N2K_DB
 * - @ref CFG_N2K_PROD_CODE
 *
 * @ref veProductInfoDef must point to the four strings. These must be zero ended.
 * Multiple trailings zeros are allowed.
 *
 * @code
 *	{
 *	'm','o','d','e','l','#',0,
 *	'v','e','r','s','i','o','n','#',0,
 *	'r','e','v','#',0,
 *	's','e','r','i','a','l','#',0
 *	};
 * @endcode
 */

#include <velib/base/base.h>

#include <velib/io/ve_stream_n2k.h>
#include <velib/nmea2k/n2k.h>
#include <velib/nmea2k/n2k_fp.h>
#include <velib/nmea2k/n2k_product.h>

// properties of the message format
#define PROD_LENGTH 			0x86
#define STRING_LENGTH 			32
#define LAST_MSG_SEQ			0x13

#define PROD_REQ	0x01
#define PROD_SEND	0x02

static un8 flags;
static un8 chars;
static un8 const * data;
static un8 sequence;

/// Definition of the product info, which must be set by the application.
un8 const * veProductInfoDef;

/// Init
void n2kProdInit(void)
{
	flags = 0;
	sequence = 0;
}

/// Continue to send product info if marked.
void n2kProdUpdate(void)
{
	if (!(flags & PROD_SEND))
		return;

	veOutAllocSf();
	if (n2kMsgOut == NULL)
		return;

	n2kMsgOut->msgId.w = J1939ID_PRI(6) | J1939ID_DP(1) | J1939ID_PGN(N2K_PGN1_PRODUCT_INFO);
	veOutUn8(sequence);

	// last frame
	if ((sequence & N2K_FP_PDU_COUNTER) == LAST_MSG_SEQ)
	{
		veOutUn8(CFG_N2K_CERT_LEVEL);
		veOutUn8(CFG_N2K_LOAD_EQUIV);
		sequence += 0x20 - LAST_MSG_SEQ;
		if (flags & PROD_REQ) 
		{
			flags &= ~PROD_REQ;
		} else {
			flags &= ~PROD_SEND;
		}
	} else {

		// first frame
		if ((sequence & 0x1F) == 0)
		{
			if (flags & PROD_REQ) {
				flags &= ~PROD_REQ;
			}
			veOutUn8(PROD_LENGTH);
			veOutUn16Le(CFG_N2K_DB);
			veOutUn16Le(CFG_N2K_PROD_CODE);
			chars = STRING_LENGTH;
			data = veProductInfoDef;
		}

		// add string data
		while(veStreamOut->index < 8)
		{
			if (*data != 0)
			{
				// output the string
				veOutUn8(*data++);
			} else {
				if (chars)
				{
					// tolerate multiple 0
					if (data[1] == 0)
						data++;

					// pad with spaces
					veOutUn8(' ');
				} else {
					// padding done -> next strings
					data++;
					chars = STRING_LENGTH + 1; // +1 for chars--
				}
			}
			chars--;
		}
		sequence++;
	}

	veOutSendSf();
}

/// Mark product info for sending.
void n2kProdReq(void)
{
	flags |= PROD_REQ | PROD_SEND;
}
