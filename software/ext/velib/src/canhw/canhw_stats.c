#include <memory.h>

#include <velib/canhw/canhw_stats.h>
#include <velib/canhw/canhw.h>

void veBusStatsInit(VeBusStats* stats)
{
	memset(stats, 0, sizeof(*stats));
}

void veBusStatsRx(VeBusStats* stats, VeCanFlags flags)
{
	if (flags & (VE_CAN_STD | VE_CAN_EXT))
		stats->rxOK++;

	if (flags & VE_CAN_RX_OVERRUN)
		stats->rxOverrun++;

	/*
	if (flags & VE_CAN_MSG_BIT)
		stats->bitError++;

	if (flags & VE_CAN_MSG_ERROR )
		stats->errorFrame++;
	*/
}

void veBusStatsTx( VeBusStats* stats)
{
	stats->txOK++;
}
