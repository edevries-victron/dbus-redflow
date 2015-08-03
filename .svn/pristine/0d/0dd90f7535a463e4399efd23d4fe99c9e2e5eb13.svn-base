#ifndef _VELIB_J1939_J1939_NMT_H_
#define _VELIB_J1939_J1939_NMT_H_

/**
 * @ingroup VELIB_J1939
 * @defgroup VELIB_J1939_NMT NMT
 * @brief
 *	Network management, maintains a list of device on the bus.
 * @{
 */

#include <velib/base/base.h>
#include <velib/J1939/J1939_acl.h>
#include <velib/J1939/J1939_function.h>

/* Internal flags, do not change */
#define J1939_NMT_DEV_RETRY_REQUEST		b00100000
#define J1939_NMT_DEV_INSTANCE_CHANGED	b00010000
#define J1939_NMT_DEV_INACTIVE			b00001000
#define J1939_NMT_DEV_DISCONNECTED		b00000100
#define J1939_NMT_DEV_RETRY_COUNTER		b00000011

/// J1939 / NMEA 2000 device on the bus.
typedef struct
{
	J1939Name			name;		///< NAME of the device.
	un8					flags;		///< Flag for disconnect etc.
	J1939Nad			nad;		///< Current Network address used.
	un8					timer;		///< Disconnect timer.
	/**
	 * ACL timer, if the remote device is performing the ACL procedure
	 * it handle destination specific messages and it might not handle
	 * broadcasted messages. Therefore keep a timer to track it.
	 */
	un8					aclTimer;
} J1939NmtVars;

VE_DCL	void	j1939NmtActivity(J1939Nad nad);
VE_DCL	void	j1939NmtActivityDev(struct J1939Func* dev);
VE_DCL	void	j1939NmtData(void);
VE_DCL	veBool	j1939NmtIsOnBus(struct J1939Func* dev);
VE_DCL	void	j1939NmtTick(void);
VE_DCL	void	j1939NmtUpdate(void);
VE_DCL	void	j1939NmtRebuild(void);

// Optional nmt memory management functions, add J1939_nmt_queue.c to
// your project if you intend to use these functions.
VE_DCL	void	j1939NmtMemInit(void);
VE_DCL	void*	j1939NmtAlloc(void);
VE_DCL	void	j1939NmtFree(void* nmt);

/// @}

#endif
