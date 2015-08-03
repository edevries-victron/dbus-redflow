#ifndef _VELIB_J1939_J1939_TX_H_
#define _VELIB_J1939_J1939_TX_H_

/**
 * @ingroup VELIB_J1939
 * @defgroup VELIB_J1939_TX TX-queue
 * @{
 */

#include <velib/J1939/J1939_types.h>

typedef struct
{
	J1939Msg		*begin;			/* first message to be send	 */
	J1939Msg		*end;			/* last message to be send */
} J1939TxVars;

VE_DCL veBool		j1939TxEmpty();
VE_DCL J1939Msg*	j1939TxPop(void);
VE_DCL void			j1939TxPush(J1939Msg* msg);
VE_DCL void			j1939TxPushFront(J1939Msg* msg);

/// @}

#endif
