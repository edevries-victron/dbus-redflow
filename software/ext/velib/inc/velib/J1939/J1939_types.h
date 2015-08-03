#ifndef _VELIB_J1939_TYPES_H_
#define _VELIB_J1939_TYPES_H_

#include <velib/base/base.h>
#include <velib/canhw/canhw_types.h>

/// Single Frame message (8 bytes of data)
#define J1939_SF		0x01
/// Transport Protocol message (1783 bytes of data) 
#define J1939_TP		0x02

#define J1939_RX		0x00
#define J1939_TX		0x08

#define J1939_SF_RX		(J1939_SF | J1939_RX)
#define J1939_SF_TX		(J1939_SF | J1939_TX)

#define J1939_TP_RX		(J1939_TP | J1939_RX)
#define J1939_TP_TX		(J1939_TP | J1939_TX)

typedef union 
{
	un8 w;
	struct
	{
		un8 kind		: 3;	// Single frame, transport etc
		un8 tx			: 1;	// indicates that the msg should be send to the bus.
		un8 level		: 3;	// Priority level to allow / disallow allocation.
		un8 reserved	: 1;	// 
	} p;
} J1939MsgType;

typedef un8	J1939Nad;

/// @brief 4th byte of the J1939 CANid
typedef union 
{
	/// the 4th byte in parts (p)
	struct 
	{
		/// data page 
		un8 dp	: 2;
		/// priority
		un8 pri	: 3;
		/// not used
		un8 nu	: 3;
	} p;
	/// the whole (w) 4th byte
	un8 w;
} J1939Page;

/** 
 * PGN (parameter group number) as un16. Since the PGN are the two middle bytes
 * of the CAN bus identifier, this 
 */
typedef un16 J1939Pgn;
typedef un32 J1939MsgNr;

/// Combines pf, ps to create the un16 PGN.
#define J1939PgnPfPs(_pf,_ps)		((((un16)(_pf))<<8)|(_ps))
/// Combines pf, ps to create the un16 PGN, sets ps to 0 when target specific.
#define J1939PgnPfPsNoTg(_pf,_ps)	(J1939PgnPfPs((_pf),(_pf) <=0xEF ? 0 : (_ps)))

/// Returns the PGN (pf, ps) from a CAN identifier.
#define J1939PgnFromId(_id)			(J1939PgnPfPs((_id).p.pf,(_id).p.ps))
/// Returns the PGN (pf, ps) from a CAN identifier.
#define J1939PgnFromIdNoTg(_id)		(J1939PgnPfPsNoTg((_id).p.pf,(_id).p.ps))
/**
 * Returns the PGN (dp, pf, ps) from a CAN identifier. Easier for larger device,
 * since dp has not to be checked seperately, but larger on 16 bitters, since 
 * it is a 17bit value (and can better test the bit and use 16bit PGN).
 * Nomenclature: J1939 refers to the (pf,ps) as PGN, and by defintion NMEA 2000
 * does the same, in practice NMEA 2000 PGN refer to dp,pf,ps. To discriminate it
 * the dp,pf,ps is named MsgNr.
 */
#define J1939MsgNrFromId(_id)		(((_id).w >> 8) & 0x1FFFF)
#define J1939MsgNrFromIdNoTg(_id)	((_id).p.pf <= 0xEF ? ((_id).w >> 8) & 0x1FF00 : ((_id).w >> 8) & 0x1FFFF)

#define J1939MsgNrToDp(_msgNr)		(((_msgNr) >> 16) & 1)
#define J1939MsgNrToPgn(_msgNr)		((_msgNr) & 0xFFFF)

#if VE_LITTLE_ENDIAN

/// The CAN-identifier split in their respective parts.
typedef struct
{
	J1939Nad 	src;	///< source address [ 1st byte, LSB ]
	un8			ps;		///< page specific: pf <= 0xFE target address, extender of pf otherwise
	un8			pf;		///< page format <= 0xFE destination specific, broadcast otherwise
	J1939Page	pg;		///< page, extended page and priority [ 4th byte, MSB ] 
} J1939IdParts;

#else

/// The CAN-identifier split in their respective parts.
typedef struct
{
	J1939Page	pg;		///< page, extended page and priority [ 4th byte, MSB ] 
	un8			pf;		///< page format <= 0xFE destination specific, broadcast otherwise
	un8			ps;		///< page specific: pf <= 0xFE target address, extender of pf otherwise
	J1939Nad	src;	///< source address [ 1st byte, LSB ]
} J1939IdParts;

#endif

typedef union
{
	J1939IdParts	p;	/**< CAN identifier broken into components */
	un32			w;	/**< CAN identifier as a whole for numeric compare */
} J1939Id;

#define DUMMY_LENGTH 		1000
/** 
 * A general definition of a messages.
 * @note 
 *		There should never be an instance of this struct. 
 * 		It is only used for casting down to the common part.
 * @extends VeRawCanMsg
 */
typedef struct _J1939Msg
{
	J1939Id  			msgId; 					///< PGN describing data contents
	VeCanLength			length;					///< Length of the data.
	VeCanFlags			flags;					///< flags for special purposes.
	J1939MsgType		type; 					///< whether sf, fp or tp
	struct _J1939Msg*	next; 					///< next message when in a list
#if CFG_CAN_MSG_EXTRA
	VeCanMsgExtra extra;						///< additional fields which can be added to a msg
#endif
	un8					mdata[DUMMY_LENGTH];	///< actual data
} J1939Msg;

#define J1939ID_SRC(x)	((un32)((x) & 0xFF))
#define J1939ID_TG(x)	((un32)((x) & 0xFF) << 8)
#define J1939ID_PGN(x)	((un32)((un16)(x) & 0xFFFF) << 8)
#define J1939ID_DP(x)	((un32)((x) & 0x3) << 24)
#define J1939ID_PRI(x)	((un32)((x) & 0x7) << 26)

#endif
