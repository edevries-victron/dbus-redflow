/**
 * @addtogroup VELIB_J1939_ACL
 *
 * The address claim procedure allows dynamic assigment of addresses to
 * devices on the bus. Details are described in ISO 11783-5 and NMEA
 * Appendix D. Basically the one with the lowest priority (by the NAME, the
 * data of the ACL) has to give up the address. To indicate that an address
 * is in use by a higher priority device it will send it's ACL when it
 * receives a lower priority NAME for the same address. No message should be
 * send before a valid NAD is obtained; ACL send and no device "objected"
 * within 250ms. The obtained NAD should be stored, see @ref J1939_ACL_STORE_NAD.
 * Note that the ACL procedure relies on the fact that no ACL message is lost
 * and that a priority 6 message can always be send within 250ms! Also note
 * adding devices to a network can cause (possible long) interruptions in the
 * CAN-bus traffic.
 *
 * 1) During the ACL procedure the same CAN identifiers can be send on the bus,
 * which should not be done with CAN-busses, since it will lead to errors,
 * till devices are pushed of the bus. The resolution is to (attempt to) stop
 * retransmission and then reschedule the transmission after a pseudo random
 * delay. It must therefore be known if sending failed. To allow other device
 * to object during 250ms, the delay is started AFTER the message is send,
 * which must thus be known as well.
 *
 * 2) Addresses are claimed for every "functional" device. A single physical
 * device can thus perform several address claims and needs to keep it's own
 * addresses consistent as well. Allthough this can be handle specially,
 * a easier solution is implemented:
 *
 * Above two points are handled by simply making a local echo of the message
 * AFTER it has been send. The @ref VELIB_CANHW does this when setting
 * @ref VE_CAN_DO_LOOP and @ref VE_CAN_SHOOT. The first to request a loopback,
 * the second flag to indicate that the hardware should not automatically
 * retransmit. The ACL bounces with an error flag when it could not be
 * send or the tx message is simply passed as a input message again. Both
 * messages are decorated with @ref VE_CAN_LOOP flags though, to recognize
 * them as locally originating. Since incoming messages will be processed by
 * all local devices, no special threatment is needed for more ACL procedures
 * in the same device. The method is prefered since e.g. a broadcasted request
 * for ACLs should also be answered by all devices (including the device the
 * request originated from). Simply decoration such a message with the loopback
 * flag fullfils such a requirement, without the need for any knowlegde about
 * the local device(s) / special treatment.
 *
 * A functional difference with the example state diagram a shown in appendix D
 * is the fact that a WON event in state 3 (i.o.w. another device with lower
 * priority started up while this device was waiting for 250ms), only
 * transmits the current device its ACL but does not reset the 250ms period.
 * There is no reason to do so, since the claim was WON in the first place.
 *
 * While the obvious case, obtaining bus access for the first time, is the most
 * common situation for the ACL procedure to be active and validated with the
 * standard test tools, there are more situations to consider:
 *
 * 1) Losing an address while it was claimed.
 * Handled by @ref j1939AclData but the caller should check the @ref
 * J1939_ACL_LOST flag. When set, all queued message related to this NAD should
 * be freed since this address is no longer valid. This includes hardware
 * queues (if possible). The flag must be reset externally when handled.
 *
 * 2) Collision / error during a reply when the address was already claimed.
 * Transmission errors during the answer of a request for ACL while the address
 * is "claimed" (which might have been caused by a simultanious ACL), must be
 * scheduled later by a delay to prevent pushing device of the bus if it was
 * caused by another ACL for the same address. Question is whether bus access
 * is lost in such a case. However if so, the CAN-bus would block for 250-403ms
 * and current transmission cancelled which could also simple be caused by
 * noise. Once realised that the other side would have seen the same error if
 * there was actually a device attempting to get bus access (and thus must
 * retry after a delay), it is unnessary to give up the address and bus access
 * on such an event. Regarding the state diagram of appendix D, this means that
 * IWON / ILOSE events are also processed in state 1, but only IF the current
 * address is claimed.
 *
 * 3) Inter-connection of two operational networks.
 * Especially with self powered devices it is possible to connect two identical
 * devices which have already performed their address claim. There are two
 * methods available to cope with this, j1939AclDetectConflict will checks if
 * no other device is using the claimed address. j1939AclTick sends ACL at a
 * low rate, which will solve the address conflict.
 *
 * 4) buffer under-run situations.
 * Buffer under-runs should normally be avoided but might occur during long
 * stalling operation (reprogramming e.g.). After such an event there is no
 * quarantee that no ACL was dropped in the meantime.
 *
 * Above situations have in common that the status of the claim is uncertain,
 * but not lost nor won and should not / be prevented from happening.
 * The event @ref J1939_ACL_EV_ASSURE can be thrown in such a situation to restart
 * the ACL procedure to claim the same address again. The current address is
 * (temporarily) given up.
 */

