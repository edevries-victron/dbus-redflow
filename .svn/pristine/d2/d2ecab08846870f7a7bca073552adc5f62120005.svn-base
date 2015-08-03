#include <stdio.h>
#include <string.h>

#include <velib/velib_inc_J1939.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_nmt.h>
#include <velib/J1939/J1939_request.h>
#include <velib/J1939/J1939_function.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_timer.h>

/**
 * @addtogroup VELIB_J1939_NMT
 * @details
 *
 *	The network management maintains a list of available devices on the network.
 *	Within J1939 / NMEA-2000 the addresses of the nodes can change whenever a new
 *	node connects to the network. In general this won't happen a lot, since all
 *	nodes should store their address. It is important though to update these,
 *	since if not, requests are send to the wrong device.
 *
 *	The network is monitored in the following way:
 *	-	During startup a query is made for all network announcements (ACL's).
 *		When an ACL is received for an yet unknown address, the device is added or
 *		its address is updated.
 *	-	When an ACL is received for a known address, and its name is of higher
 *		priority then the current, the current device is set to inactive and
 *		the new one is added or its address is updated.
 *
 *	-	A device based timer is reset whenever activity of an active device is
 *		seen on the bus. Either j1939NmtActivity / j1939NmtActivityDev must be
 *		called for the device in question.
 *	-	If the timer is expired, the device is set to inactive and is actively
 *		queried for its presence.
 *	-	If the device doesn't respond in CFG_J1939_NMT_RETRY_ATTEMPTS times,
 *		it is marked as disconnected.
 *
 * During an ACL procedure, devices do not respond to requests addressed to
 * them. The acl timer keeps track of the 250ms time of the remote side.
 * j1939NmtIsOnBus can be used to verify if the remote device is performing
 * an address claim at the moment.
 */

/**
 * Time in 50ms ticks to set to inactive, do not set too low, since simple sensors
 * might only send frames once every 3 seconds.
 */
#define CFG_J1939_NMT_TIMEOUT				(10 * 20)	/* about 10 seconds */

/**
 * Number of times to query for an ACL of an inactive device before settings it to
 * disconnected. The time between the queries can also be set.
 */
#define CFG_J1939_NMT_RETRY_ATTEMPTS		3
#define CFG_J1939_NMT_RETRY_TIMEOUT			(1 * 20)	/* about 1 second */

/** the ACL takes 250 ms, so use 7 * = 350 ms to keep some margin */
#define ACL_TIMER_RESET						7

/** the NAD activity bitmap, 256 addresses / 8 bits = 32 entries */
#define NAD_BITMAP_SIZE						32

typedef enum
{
	J1939_NMT_OFFLINE,
	J1939_NMT_RUNNING
} J1939NmtState;

J1939FuncTp* j1939FuncList;

static J1939NmtState state = J1939_NMT_OFFLINE;
static un8 bitmap[NAD_BITMAP_SIZE];

/**
  * \todo Move this helper mask to a generic location (e.g. base.h) so it can be shared.
  */
static const un8 bitmapHelperMask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

static veBool bitmapGet(J1939Nad nad)
{
	return bitmap[nad / 8] & bitmapHelperMask[nad & 7];
}

static void bitmapSet(J1939Nad nad)
{
	bitmap[nad / 8] |= bitmapHelperMask[nad & 7];
}

static void bitmapClear(void)
{
	(void) memset(&bitmap[0], 0, NAD_BITMAP_SIZE);
}

void j1939NmtUpdate(void)
{
	/* During startup a query is made to all device to announce themselves */
	if (state == J1939_NMT_OFFLINE)
		if (j1939RequestPgn(J1939_NAD_BROADCAST, 0, J1939_PGN0_ACL))
			state = J1939_NMT_RUNNING;
}

void j1939NmtRebuild(void)
{
	/* Force the nmt module in the offline mode, this ensures that a request
	   is sent to update the database. Typically this function can be called
	   when the unit loses its address or when the device instance is changed.
	 */
	state = J1939_NMT_OFFLINE;
}

