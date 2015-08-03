#ifndef _VELIB_J1939_CONSTANTS_H_
#define _VELIB_J1939_CONSTANTS_H_

/** 
 * Known J1939 PGNs. 8 bitters might not like a un16 as enum type,
 * hence these values are now defined.
 */
typedef un16 J1939PgnPage0;

#define	J1939_PGN0_ACK			0xE800
#define	J1939_PGN0_REQUEST		0xEA00
#define	J1939_PGN0_TP_DATA		0xEB00
#define	J1939_PGN0_TP_CTSG		0xEC00
#define	J1939_PGN0_ACL			0xEE00
#define	J1939_PGN0_CMD_NAD		0xFED8

#define	J1939_PGN0_UTIL_UTACE	0xFDED	///< Utility, Total AC Energy

#define	J1939_PGN0_UTIL_UPCACR	0xFDEE	///< Phase C, AC Reactive Power
#define	J1939_PGN0_UTIL_UPCACP	0xFDEF	///< Phase C, AC Power
#define	J1939_PGN0_UTIL_UPCAC	0xFDF0	///< Phase C, AC Voltage, AC Current, AC frequency
	
#define	J1939_PGN0_UTIL_UPBACR	0xFDF1	///< Phase B, AC Reactive Power
#define	J1939_PGN0_UTIL_UPBACP	0xFDF2	///< Phase B, AC Power
#define	J1939_PGN0_UTIL_UPBAC	0xFDF3	///< Phase B, AC Voltage, AC Current, AC frequency

#define	J1939_PGN0_UTIL_UPAACR	0xFDF4	///< Phase A, AC Reactive Power
#define	J1939_PGN0_UTIL_UPAACP	0xFDF5	///< Phase A, AC Power
#define	J1939_PGN0_UTIL_UPAAC	0xFDF6	///< Phase A, AC Voltage, AC Current, AC frequency

#define	J1939_PGN0_UTIL_UTACR	0xFDF7	///< Total AC Reactive Power
#define	J1939_PGN0_UTIL_UTACP	0xFDF8	///< Sum of AC Power
#define	J1939_PGN0_UTIL_UAAC	0xFDF9	///< Average AC Voltage, AC Current, AC frequency

/* End of known J1939 PGNs. */

/// Known J1939 PGNs page formats.
typedef enum
{
	J1939_PGN0_PF_ACK		=	0xE8,
	J1939_PGN0_PF_REQUEST	=	0xEA,
	J1939_PGN0_PF_TP_DATA	=	0xEB,
	J1939_PGN0_PF_TP_CTSG	=	0xEC,
	J1939_PGN0_PF_ACL		=	0xEE
} J1939PgnPfPage0;

/// Known industry groups.
typedef enum
{
	J1939_INDUSTRY_GLOBAL,
	J1939_INDUSTRY_ON_HIGHWAY,
	J1939_INDUSTRY_ARGRICULTURAL,
	J1939_INDUSTRY_CONSTRUCTIION,
 	J1939_INDUSTRY_MARINE,
	J1939_INDUSTRY_INDUSTRIAL
} J1939IndustryGroup;

#endif