#include <velib/velib_config.h>
#include <velib/velib_inc_J1939.h>

#include <velib/canhw/canhw.h>
#include <velib/J1939/J1939.h>
#include <velib/J1939/J1939_device.h>
#include <velib/J1939/J1939_tx.h>
#include <velib/J1939/J1939_acl.h>
#include <velib/io/ve_stream_n2k.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_todo.h>
#include <velib/utils/ve_rand.h>
#include <velib/utils/ve_timer.h>

/**
 * ISO 117783-5:2001 p15: The ECU should be able to schedule the
 * next claim message within +/- 0.6 ms of the calculated delay.
 */

/* 1 ms tolerance. */
#define CFG_J1939_ACL_250MS_TIMEOUT		(250 + 1)

#if !CFG_J1939_VELIB_TIMER_DISABLED
/**
 * Set the timer to countdown ms.
 *
 * The timer should be accurate to the ms. When ms is @ref J1939_ACL_TMR_OFF
 * the timer must be disabled. By default the timer will rely on a background
 * ms counter, by veCircularTimeDiff.
 *
 * @ref CFG_J1939_VELIB_TIMER_DISABLED can be defined to allow a custom
 * implementation of the timer. @ref J1939_ACL_EV_TMR should be thrown
 * into @ref j1939AclUpdateState on expiration.
 */
void j1939AclSetTmr(un8 ms)
{
	j1939Device.acl.timer = ms;
	j1939Device.acl.timerStart = pltGetCount1ms();
}
#endif

/**
 * Sets up the address claim procedure for the active device.
 * The address claim is not started however.
 *
 * @note This must be done for every device.
 */
void j1939AclInit(void)
{
	j1939Device.flags = 0;
	j1939Device.acl.state = J1939_ACL_IDLE;
	j1939AclSetTmr(J1939_ACL_TMR_OFF);
}

/**
 * Set the timer to a new random delay.
 *
 * 0 to 153 milliseconds in steps of 0.6ms send within +/- 0.6.
 */
static void j1939AclSetTmrRnd(void)
{
	j1939AclSetTmr((un8) (((un16) veRand() * 6) / 10));
}

/**
 * Start to bring the current device on bus.
 */
void j1939AclEnable(void)
{
	if (j1939Device.acl.state != J1939_ACL_IDLE)
		return;

	veAssert(j1939Device.name.high.fields.reserved == 0);
	veAssert(j1939Device.name.high.fields.sca == 1);

	// Make sure the start address is valid
	// if not, change an set store flag
	if (j1939Device.nad > J1939_NAD_MAX)
	{
		j1939Device.nad = J1939_NAD_MAX;
		j1939Device.flags |= J1939_ACL_STORE_NAD;
	}

	j1939Device.acl.nadFirst = j1939Device.nad;

	// Initialization state used to start the address
	// claim process. Wait random delay, see F 6.4.6.1.1
	j1939Device.acl.state = J1939_ACL_RANDOM_DELAY;
	j1939AclSetTmrRnd();
}

#ifndef CFG_J1939_ACL_NAD_CHANGE_DISABLE
/**
 * Change the active network address. The device will directly go offline
 * and start the ACL procedure with the new address.
 */
void j1939AclChangeNad(J1939Nad cmndAddress)
{
	if (cmndAddress <= J1939_NAD_MAX)
	{
		j1939Device.acl.state = J1939_ACL_MON_TRANSMIT;
		j1939Device.flags = J1939_ACL_SEND;
		if (j1939Device.nad != cmndAddress)
			j1939Device.flags |= J1939_ACL_STORE_NAD;
		j1939Device.nad = cmndAddress;
		j1939Device.acl.nadFirst = cmndAddress;
		j1939AclSetTmr(J1939_ACL_TMR_OFF);
	}
}
#endif

