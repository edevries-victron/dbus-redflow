#ifndef _VELIB_J1939_J1939_RX_H_
#define _VELIB_J1939_J1939_RX_H_

/**
 * @ingroup VELIB_J1939
 * @defgroup VELIB_J1939_RX RX-queue
 * @{
 */

#include <velib/J1939/J1939_types.h>

/** Variables used for the rx queue. Must be added to @ref j1939Stack as rx */
typedef struct
{
	/** pointer to top of receive msg queue	*/
	J1939Msg		*begin;
	/** pointer to bottom of receive msg queue */
	J1939Msg		*end;
} J1939RxVars;

VE_DCL	J1939Msg* 	j1939RxPop(void);
VE_DCL	void 		j1939RxPush(J1939Msg* msg);
VE_DCL	void		j1939RxPushFront(J1939Msg* msg);

/// @}

#endif
