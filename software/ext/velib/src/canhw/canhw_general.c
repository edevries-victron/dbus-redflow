#include <stdio.h>

#include <velib/canhw/canhw.h>	// defines what to include
#if CFG_CANHW_KVASER
#include <canlib.h>
#endif

#include <velib/base/ve_string.h>

#include <velib/canhw/canhw_driver.h>

#include <velib/canhw/canhw_general.h>
#include <velib/canhw/canhw_kvaser.h>
#include <velib/canhw/canhw_lincan.h>
#include <velib/canhw/canhw_skcan.h>
#include <velib/utils/ve_logger.h>

static VeBusId activeBus;
VeBusStats veBusStats[CFG_CANHW_MAX_BUSSES];

/* !! THIS FILE WILL BE REPLACED BY CANHW-DRIVER !! */

// All supported drivers; only kvaser for now.
typedef enum
{
	VE_CAN_DRV_KVASER,
	VE_CAN_DRV_LIN_CAN,
	VE_CAN_DRV_SK_CAN,
	VE_CAN_DRV_NULL
} VeCanDrivers;

// Just a long way of saying that the channels are ints now....
// but allows adding different identifiers for different drivers.
typedef union
{
#if CFG_CANHW_KVASER
	canHandle   kv;
#endif
	int		 lc;
	int		 sk;
} VeCanChannel;

typedef enum
{
	Disconnected,
	Connected
} VeCanState;

typedef struct
{
	VeCanState		connected;
	VeCanDrivers 	driver;
	VeCanChannel	channel;
} VeCanConnection;


static VeCanConnection conn[CFG_CANHW_MAX_BUSSES];

/// Implements @ref veCanInit
void veGeCanInit(void)
{
	un8 n;
	VeCanDriver* drv;

	for (n = 0; n < CFG_CANHW_MAX_BUSSES; n++)
	{
		conn[n].connected = Disconnected;
		veBusStatsInit(&veBusStats[n]);
	}

#if CFG_CANHW_KVASER
	if (drv = veKvaserRegister())
		veCanDrvRegister(drv);

	veKvCanInit();
#endif

#if  CFG_CANHW_SK_CAN
	veSkCanInit();
#endif

#if CFG_CANHW_LIN_CAN
	veLcInit();
#endif

	veCanDrvEnumerate();
}

/// Get the active interface.
VeBusId veGeCanGetActiveBus(void)
{
	return activeBus;
}

/// Set the active interface.
void veGeCanSetActiveBus(VeBusId busId)
{
	activeBus = busId;
}

/** Open connection as specified by the string.
 * @param busId
 *		the bus number to assign the CAN connection to.
 * @param driver
 *		the connection string of the CAN interface, e.g.
 *		- 0, "kvaser:1" will assign Kvaser connection 1 to bus 0.
 *		- 1, "disconnected" will silently allow all accesses to this bus.
 *
 * @returns whether successful.
 */
#define KVASER_STR		"kvaser:"
#define SOCKETCAN_STR	"socketcan:"
#define LINCAN_STR		"lincan:"

veBool veGeCanOpen(VeBusId busId, char const* driver)
{
	activeBus = busId;

	if (ve_stricmp(driver, "null") == 0)
	{
		conn[busId].connected = Connected;
		conn[busId].driver = VE_CAN_DRV_NULL;
		return veTrue;

#if CFG_CANHW_KVASER
	} else if (  strstr( driver, KVASER_STR) == driver)
	{
		VeCanGateway *gw = veCanGwGet(driver);

		conn[busId].driver = VE_CAN_DRV_KVASER;
		if (!gw)
			return veFalse;

		if (!veCanGwOpen(gw))
			return veFalse;

		/* Open channels, parameters and go on bus */
		conn[busId].channel.kv = veKvCanGetHandle();

		if (conn[busId].channel.kv < 0)
		{
			logE("CANHW", "Connection could not be made: '%s'", driver);
			return veFalse;
		}

		conn[busId].connected = Connected;
		return veTrue;
#endif

#if CFG_CANHW_SK_CAN
	} else if (  strstr( driver, SOCKETCAN_STR) == driver)
	{
		char const * channelStr = driver;
		int channel;

		conn[busId].driver = VE_CAN_DRV_SK_CAN;

		channelStr += strlen(SOCKETCAN_STR);
		channel = atoi( (char*) channelStr);

		/* Open channels, parameters and go on bus */
		conn[busId].channel.sk = veSkCanOpen(channel);

		if (conn[busId].channel.sk < 0)
		{
			logE("CANHW", "Connection could not be made: '%s' in socketcan:%d", driver, channel);
			return veFalse;
		}

		conn[busId].connected = Connected;
		return veTrue;
#endif

#if  CFG_CANHW_LIN_CAN
	} else if ( strstr( driver, LINCAN_STR) == driver) {

		nc8 const * channelStr = driver + strlen(LINCAN_STR);

		conn[busId].driver = drvLinCan;
		if (!veLcOpen(channelStr))
		{
			logE("CANHW", "Connection could not be made: '%s' in canlin:%s", driver, channelStr);
			return veFalse;
		}

		conn[busId].channel.lc = veLcGetHandle();
		conn[busId].connected = Connected;
		return veTrue;
#endif

	}

	// invalid configuration
	logE("CANHW", "Unknown interface: '%s'!", driver);
	return veFalse;
}