/**
 * Function were the received ACLs must be passed to.
 *
 * If the address is lost, (j1939Device.flags & @ref J1939_ACL_LOST) all queues
 * must be directly emptied. Above condition must therefore always be tested
 * after calling this function.
 *
 * * @ref	J1939_PGN0_ACL should be passed here by @ref n2kMsgIn.
 */
void j1939AclData(void)
{
	J1939Name name;
	J1939AclEvent aclEvent;

	if (j1939Device.flags & J1939_ACL_DISABLED)
		return;

	// Ignore if the source address is not equal to the NAD of the current device.
	if (n2kMsgIn->msgId.p.src != j1939Device.nad)
		return;

	// Read NAME and check if valid.
	j1939AclReadName(&name);
	if (veStreamIn->error)
		return;

	switch (j1939AclCompare(&j1939Device.name, &name))
	{
	case 1:
		aclEvent = J1939_ACL_EV_WON;
		break;

	case 0:	// equal
#ifndef CFG_CANHW_NO_IS_LOOP_FLAG
		if (n2kMsgIn->flags & VE_CAN_IS_LOOP) {
			aclEvent = J1939_ACL_EV_TX_OK;
		} else {
			// This should normally not happen; an ACL is received with exactly the
			// same NAME. However in practice it can of course. In order not to
			// cause a race for the same address it is regarded as a lose of address.
			aclEvent = J1939_ACL_EV_LOST;
		}
#else
		// If the CAN driver doesn't mark local message, skip the sanity check.
		aclEvent = J1939_ACL_EV_TX_OK;
#endif
		break;

	case -1:
	default: // should not happen, just to please the compiler.
		aclEvent = J1939_ACL_EV_LOST;
		break;
	}

	// Handle error notifications.. Since there can be multiple devices on the same
	// physical device, only forward if the NAME is equal to this device and the
	// message originated from this device, i.o.w. aclEvent == J1939_ACL_EV_TX_OK.
	if (n2kMsgIn->flags & VE_CAN_TX_ERR)
	{
		if (aclEvent != J1939_ACL_EV_TX_OK)
			return;
		aclEvent = J1939_ACL_EV_TX_ERR;
	}

	j1939AclUpdateState(aclEvent);
}

/**
 * @note since this can change the ACL state machine it should be called
 * from the main context or hold appropriate locks.
 */
void j1939AclDetectConflict(J1939Id const *id)
{
	VE_UNUSED(id);

	/*
	 * If two running networks are connected, there might be a situation
	 * where to devices use the same address. If such a situation is detected
	 * assure that address conflicts are resolved (which is only possible if
	 * it is known which device sent it)
	 *
	 * There is a conflict if a message is seen on the bus with the same source
	 * address as the current device address and the current device address is
	 * valid and not equal to the special address 0xFE and it is not a loopback
	 * message. Receiving a ACL message on our address is not a conflict but
	 * part of the regular address conflict resolution.
	 */
#ifndef CFG_CANHW_NO_IS_LOOP_FLAG

	if (
			id->p.src == j1939Device.nad &&
			(j1939Device.flags & J1939_ACL_VALID_NAD) &&
			(n2kMsgIn->flags & VE_CAN_IS_LOOP) == 0 &&
			j1939Device.nad != J1939_NAD_NONE &&
			!(id->p.pg.p.dp == 0 && id->p.pf == J1939_PGN0_PF_ACL)
		)
	{
		j1939AclUpdateState(J1939_ACL_EV_ASSURE);
	}
#endif
}

/**
 * Update function which tries to send the ACL.
 */
void j1939AclUpdate(void)
{
	if (j1939Device.flags & J1939_ACL_DISABLED)
		return;

#if !CFG_J1939_VELIB_TIMER_DISABLED
	// keep spinning to get as close as possible to scheduled time.
	if (j1939Device.acl.timer != J1939_ACL_TMR_OFF)
	{
		veTodo();
		if (veCircularTimeDiff(j1939Device.acl.timerStart) > j1939Device.acl.timer)
		{
			j1939Device.acl.timer = J1939_ACL_TMR_OFF;
			j1939AclUpdateState(J1939_ACL_EV_TMR);
		}
	}
#endif

	// try to send a message
	if ((j1939Device.flags & J1939_ACL_SEND)) {
		if (j1939AclSend()) {
			j1939Device.flags &= ~(J1939_ACL_SEND | J1939_ACL_SEND_WHEN_CLAIMED);
			j1939Device.acl.timerBroadcast = 20 * 60;
		}
	}
}

