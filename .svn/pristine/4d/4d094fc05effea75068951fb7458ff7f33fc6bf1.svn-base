#include <velib/J1939/J1939_types.h>
#include <velib/nmea2k/n2k.h>
#include <velib/nmea2k/n2k_fp.h>

/**
 * @retval If the identifier must be reconstructed as a Fast Packet.
 */
veBool n2kIsFp(J1939Id const* id)
{
	if (id->p.pg.p.dp == 1)
	{
		// all propriatary pgn's in datapage 1 are fast packets.
		if (id->p.pf == N2K_PGN1_PF_PROPRIETARY_PDU2)
			return veTrue;

		switch (J1939PgnFromIdNoTg(*id))
		{
		case N2K_PGN1_PROPRIETARY_PDU1:
		case N2K_PGN1_COMPLEX_GROUP:
		case N2K_PGN1_PGN_LIST:
		case N2K_PGN1_PRODUCT_INFO:
		case N2K_PGN1_CONFIG_INFO:
		case N2K_PGN1_AC_INPUT_STATUS:
		case N2K_PGN1_DC_DETAILS:
		case N2K_PGN1_CHARGER_STATUS:
		case N2K_PGN1_INVERTER_STATUS:
		case N2K_PGN1_CHARGER_CFG_STATUS:
			return veTrue;
		}
	}

	return veFalse;
}