/// Implements @ref veCanSetBitRate.
veBool veGeCanSetBitRate(un16 kbit)
{
	if (conn[activeBus].connected == Disconnected)
	{
		logE("CANHW_GE", "veGeCanSetBusParams: connection not yet opened");
		return veFalse;
	}

	switch (conn[activeBus].driver)
	{
		case VE_CAN_DRV_NULL:
			return veTrue;

#if CFG_CANHW_KVASER
		case VE_CAN_DRV_KVASER:
			veKvCanSetHandle(conn[activeBus].channel.kv);
			return veKvCanSetBitRate(kbit);
#endif

#if  CFG_CANHW_SK_CAN
		case VE_CAN_DRV_SK_CAN:
			veSkCanSetHandle(conn[activeBus].channel.sk);
			return veSkCanSetBitRate(kbit);
#endif

#if CFG_CANHW_LIN_CAN
		case drvLinCan:
			veLcSetHandle(conn[activeBus].channel.lc);
			return veLcSetBusParams
				(
					freq,
					tseg1,
					tseg2,
					sjw,
					noSamp,
					syncmode
				);
#endif

		default:
			break;
	}

	// invalid configuration
	logE("CANHW_GE", "veGeCanSetBusParams: not implemented");
	return veFalse;
}

/// Implements @ref veCanBusOn
veBool veGeCanBusOn(void)
{
	if (conn[activeBus].connected == Disconnected)
	{
		logE("CANHW_GE", "veGeCanBusOn: connection not yet opened\n");
		return veFalse;
	}

	switch (conn[activeBus].driver)
	{
		case VE_CAN_DRV_NULL:
			return veTrue;

#if CFG_CANHW_KVASER
		case VE_CAN_DRV_KVASER:
			veKvCanSetHandle(conn[activeBus].channel.kv);
			return veKvCanBusOn();
#endif

#if CFG_CANHW_SK_CAN
		case VE_CAN_DRV_SK_CAN:
			veSkCanSetHandle(conn[activeBus].channel.sk);
			return veSkCanBusOn();
#endif

#if CFG_CANHW_LIN_CAN
		case VE_CAN_DRV_LIN_CAN:
			veLcSetHandle(conn[activeBus].channel.lc);
			return veLcBusOn();
#endif
		default:
			break;
	}

	// invalid configuration
	logE("CANHW_GE", "veGeCanBusOn: not implemented\n");
	return veFalse;
}

/// Close the CAN interface.
veBool veGeCanBusClose(void)
{
	if (conn[activeBus].connected == Disconnected)
		return veTrue;

	switch (conn[activeBus].driver)
	{
		case VE_CAN_DRV_NULL:
			return veTrue;

#if CFG_CANHW_KVASER
		case VE_CAN_DRV_KVASER:
			return canClose( conn[activeBus].channel.kv );
#endif

#if CFG_CANHW_SK_CAN
		case VE_CAN_DRV_SK_CAN:
			veSkCanSetHandle(conn[activeBus].channel.sk);
			return veSkCanBusOff();
#endif

#if CFG_CANHW_LIN_CAN
		case VE_CAN_DRV_LIN_CAN:
			veLcSetHandle(conn[activeBus].channel.lc);
			return veLcBusOff();
#endif
		default:
			break;
	}

	logE("CANHW_GE", "veGeCanBusClose: request not implemented for connection");
	return veFalse;
}

/// Implements @ref veCanSend
veBool veGeCanSend(VeRawCanMsg *rawCanMsg)
{
	veBool status = veFalse;

	if (conn[activeBus].connected == Disconnected)
	{
		logE("CANHW_GE", "veGeCanSend: connection not yet opened");
		return veFalse;
	}

	switch (conn[activeBus].driver)
	{
		case VE_CAN_DRV_NULL:
			return veTrue;

#if CFG_CANHW_KVASER
		case VE_CAN_DRV_KVASER:
			veKvCanSetHandle(conn[activeBus].channel.kv);
			status = veKvCanSend(rawCanMsg);
			break;
#endif

#if CFG_CANHW_SK_CAN
		case VE_CAN_DRV_SK_CAN:
			veSkCanSetHandle(conn[activeBus].channel.sk);
			status = veSkCanSend(rawCanMsg);
			break;
#endif

#if CFG_CANHW_LIN_CAN
		case VE_CAN_DRV_LIN_CAN:
			veLcSetHandle(conn[activeBus].channel.lc);
			status = veLcCanSend(rawCanMsg);
			break;
#endif
		default:
			logE("CANHW_GE", "veGeCanSend: sendmessage not implemented for connection");
			break;
	}

	if (status)
		veBusStatsTx(&veBusStats[activeBus]);

	return status;
}

/// Implements @ref veCanRead
veBool veGeCanRead(VeRawCanMsg * rawCanMsg)
{
	veBool status = veFalse;

	if (conn[activeBus].connected == Disconnected)
	{
		logE("CANHW_GE", "veGeCanRead: connection not yet opened");
		return veFalse;
	}

	switch (conn[activeBus].driver)
	{
		case VE_CAN_DRV_NULL:
			return veFalse;

#if CFG_CANHW_KVASER
		case VE_CAN_DRV_KVASER:
			veKvCanSetHandle(conn[activeBus].channel.kv);
			status = veKvCanRead(rawCanMsg);
			break;
#endif

#if CFG_CANHW_SK_CAN
		case VE_CAN_DRV_SK_CAN:
			veSkCanSetHandle(conn[activeBus].channel.sk);
			status = veSkCanRead(rawCanMsg);
			break;
#endif

#if CFG_CANHW_LIN_CAN
		case VE_CAN_DRV_LIN_CAN:
			veLcSetHandle(conn[activeBus].channel.lc);
			status = veLcCanRead(rawCanMsg);
			break;
#endif
		default:
			logE("CANHW_GE", "veGeCanRead: not implemented for connection");
			break;
	}

	if (status)
		veBusStatsRx(&veBusStats[activeBus], rawCanMsg->flags);

	return status;
}