void j1939AclTick(void)
{
	/*
	 * Low rate / periodic ACL broadcast: When the unit has a valid address
	 * it will periodically broadcast its ACL record. This always solves address
	 * conflicts (eventually) when CAN segments are physically joined after
	 * they have already been initialised. Actively checking for address
	 * conflicts with j1939MsgDetectAddressConflict is faster, but depends on
	 * loopback flags being supported.
	 */

	if (j1939Device.flags & J1939_ACL_VALID_NAD)
		if (j1939Device.acl.timerBroadcast && --j1939Device.acl.timerBroadcast == 0)
			j1939Device.flags |= J1939_ACL_SEND;
}

/**
 * State machine for the ACL procedure.
 *
 * Only the following event are expected externally:
 * - @ref J1939_ACL_EV_REQ
 *		Sends the ACL when in a appropiate state and monitor progress.
 *
 * - @ref J1939_ACL_EV_TMR
 * 		If the ACL timer is implemented externally. @ref j1939AclSetTmr should
 *		be overwritten and this event thrown on expiration of the timer.
 *
 */
void j1939AclUpdateState(J1939AclEvent aclEvent)
{
	// Immediately return when the ACL procedure is not enabled.
	if (j1939Device.acl.state == J1939_ACL_IDLE)
		return;

	// Only react in active states.
	// (or if an address is still claimed but delaying reply, see intro)
	if (
			j1939Device.acl.state > J1939_ACL_MON_TRANSMIT ||
			aclEvent == J1939_ACL_EV_TX_ERR ||
			((j1939Device.acl.state == J1939_ACL_RANDOM_DELAY) &&
			(j1939Device.flags & J1939_ACL_VALID_NAD))
		)
	{
		switch (aclEvent)
		{
		//case J1939_ACL_EV_ASSURE equal by define.
		case J1939_ACL_EV_TX_ERR:
			if (j1939Device.flags & J1939_ACL_VALID_NAD)
				j1939Device.flags = J1939_ACL_LOST;

			j1939Device.acl.state = J1939_ACL_RANDOM_DELAY;
			j1939AclSetTmrRnd();
			break;

		case J1939_ACL_EV_LOST:
			if (j1939Device.flags & J1939_ACL_VALID_NAD)
				j1939Device.flags = J1939_ACL_LOST; // also !J1939_ACL_VALID_NAD

			// Lost desired address, determine next address to claim.
			j1939Device.nad++;
			if (j1939Device.nad > J1939_NAD_MAX)
				j1939Device.nad = 0;

			// If the next address to try is the first address tried then
			// claiming an address failed.
			if (j1939Device.nad == j1939Device.acl.nadFirst)
			{
				j1939Device.nad = J1939_NAD_NONE;
				j1939AclSetTmrRnd();
				j1939Device.acl.state = J1939_ACL_CANNOT_CLAIM;
			} else {
				// trying the next address
				j1939Device.flags |= J1939_ACL_SEND;
				j1939Device.acl.state = J1939_ACL_MON_TRANSMIT;
				j1939AclSetTmr(J1939_ACL_TMR_OFF);
			}
			break;

		case J1939_ACL_EV_WON:
			// ACL for the same NAD arrived with lower priority -> resend ACL.
			j1939Device.flags |= J1939_ACL_SEND;
			veTodo();
			break;

		default:
			break;
		}
	}

	switch (j1939Device.acl.state)
	{
	case J1939_ACL_IDLE:
		break;

	case J1939_ACL_CANNOT_CLAIM:
		// No valid NAD, just answer requests (delayed).
		// Only power cycling leaves this state.
		switch (aclEvent)
		{
		case J1939_ACL_EV_TX_ERR:
		case J1939_ACL_EV_REQ:
			j1939AclSetTmrRnd();
			break;

		case J1939_ACL_EV_TMR:
			j1939Device.flags |= J1939_ACL_SEND;
			break;

		default:
			break;
		}
		break;

	case J1939_ACL_RANDOM_DELAY:
		// Waiting for random delay to complete.
		if (aclEvent == J1939_ACL_EV_TMR)
		{
			j1939Device.flags |= J1939_ACL_SEND;
			j1939Device.acl.state = J1939_ACL_MON_TRANSMIT;
			veTodo();
		}
		break;

	case J1939_ACL_MON_TRANSMIT:
		if (aclEvent == J1939_ACL_EV_TX_OK)
		{
			// ACL has been send.
			j1939AclSetTmr(CFG_J1939_ACL_250MS_TIMEOUT);
			j1939Device.acl.state = J1939_ACL_WAITING_FOR_RESPONSE;
		}
		break;

	case J1939_ACL_WAITING_FOR_RESPONSE:

		if (aclEvent == J1939_ACL_EV_TMR)
		{
			// 250 milliseconds passed; the address is claimed successfully.
			// Save NAD if another address is used then the one started with.
			if (j1939Device.acl.nadFirst != j1939Device.nad)
				j1939Device.flags |= J1939_ACL_STORE_NAD;

			j1939Device.flags |= J1939_ACL_VALID_NAD;
			j1939Device.acl.state = J1939_ACL_SUCCESSFUL;

			// Since there is a valid address now, answer older ACL requests
			// occuring while the address claim was pending. Note: the J1939_ACL_SEND
			// will also clear the also J1939_ACL_SEND_WHEN_CLAIMED, @see j1939AclUpdate.
			if (j1939Device.flags & J1939_ACL_SEND_WHEN_CLAIMED)
			{
				j1939Device.flags |= J1939_ACL_SEND;
				veTodo();
			}

			// Reset the starting point for addresses. In case another node with
			// higher priority gets on bus, the whole range should be searched again.
			j1939Device.acl.nadFirst = j1939Device.nad;
			break;
		}
		break;

	case J1939_ACL_SUCCESSFUL:
		// On bus, answer requests.
		if (aclEvent == J1939_ACL_EV_REQ)
		{
			j1939Device.flags |= J1939_ACL_SEND;
			veTodo();
		}
		break;
	}

	// Not allowed to respond yet, so mark as delayed request,
	// and answer when an address is claimed.
	if (aclEvent == J1939_ACL_EV_REQ && (j1939Device.flags & J1939_ACL_SEND) == 0)
		j1939Device.flags |= J1939_ACL_SEND_WHEN_CLAIMED;
}

