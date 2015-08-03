#ifndef _VELIB_J1939_J1939_H_
#define _VELIB_J1939_J1939_H_

/**
 * @ingroup VELIB
 * @defgroup VELIB_J1939 J1939
 * @brief 
 *	Support for the J1939 - higher level CANbus protocol.
 * @details
 *	J1939 is used for (duty) vehicles. Many diesel engines / gensets speak J1939. 
 *	Specifications are maintained by the Society of Automative Engineers, SAE.
 *	NMEA2000 and ISOBUS are derived from J1939. NMEA2000 is supposed to be 
 *	compatible with the J1939 protocol.
 * 
 *	J1939 relies on single CAN bus frames for normal operation. Maintance and 
 *	diagnostics might use the RTS/CTS or BAM transport protocol.
 *
 *	The library code uses the same approach, NMEA 2000 extends the J1939. All 
 *	messages are derived from @ref J1939Msg which itself might be derived from
 *	the @ref VeRawCanMsg.
 *
 * @{
 */

#include <velib/velib_config.h>
#include <velib/J1939/J1939_types.h>
#include <velib/J1939/J1939_constants.h>

/// Must be implemented at higher levels, depends on message types supported.
VE_DCL		void 		j1939RxFree(J1939Msg *msg);
/// Must be implemented at higher levels, depends on message types supported.
VE_DCL		J1939Msg* 	j1939TxAlloc(un8 type);
/// Must be implemented at higher levels, depends on message types supported.
VE_DCL		void 		j1939TxFree(J1939Msg* msg);
/// Must be implemented at higher levels, depends on message types supported.
VE_DCL		void 		j1939Free(J1939Msg* msg);

/// @}

#endif
