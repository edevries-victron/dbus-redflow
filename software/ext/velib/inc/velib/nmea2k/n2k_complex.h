#ifndef _VELIB_NMEA2K_COMPLEX_H_
#define _VELIB_NMEA2K_COMPLEX_H_

#include <velib/velib_config.h>
#include <velib/J1939/J1939_acl.h>
#include <velib/types/variant.h>

/**
 * @ingroup VELIB_NMEA2K
 * @defgroup VELIB_NMEA2K_COMPLEX Complex Group Function
 * @brief
 *	Construction of Complex Function messages.
 * @details
 *	Since device typically either send or receive Complex Commands / Request
 *	devices it is implemented in a seperate files, n2k_complex_tx.c and 
 *	n2k_complex_rx.c. Overlapping functions in n2k_complex.c. All can be 
 *	included.
 */
/// @{

/// param 1, Opcode to select a particular Complex Function
typedef enum
{
	N2K_CM_REQUEST			= 0x00,
	N2K_CM_COMMAND			= 0x01,
	N2K_CM_ACK				= 0x02,
	N2K_CM_READ				= 0x03,
	N2K_CM_READ_REPLY		= 0x04,
	N2K_CM_WRITE			= 0x05,
	N2K_CM_WRITE_REPLY		= 0x06
} N2kCmType;

// param 2 is always the PGN.

//== COMPLEX REQUEST ==

/// Complex Request, param 3 transmission interval
#define N2K_CM_REQ_INTERVAL_AS_IS	0xFFFFFFFFL
#define N2K_CM_REQ_INTERVAL_RESTORE	0xFFFFFFFEL

/// Complex Request, param 4 transmission offset
#define N2K_CM_REQ_OFFSET_AS_IS		0xFFFFL


//== COMPLEX COMMAND ==

/// Complex Command, param 3 requested priority 
typedef enum
{
	N2K_CM_PRIORITY0,			// highest
	N2K_CM_PRIORITY1,
	N2K_CM_PRIORITY2,
	N2K_CM_PRIORITY3,
	N2K_CM_PRIORITY4,
	N2K_CM_PRIORITY5,
	N2K_CM_PRIORITY6,
	N2K_CM_PRIORITY7,			// lowest
	N2K_CM_PRIORITY_UNCHANGED,
	N2K_CM_PRIORITY_RESET
} N2kCmPriCode;

// Complex Command, param 4 reserved 

//== COMPLEX ACK ==

/// Complex Ack, param 3, PGN error code
typedef enum
{
	N2K_CM_PGN_NO_ERROR,
	N2K_CM_PGN_NO_SUPPORT,
	N2K_CM_PGN_TEMP_UNAVAIL,
	N2K_CM_PGN_ACCESS_DENIED,
	N2K_CM_PGN_REQ_NOT_SUPPORTED
} N2kCmPgnCode;


/// Complex Ack, param 4, transmission / priority error
typedef enum
{
	N2K_CM_TRANS_NO_ERROR,
	N2K_CM_TRANS_PRI_INTERVAL_NOT_SUPPORTED,
	N2K_CM_TRANS_INTERVAL_TOO_LOW,
	N2K_CM_TRANS_PRI_ACCESS_DENIED,
	N2K_CM_TRANS_REQ_NOT_SUPPORTED
} N2kCmTransCode;

// Complex Ack, param 5. Number of argument errors.

/// Complex Ack, param 6 and higher, paramater error
typedef enum
{
	N2K_CM_PARAM_NO_ERROR,
	N2K_CM_PARAM_INVALID,
	N2K_CM_PARAM_TEMP_UNAVAILABLE,
	N2K_CM_PARAM_OUT_OF_RANGE,
	N2K_CM_PARAM_ACCESS_DENIED,
	N2K_CM_PARAM_NOT_SUPPORTED
} N2kCmParamCode;

/*
typedef enum
{
	N2K_CM_WAITING_FOR_ACK,
	N2K_CM_WAITING_FOR_ACK_OR_DATA,
	N2K_CM_ACKED,
	N2K_CM_ERROR_INVALID_REQ	= 0x40,		///< Do not try again
	N2K_CM_ERROR_TEMP,						///< Try again later
	N2K_CM_ERROR_TIMEOUT					// Should not happen!
} eVeCmState;
*/


VE_DCL	void	n2kCmAlloc(J1939Nad tg, un8 dp, un16 pgn, N2kCmType cmType);
VE_DCL	un8*	n2kCmAllocCmd(J1939Nad tg, un8 dp, un16 pgn);
VE_DCL	un8*	n2kCmAllocReq(J1939Nad tg, un8 dp, un16 pgn);
/// Allocates and sets the general part of a Complex Ack.
/// @see n2kCmAlloc
#define			n2kCmPrepareAck(tg, dp, pgn) n2kCmAlloc(tg, dp, pgn, N2K_CM_ACK)
VE_DCL	void	n2kCmParam(un8 paramNr, VeVariant* variant);
VE_DCL	void	n2kCmSend(void);

/// @}

#endif