/**
 * Extract the name from a message.
 */
void j1939AclReadName(J1939Name* name)
{
	name->low = veInUn32Le();
	name->high.vun32 = veInUn32Le();
}

/**
 * This routine is responsible for packing and transmitting an
 * address claim message with the desired source address.
 */
veBool j1939AclSend(void)
{
	J1939Msg* msg;

	msg = (J1939Msg*) j1939SfTxAllocNoNad();
	if (msg == NULL)
		return veFalse;

	msg->msgId.w = J1939ID_PRI(6) | J1939ID_PGN(J1939_PGN0_ACL | J1939_NAD_BROADCAST);
	msg->flags |= VE_CAN_SHOOT | VE_CAN_DO_LOOP;

	// add the NAME.
	veOutMsg(msg);
	veOutUn32Le(j1939Device.name.low);
	veOutUn32Le(j1939Device.name.high.vun32);
	(void) veOutFinalize();

	j1939TxPush(msg);
	return veTrue;
}

/**
 * Compares the priority of two NAME.
 *
 * @param high 	The NAME which is assumed to be higher priority.
 * @param low	The NAME which is assumed to be lower priority.
 *
 * @retval 1 	When the name which is assumed to be higher priority
 * 				actually is.
 * @retval -1	When the opposite is the case.
 * @retval 0	When the NAMEs are equal.
 *
 * @note An higher priority means a lower numerical value.
 */
sn8 j1939AclCompare(J1939Name const * high, J1939Name const* low)
{
	// if the high field is smaller (higher in priority) condition is met
	if (high->high.vun32 < low->high.vun32)
		return 1;

	// return -1 when the opposite is the case
	if (high->high.vun32 > low->high.vun32)
		return -1;

	// high fields are equal, check the low
	if (high->low < low->low)
		return 1;

	if (high->low > low->low)
		return -1;

	return 0;
}

/**
 * Verify if the message is targetted to the active device.
 */
veBool j1939MsgForDevice(J1939Id const* id)
{
	// A message is direct to this device if it is a broadcast message or a targed specific
	// message send to the broadcast address; or the message is directed to the current device
	// address and the current device it's address is valid or it is the special address 0xFE.
	if (
			id->p.pf >= 0xF0  ||
			id->p.ps == 0xFF  ||
			(
				id->p.ps == j1939Device.nad &&
				(
					(j1939Device.flags & J1939_ACL_VALID_NAD) ||
					j1939Device.nad == J1939_NAD_NONE
				)
			)
		)
	{
		return veTrue;
	}

	return veFalse;
}