void j1939NmtTick(void)
{
	J1939FuncTp* dev;

	dev = j1939FuncList;
	while (dev != NULL)
	{
		/* Read-out bitmap and update device activity accordingly */
		if (bitmapGet(dev->nmt.nad))
		{
			j1939NmtActivityDev(dev);
		}

		/* Skip disconnected devices */
		if (dev->nmt.flags & J1939_NMT_DEV_DISCONNECTED)
		{
			dev = dev->next;
			continue;
		}

		if (dev->nmt.aclTimer)
			dev->nmt.aclTimer--;

		if (dev->nmt.timer && --dev->nmt.timer == 0)
		{
			/*
			 * No activity is seen during the inactive time, query the device
			 * to check if it is still there...
			 */
			dev->nmt.flags |= J1939_NMT_DEV_INACTIVE;

			/* Give up on querying the device after RETRY_ATTEMPTS */
			if ((dev->nmt.flags & CFG_J1939_NMT_RETRY_ATTEMPTS) >=
					CFG_J1939_NMT_RETRY_ATTEMPTS)
			{
				dev->nmt.flags |= J1939_NMT_DEV_DISCONNECTED;
				dev = dev->next;
				continue;
			}


			/* Query device for a response and make sure the stack accepts it */
			if (j1939RequestPgn(dev->nmt.nad, 0, J1939_PGN0_ACL))
			{
				/* clear retry request flag */
				dev->nmt.flags &= ~J1939_NMT_DEV_RETRY_REQUEST;
			} else {
				/* stack busy, retry asap */
				dev->nmt.flags |= J1939_NMT_DEV_RETRY_REQUEST;
			}

			/* increase retry counter */
			dev->nmt.flags++;

			/* set retry timer */
			dev->nmt.timer = CFG_J1939_NMT_RETRY_TIMEOUT;
		}
		else if (dev->nmt.flags & J1939_NMT_DEV_RETRY_REQUEST)
		{
			if (j1939RequestPgn(dev->nmt.nad, 0, J1939_PGN0_ACL))
			{
				/* clear retry request flag */
				dev->nmt.flags &= ~J1939_NMT_DEV_RETRY_REQUEST;

				/* set retry timer */
				dev->nmt.timer = CFG_J1939_NMT_RETRY_TIMEOUT;
			}
		}
		dev = dev->next;
	}

	/* Clear activity bitmap */
	bitmapClear();
}

/** Find a device function by its name (which is unique). */
struct J1939Func* j1939FuncByName(J1939Name* name)
{
	struct J1939Func* ret = j1939FuncList;

	while (ret != NULL)
	{
		if (j1939AclCompare(name, &ret->nmt.name) == 0)
			return ret;
		ret = ret->next;
	}

	return NULL;
}

/** Find a device function by its network address (this can change) */
struct J1939Func* j1939FuncByNad(J1939Nad nad)
{
	struct J1939Func* ret = j1939FuncList;

	while (ret != NULL)
	{
		if (ret->nmt.nad == nad)
			return ret;
		ret = ret->next;
	}
	return NULL;
}

/**
 * Incoming ACL announcements.
 * @ref J1939_PGN0_ACL should be passed here by @ref n2kMsgIn.
 */
