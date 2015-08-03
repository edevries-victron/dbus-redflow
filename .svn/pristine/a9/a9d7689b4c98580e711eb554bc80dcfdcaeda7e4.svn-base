#ifndef _VELIB_J1939_J1939_ACL_H_
#define _VELIB_J1939_J1939_ACL_H_

/**
 * @ingroup VELIB_J1939
 * @defgroup VELIB_J1939_ACL ACL
 * @brief
 *	ACL procedure to obtain network address.
 * @{
 */

#include <velib/base/base.h>
#include <velib/J1939/J1939_types.h>
#include <velib/J1939/J1939_sf.h>

// Flags of the ACL-procedure
/// When the flag is set the application should store the address and clear the flag.
#define J1939_ACL_STORE_NAD			0x01
/// Read only, whether a valid network address is obtained. Do not change!
#define J1939_ACL_VALID_NAD			0x02
/// Set when the current address is lost, the application should directly stop sending
/// messages on the old address. The flag must be cleared by the application.
#define J1939_ACL_LOST				0x04
/// @internal ACL send is pending. Do not change!
#define J1939_ACL_SEND				0x08
/// gatways might do an ACL themselves, the internal ACL should then be disabled.
#define J1939_ACL_DISABLED			0x10
/// Send as soon as the address claim is succesful.
/// Used when an ACL request is received during the address claim procedure.
#define J1939_ACL_SEND_WHEN_CLAIMED	0x20

// Special network addresses
/// Largest valid claimable address.
#define J1939_NAD_MAX				0xFB
/// Could not claim address
#define J1939_NAD_NONE				0xFE
/// Broadcast message NAD
#define J1939_NAD_BROADCAST			0xFF

// Macro's to compose a NAME
/// construct high NAME -> set self configurable address.
#define J1939_ACL_H_SCA						(0x80000000)
/// construct high NAME -> set industry group
#define J1939_ACL_H_INDUSTRY(x)				((un32) ((x) & 0x7) << 28)
/// construct high NAME -> set class instance
#define J1939_ACL_H_CLASS_INST(x)			((un32) ((x) & 0xF) << 24)
/// construct high NAME -> set device class.
#define J1939_ACL_H_CLASS(x)				((un32) ((x) & 0x7F) << 17)
/// construct high NAME -> set reserved, must be 0
#define J1939_ACL_H_RESERVED(x)				0
/// construct high NAME -> set device function
#define J1939_ACL_H_FUNCTION(x)				((un16) ((x) & 0xFF) << 8)
/// construct high NAME -> set device function instance
#define J1939_ACL_H_FUNCTION_INST(x)		(		((x) & 0x1F) << 3)
/// construct high NAME -> set system instance (for redundancy)
#define J1939_ACL_H_ECU_INST(x)				(		 (x) & 0x7)

/// construct low NAME -> set manufacturer code
#define J1939_ACL_L_MAN(x) 					((un32)  (x) << 21)
/// construct low NAME -> set manufacturer identity number
#define J1939_ACL_L_IDENTITY_NUMBER(x)		((un32)  (x) & 0x1FFFFF)

// Macro's to extract information from a NAME.
#define J1939_ACL_L_GET_MAN(x)				((x) >> 21)
#define J1939_ACL_L_GET_IDENTITY_NUMBER(x)	((x) & 0x1FFFFF)
#define J1939_ACL_H_GET_CLASS_INST(x)		((x) >> 24)
/**
 * NMEA 2000 combines, function instance an ecu instance. Perhaps not the best
 * place but at least these are defined together then.
 */
#define N2K_ACL_H_GET_DEVICE_INST(x)		((x).vun32 & 0xFF)
/// For clarity, NMEA 2000 redefines the class instance
#define N2K_ACL_H_GET_SYSTEM_INST(x)		J1939_ACL_H_GET_CLASS_INST(x.vun32)
/// Reserved timer value
#define J1939_ACL_TMR_OFF					0xFF

#if	VE_LITTLE_ENDIAN

/// The 'High' part of the NAME; the last 4 bytes of the ACL message.
typedef struct
{
	un8 ecuInst  : 3;			/**< ECU instance 						LSB		1	*/
	un8 funcInst : 5;			/**< function instance 							2	*/
	un8 function : 8;			/**< function field								3	*/
	un8	reserved : 1;			/**< reserved field should be set to 0			4	*/
	un8	devClass : 7;			/**< device class								5	*/
	un8 dcInst   : 4;			/**< device class instance						6	*/
	un8 indGroup : 3;			/**< industry group								7	*/
	un8 sca      : 1;			/**< self configurable address 			MSB		8	*/
} J1939NameHighFields;

