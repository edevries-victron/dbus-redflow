#ifndef _VELIB_CANHW_CANHW_STATS_H_
#define _VELIB_CANHW_CANHW_STATS_H_

/**
 * @ingroup VELIB_CANHW
 * @defgroup VELIB_CANHW_STATS Stats
 * @brief 
 *	Bus statistics counter.
 * @{
*/

#include <velib/base/base.h>
#include "canhw_types.h"

typedef struct
{
	un32 txOK;				// succesfull transmission
	un32 rxOK;				// succesfull reception

	un32 errorFrame;		// a remote device signalled an error
	un32 rxOverrun;			// this device signalled an error
	un32 bitError;			// sum of the bit errors

} VeBusStats;

void veBusStatsInit(VeBusStats* stats);
void veBusStatsRx(VeBusStats* stats, VeCanFlags flags);
void veBusStatsTx(VeBusStats* stats);

/// @}

#endif