void j1939NmtData(void)
{
	J1939FuncTp* dev;
	J1939Name name;

	/*	Try to find the device by address and read the NAME from the message */
	dev = j1939FuncByNad(n2kMsgIn->msgId.p.src);
	j1939AclReadName(&name);

	/* If the address is known, check if it is still valid. */
	if (dev != NULL)
	{
		/* Check if the same device is seen */
		if ( dev->nmt.name.low == name.low && // Same manufacturer and ucn
			dev->nmt.name.high.fields.indGroup == name.high.fields.indGroup &&
			dev->nmt.name.high.fields.devClass == name.high.fields.devClass &&
			dev->nmt.name.high.fields.function == name.high.fields.function)
		{
			/* When the same device is seen, check if its instance has changed */
			if (N2K_ACL_H_GET_DEVICE_INST(dev->nmt.name.high) != N2K_ACL_H_GET_DEVICE_INST(name.high))
			{
				/* Update and set appropriate flag so the application can take action */
				dev->nmt.flags |= J1939_NMT_DEV_INSTANCE_CHANGED;
				dev->nmt.name.high.vun32 = name.high.vun32;
			}

			/* If the device was disconnected or inactive assumes the device is performing the ACL procedure */
			if (dev->nmt.flags & (J1939_NMT_DEV_INACTIVE | J1939_NMT_DEV_DISCONNECTED))
				dev->nmt.aclTimer = ACL_TIMER_RESET;

			/* The device address and NAME are valid. Mark device as present. */
			dev->nmt.flags &= ~(J1939_NMT_DEV_RETRY_COUNTER | J1939_NMT_DEV_INACTIVE | J1939_NMT_DEV_DISCONNECTED);
			return;
		}

		/* ACL's with lower or equal priority are ignored */
		switch (j1939AclCompare(&dev->nmt.name, &name))
		{
			case -1:
				/*
				 * The new device trying to claim this address must choose another address..
				 * Ignore the device and wait for the next ACL message.
				 */
				return;

			case 0:
				/* This is already be handled by the specific check above */
				veAssert(veFalse);
				return;

			case 1:
				/*
				 * The ACL was received from a device with higher priority. Set the
				 * current device to inactive (it will send a new ACL if capable of
				 * address change)
				 */
				dev->nmt.nad = 0;
				dev->nmt.flags |= J1939_NMT_DEV_INACTIVE;
				dev->nmt.timer = CFG_J1939_NMT_TIMEOUT;
		}
	}

	/*
	 * Either a device changed addressed or just connected. Check if the
	 * device is changing address by looking up the device by its name.
	 */
	dev = j1939FuncByName(&name);

	/* If it is still not found, this is truly a new device. */
	if (dev == NULL)
	{
#if 0
		veInMove(0);
		printf("New device found (0x%x)\n", n2kMsgIn->msgId.p.src);
		printf("Unique#         : %d\n", veInBits32Le(21));
		printf("Manufacturer    : %d\n", veInBits32Le(11));
		printf("Instance low    : %d\n", veInBits32Le(3));
		printf("Instance high   : %d\n", veInBits32Le(5));
		printf("Function        : %d\n", veInUn8());
		printf("Reserved        : %d\n", veInBits32Le(1));
		printf("Class           : %d\n", veInBits32Le(7));
		printf("Instance        : %d\n", veInBits32Le(4));
		printf("Industry group  : %d\n", veInBits32Le(3));
		printf("Reserved        : %d\n", veInBits32Le(1));
#endif

		dev = j1939FuncAlloc(&name);

		/* Only continue if memory is available. */
		if (dev == NULL)
			return;

		/* Mark the device as a new device. */
		dev->nmt.flags = 0;
		dev->nmt.name = name;
		dev->nmt.timer = CFG_J1939_NMT_TIMEOUT;
		dev->nmt.nad = n2kMsgIn->msgId.p.src;
		dev->nmt.aclTimer = ACL_TIMER_RESET;

		/* Add to list. */
		dev->next = j1939FuncList;
		j1939FuncList = dev;

		/* Announce new device. */
		j1939FuncNew(dev);

	} else {
		/*
		 * The device changed address (but without losing it first), e.g.
		 * since the address was set by a command address. Reset all flags
		 * and assumes it is preforming an ACL procedure.
		 */
		dev->nmt.flags &= ~(J1939_NMT_DEV_RETRY_COUNTER | J1939_NMT_DEV_INACTIVE | J1939_NMT_DEV_DISCONNECTED);
		dev->nmt.timer = CFG_J1939_NMT_TIMEOUT;
		dev->nmt.nad = n2kMsgIn->msgId.p.src;
		dev->nmt.aclTimer = ACL_TIMER_RESET;
	}
}

void j1939NmtActivityDev(struct J1939Func* dev)
{
	if (dev == NULL)
		return;

	/* Device active -> reset timer when activity is seen */
	if (!(dev->nmt.flags & J1939_NMT_DEV_INACTIVE))
		dev->nmt.timer = CFG_J1939_NMT_TIMEOUT;

	/* Activity on a device that is disconnected */
	if ((dev->nmt.flags & J1939_NMT_DEV_DISCONNECTED))
	{
		/* Reset disconnected state / retry counter */
		dev->nmt.flags &= ~(J1939_NMT_DEV_DISCONNECTED | J1939_NMT_DEV_RETRY_COUNTER);

		/* Set to inactive so the acl record will be requested */
		dev->nmt.flags |= J1939_NMT_DEV_INACTIVE;
		dev->nmt.timer = CFG_J1939_NMT_TIMEOUT;
	}
}

/** All messsages can be passed here to update the presence of the device. */
void j1939NmtActivity(J1939Nad nad)
{
	/* Update device activity in the bitmap */
	bitmapSet(nad);
}

/**
 * Returns whether the device is not disconnected and not performing an ACL.
 * This is usefull for initialisation. If a new device is found and messages
 * are directly send to it, it might not respond yet, since it waits for 250ms
 * for objections against the claim.
 */
veBool j1939NmtIsOnBus(struct J1939Func* dev)
{
	if (dev->nmt.flags & (J1939_NMT_DEV_INACTIVE | J1939_NMT_DEV_DISCONNECTED))
		return veFalse;

	if (dev->nmt.aclTimer != 0)
		return veFalse;

	return veTrue;
}