#else

/// The 'High' part of the NAME; the last 4 bytes of the ACL message.
typedef struct
{
	un8	dcInst   : 4;			/**< device class instance						3	*/
	un8 indGroup : 3;			/**< industry group 							2	*/
	un8 sca      : 1;			/**< self configurable address 			MSB		1	*/
	un8 reserved : 1;			/**< reserved field should be set to 0 			5	*/
	un8 devClass : 7;			/**< device class 								4	*/
	un8 function : 8;			/**< function field 							6	*/
	un8 ecuInst  : 3;			/**< ECU instance 						LSB		8	*/
	un8	funcInst : 5;			/**< function instance 							7	*/
} J1939NameHighFields;

#endif

/// The 'High' part of the NAME as number or individual fields.
typedef union
{
	un32 				vun32;
	J1939NameHighFields	fields;
} J1939NameHigh;

/// The complete 8 byte NAME as contained in the ACL.
typedef struct
{
	J1939NameHigh 	high;
	/// int only, 8 bit device cannot cope with 11 / 21 bit fields.
	/// Macro's are defined to get the manufactur / identifier.
	un32 			low;
} J1939Name;

typedef enum
{
	J1939_ACL_IDLE,					/**< No bus access and no intention to get it */
	J1939_ACL_CANNOT_CLAIM,			/**< No bus access, all addresses used. */
	J1939_ACL_RANDOM_DELAY,			/**< No bus access */
	J1939_ACL_MON_TRANSMIT,			/**< Verifying if ACL caused error / was send ok */
	J1939_ACL_WAITING_FOR_RESPONSE, /**< Waiting for objection for 250 ms */
	J1939_ACL_SUCCESSFUL			/**< Address claimed, on bus */
} J1939AclState;

typedef enum
{
	J1939_ACL_EV_TX_ERR,	/**< @internal sending ACL caused bus error */
	J1939_ACL_EV_TX_OK,		/**< @internal sending ACL ok */
	J1939_ACL_EV_REQ,		/**< public, answer ACL request (resend ACL) */
	J1939_ACL_EV_TMR,		/**< internal by default, requested interval has expired */
	J1939_ACL_EV_WON,		/**< @internal local device WON the address challenge */
	J1939_ACL_EV_LOST		/**< @internal local device LOST the address challenge */
} J1939AclEvent;

/// Reassuring an address is the same behaviour as after a transmission error of
/// the ACL. Not all compilers appreciate enum with repeating values, hence it is
/// defined to be equal.
#define J1939_ACL_EV_ASSURE	J1939_ACL_EV_TX_ERR

/// Contains the ACL information for a local device
typedef struct
{
	/// @protected 	Stored start NAD to detect complete loop.
	J1939Nad		nadFirst;
	/// @protected 	Address claim timeout counter.
	un8				timer;
#if !CFG_J1939_VELIB_TIMER_DISABLED
	/// @protected 	Start time of the delay.
	un16			timerStart;
#endif
	/// @protected	Timer to send ACL messages periodically at a low rate.
	un16			timerBroadcast;
	/// state of the ACL procedure
	J1939AclState	state;
} J1939AclVars;

VE_DCL	void	j1939AclInit(void);
VE_DCL	void	j1939AclDetectConflict(J1939Id const *id);
VE_DCL	void	j1939AclEnable(void);
VE_DCL	void	j1939AclUpdate(void);
VE_DCL	void	j1939AclUpdateState(J1939AclEvent aclEvent);
VE_DCL	veBool	j1939AclSend(void);
VE_DCL	void	j1939AclTick(void);
VE_DCL	void	j1939AclReadName(J1939Name* name);
VE_DCL	void	j1939AclData(void);
VE_DCL	void	j1939AclChangeNad(J1939Nad cmndAddress);
VE_DCL	sn8		j1939AclCompare(J1939Name const * high, J1939Name const * low);

VE_DCL	void	j1939AclSetTmr(un8 ms);
VE_DCL	veBool	j1939MsgForDevice(J1939Id const* id);

/// @}

/**
 * @ingroup VELIB_J1939
 * @defgroup VELIB_J1939_CMDA	CMDA
 * Handles an incoming commanded address.
 */
/// @{
void	j1939DataCmdNad(void);
/// @}

#endif
