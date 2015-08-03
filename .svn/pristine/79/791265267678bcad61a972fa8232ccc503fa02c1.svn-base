#ifndef _VELIB_VECAN_UPD_H_
#define _VELIB_VECAN_UPD_H_

#include <velib/velib_inc_J1939.h>
#include <velib/utils/ve_events.h>
#include <velib/vecan/regs.h>

/**
 * @ingroup VELIB_VECAN
 * @defgroup VELIB_VECAN_UPD Update
 */
/// @{

/// Callback pointer for data to send / receive.
typedef veBool (*UpdDataFunc)(VeEvent ev);

/// State of the update.
typedef enum
{
	UPD_IDLE,			///< Doing nothing.
	UPD_UPDATE_ENABLE,	///< Enable (bootloader e.g.).
	UPD_UPDATE_BEGIN,	///< Periodically send stay in update mode.
	UPD_UPDATE_READY,	///< Ready, waiting for @ref updSend.
		
	UPD_SENDING,		///< Send data.
	UPD_ACK_WAIT,		///< Wait till it is processed.
	UPD_DONE_WAIT,		///< Wait for confirm that upload is complete. 
	UPD_DONE,			///< Upload completed succesfully.

	UPD_SEND_END,		///< Send resume normal operation.
	UPD_ENDED,			///< Resume normal operation has been queued.

	UPD_ERROR			///< Something went wrong, details in updVars.errorCode.
} UpdState;

/// Variables belonging to the update (sending part).
typedef struct
{
	UpdState			state;			///< Progress of update.
	un16				timer;			///< Action timer.
	un16				timeout;		///< Fatal timer.
	UpdDataFunc			dataFunc;		///< Function which set the actual data.
	J1939FuncTp*		udf;			///< Device Function handling the update.
	un8					vupInstance;	///< Chip in / around / behind the UDF to update.
	/// If the applications does not enter boot mode, it can be attempted during a
	/// reset. The device must then be physically restarted.
	veBool				panic;
	VeRegAckCode		errorCode;		///< Error code send by the UDF or local timeout.
} UpdVars;

/// Variables related to sending updates.
extern UpdVars updVars;

veBool	updBegin(J1939FuncTp* dev, un8 instance, veBool panic);
void	updEnd(void);
void	updHandler(VeEvent ev);
veBool	updSend(UpdDataFunc dataFunc);
veBool	updSendUpdateBegin(J1939Name* name, veBool panic);
veBool	updSendUpdateEnd(J1939Nad nad);

/// @}

#endif
