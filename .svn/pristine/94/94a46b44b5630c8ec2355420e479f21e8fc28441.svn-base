#include <stdio.h>

#include <velib/velib_inc_J1939.h>
#include <velib/base/ve_string.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939_function.h>
#include <velib/J1939/J1939_nmt.h>
#include <velib/J1939/J1939_request.h>
#include <velib/nmea2k/n2k.h>
#include <velib/vecan/reg.h>
#include <velib/vecan/dev_n2k.h>

void veDevN2kUpdate(J1939FuncTp* veDev)
{
	if (!veDev->n2k.request)
		return;

	// request product info.
	if (veDev->n2k.request & VE_DEV_N2K_PROD_INFO)
		if (j1939RequestPgn(veDev->nmt.nad, 1, N2K_PGN1_PRODUCT_INFO))
			veDev->n2k.request &= ~VE_DEV_N2K_PROD_INFO;
}

/*
 * Copies the unterminated 32 char strings from the product info to a
 * destination buffer and removes the padding. If the destination buffer
 * is smaller the first n-1 chars are copied.
 *
 * note: the size of dest, n, includes the terminating 0, to copy X chars
 * n must X+1. n must be at least 1.
 */
static veBool strncpyClean(char* dest, char const* src, size_t n)
{
	if (src == NULL)
		return veFalse;

	ve_snprintf(dest, n, "%.32s", src);
	n--; /* make n and index */
	dest[n] = 0; /* paranoid, make sure zero ended, also when n < 33 */
	while (n && (dest[n] <= 0x20 || (un8) dest[n] == 0xFF))
		dest[n--] = 0;

	return veTrue;
}

/// Handle incoming product info.
void veDevN2kDataProdInfo(void)
{
	J1939FuncTp* veDev;

	veDev = j1939FuncByNad(n2kMsgIn->msgId.p.src);
	if (veDev == NULL)
		return;

	if (veDev->n2k.valid & VE_DEV_N2K_PROD_INFO)
		return;

	veDev->n2k.prod.db = veInUn16Le();
	veDev->n2k.prod.code = veInUn16Le();

	if (!strncpyClean(veDev->n2k.prod.modelId, veInFixedString(32), sizeof(veDev->n2k.prod.modelId) ))
		return;

	if (!strncpyClean(veDev->n2k.prod.firmwareVersion, veInFixedString(32), sizeof(veDev->n2k.prod.firmwareVersion)))
		return;

	if (!strncpyClean(veDev->n2k.prod.modelVersion, veInFixedString(32), sizeof(veDev->n2k.prod.modelVersion)))
		return;

	if (!strncpyClean(veDev->n2k.prod.serial, veInFixedString(32), sizeof(veDev->n2k.prod.serial)))
		return;

	veDev->n2k.prod.level = veInUn8();
	veDev->n2k.prod.load = veInUn8();

	if (veStreamIn->error)
		return;

	veDev->n2k.valid |= VE_DEV_N2K_PROD_INFO;
	veDev->n2k.request &= ~VE_DEV_N2K_PROD_INFO;
}
