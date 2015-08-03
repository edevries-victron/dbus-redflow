/**
 * An VE.Bus system can consists of up to 32 devices. These can be
 * configured to be in parallel or multi phases or combinations of that.
 * Typically user are not interesed in what a single device is doing,
 * but only in the system as whole. This information is collected
 * here, e.g. the total current per phase etc, total charge current etc.
 * Only the system as a whole can be turned off / on / set to charger
 * only etc which is handled below.
 *
 * The Interface documentation does refer to these frame as Special frames.
 */

#include <string.h> // memset

#include <velib/velib_inc_mk2.h>

#include <velib/utils/ve_assert.h>
#include <velib/base/endian.h>
#include <velib/mk2/hal.h>
#include <velib/mk2/mk2.h>
#include <velib/mk2/hal.h>
#include <velib/mk2/vebus_system.h>
#include <velib/utils/ve_logger.h>
#include <velib/utils/ve_rand.h>

#define PANEL_TIMEOUT					(4 * 20)

// Arguments which can be based to the MK2 F command to request a special frame.
typedef enum
{
	INFO_DC,
	INFO_AC0,
	INFO_AC1,
	INFO_AC2,
	INFO_AC3,
	AC_IN_CONFIG
} VebusSpecialFrame;

// Known special frames.
#define VEBUS_RSP_INFO				0x20
#define VEBUS_RSP_AC_IN_CONFIG		0x41

// Opcode in info frame response.
typedef enum
{
	PHASE_INFO_AC3 = 0x05,
	PHASE_INFO_AC2,
	PHASE_INFO_AC1,
	PHASE_INFO_AC0_1,
	PHASE_INFO_AC0_2,
	PHASE_INFO_AC0_3,
	PHASE_INFO_AC0_4,
	PHASE_INFO_DC
} VebusInfoFrameRsp;

static void pollAssistant(void);
static void vebusCreatePowerSnapshot(Mk2Callback *callback);
static void vebusGetPowerSnapshot(VebusDevice *dev, Mk2Callback *callback);

/**
 * Request a VE.Bus info frame.
 *
 * Note: In the current MK2 firmware, this command works by setting the MK2 SFR
 * ID and relevant state flags to allow the response to be forwarded regardless
 * of the current VE.Bus address. If the MK2 state is changed before the reply
 * is received, those values must not be changed or reply will be filtered out!
 *
 * @param type - The type of frame to request.
 */
static Mk2MsgTx* reqInfoFrameMsg(un8 type)
{
	Mk2MsgTx* msg = mk2Msg('F');
	veAssert(msg);
	if (!msg)
		return NULL;
	msg->buf[2] = type;
	msg->length = 3;
	return msg;
}

/** handle AC info frames */
static Mk2EventReply acFrameHandler(Mk2Event ev, Mk2MsgRx* msg)
{
	un8 phaseInfo;
	un8 phases;
	Mk2RawAcPhase* phase;

	if (ev != MK2_FRAME || msg->length < 14 || msg->buf[0] != 0x20)
		return MK2_CONTINUE;

	/* mk2TxMsg->rsp = requested phase (zero offset) */
	phase = (Mk2RawAcPhase*) mk2.fh.msgOut->ctx;
	phaseInfo = msg->buf[5];
	if (mk2.fh.msgOut->rsp == 0) {
		/*
		 * This is the first phase, so the phaseInfo field also contains the number
		 * of phases. 8 -> 1 phase, 9 -> 2 phases etc.
		 */
		phases = phaseInfo - 7;
		if (phases > VEBUS_MAX_PHASES)
			return MK2_CONTINUE;
		phase->phaseCount = phases;
	} else {
		if ((un16) (8 - phaseInfo) != mk2.fh.msgOut->rsp)
			return MK2_CONTINUE;
	}

	logI("vebus_system", "got phase %d", mk2.fh.msgOut->rsp);
	phase->backfeedFactor = msg->buf[1];
	phase->inverterFactor = msg->buf[2];
	phase->inputV = le_u16_to_cpu_p(&msg->buf[6]);
	phase->inputI = le_u16_to_cpu_p(&msg->buf[8]);
	phase->inverterV = le_u16_to_cpu_p(&msg->buf[10]);
	phase->inverterI = le_s16_to_cpu_p(&msg->buf[12]);
	phase->inputPeriod = msg->buf[14];

	return MK2_DONE;
}

/** request AC frame */
void vebusGetAcPhase(un8 phase, Mk2RawAcPhase* out, Mk2Callback* callback)
{
	Mk2MsgTx* msg = reqInfoFrameMsg(INFO_AC0 + phase);
	veAssert(msg);
	if (!msg)
		return;
	logI("vebus_system", "---- get phase %d ----", phase);
	msg->callback = callback;
	msg->handler = acFrameHandler;
	msg->rsp = phase;
	msg->ctx = out;
	mk2FhQueueFrame(msg);
}

/** handle DC frame */
static Mk2EventReply dcFrameHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	Mk2RawDc* dc;

	if (ev != MK2_FRAME || msg->length < 14 || msg->buf[0] != 0x20 ||
			msg->buf[5] != PHASE_INFO_DC)
		return MK2_CONTINUE;

	logI("vebus_system", "++++ got dc ++++");

	// This is a DC info frame
	dc = (Mk2RawDc*) mk2.fh.msgOut->ctx;
	dc->V = le_u16_to_cpu_p(&msg->buf[6]);
	dc->inverterI = le_u24_to_cpu_p(&msg->buf[8]);
	dc->chargerI = le_u24_to_cpu_p(&msg->buf[11]);
	dc->period = msg->buf[14];
	return MK2_DONE;
}

/* request DC frame */
void vebusGetDcValues(Mk2RawDc* dc, Mk2Callback* callback)
{
	Mk2MsgTx* msg = reqInfoFrameMsg(INFO_DC);
	veAssert(msg);
	if (!msg)
		return;

	logI("vebus_system", "---- get dc ----");
	msg->ctx = dc;
	msg->callback = callback;
	msg->handler = dcFrameHandler;
	mk2FhQueueFrame(msg);
}

/** Parse AC in related settings */
static Mk2EventReply acInConfigHandler(Mk2Event mk2event, Mk2MsgRx *msg)
{
	VebusAcInConfig* info;
	un16 limit;

	if (mk2event != MK2_FRAME || msg->length < 12 || msg->buf[0] != VEBUS_RSP_AC_IN_CONFIG)
		return MK2_CONTINUE;

	info = (VebusAcInConfig*) mk2.fh.msgOut->ctx;
	info->acInputConfig = msg->buf[5];
	info->min = le_u16_to_cpu_p(&msg->buf[6]);
	info->max = le_u16_to_cpu_p(&msg->buf[8]);
	limit = le_u16_to_cpu_p(&msg->buf[10]);

	if (limit != info->limit)
		vebusCurrentLimitChanged(info->acInputConfig, limit);
	info->limit = limit;

#if CFG_WITH_CONTROL
	/*
	 * note: the returned value is never smaller then the minimum, but 0 is valid
	 * for writing. In order to report this properly, the valus is forced to zero
	 * when writing zero and the minimum is returned.
	 */
	if (vebusSystem.ctrl.state == VEBUS_PANEL_CONTROL && veGetCurrentLimitCb() == 0
			&& info->limit == info->min)
	{
		info->limit = 0;
	}
#endif

	logI("vebus_system", "++++ got ac in config ++++");
	logI("ac-in-config", "active input: %d", info->acInputConfig);
	logI("ac-in-config", "current limit %d", info->limit);
	logI("ac-in-config", "current range %d - %d", info->min, info->max);

	return MK2_DONE;
}

/** request DC frame */
void vebusGetAcInConfig(VebusAcInConfig* info, Mk2Callback* callback)
{
	Mk2MsgTx* msg = reqInfoFrameMsg(AC_IN_CONFIG);
	veAssert(msg);
	if (!msg)
		return;
	logI("vebus_system", "---- get ac in config ----");
	msg->ctx = info;
	msg->callback = callback;
	msg->handler = acInConfigHandler;
	mk2FhQueueFrame(msg);
}

/* == CONTROL == */
#if CFG_WITH_CONTROL

void vebusSetModeVariable(VebusSwitchPos mode)
{
	if (mk2.state.mode == mode)
		return;

	mk2.state.mode = mode;
	vebusModeChanged(mode);
}

/**
 * In order to be able to command a multi the following is needed.
 * - The panel detect line must be grounded.
 * - call vebusControlInit / vebusControlTick/ vebusControlUpdate
 * - Incoming frames should go to vebusControlHandler typically in
 *   mk2FhFrameReceivedEvent add vebusControlHandler(MK2_FRAME, mk2msg);
 *
 * On a state change, try to claim the vebus system, with
 *	vebusControlDevice and implement the function:
 *	- veGetCurrentLimitCb
 */

/*
 * Must be called after init vebusControlInit and BMS / DMC discovery.
 * Since the vebusSystem cannot be controlled in the presence of
 * one of these devices, don't even try.
 */
void vebusControlUpdate(void)
{
	/* If there is a VMC or BMS don't try to control the system */
	vebusSystem.ctrl.state = (vebusCanBeControlled() ? VEBUS_PANEL_INIT : VEBUS_PANEL_DISABLED);
}

void vebusControlTick(void)
{
	if (vebusSystem.ctrl.timer)
		vebusSystem.ctrl.timer--;

	if (vebusSystem.ctrl.vebusPowerTimer && --vebusSystem.ctrl.vebusPowerTimer == 0)
		mk2EnsureVebusPower(veFalse);

	/*
	 * take over attempt: If in the first x second after the mk2 panel frames
	 * where received, they are ignored to provide the other side some time to
	 * backoff. If a panel frame is received between x / 2*x seconds it is
	 * assumed that the panel won't backoff.
	 */
	if (vebusSystem.ctrl.state == VEBUS_PANEL_TAKE_OVER && vebusSystem.ctrl.timer < PANEL_TIMEOUT / 2)
	{
		if (vebusSystem.ctrl.panelFrameReceived)
		{
			logI("control", "could not take over control, syncing again");
			vebusSystem.ctrl.timer = PANEL_TIMEOUT;
			vebusSystem.ctrl.state = VEBUS_PANEL_SYNC;

			if (vebusSystem.ctrl.controlCb) {
				vebusSystem.ctrl.controlCb(veFalse);
				vebusSystem.ctrl.controlCb = NULL;
			}
		} else if (vebusSystem.ctrl.timer == 0) {
			logI("control", "ok sending panel frames...");
			vebusSetModeVariable(vebusSystem.ctrl.intendedMode);
			vebusSystem.ctrl.state = VEBUS_PANEL_CONTROL;
		}
	}
	vebusSystem.ctrl.panelFrameReceived = veFalse;

	/* periodically sent a panel frame when the system is controlled from this device */
	if ((vebusSystem.ctrl.state == VEBUS_PANEL_CONTROL || vebusSystem.ctrl.state == VEBUS_PANEL_TAKE_OVER) &&
			(vebusSystem.ctrl.panelTimer == 0 || --vebusSystem.ctrl.panelTimer == 0)) {
		vebusQueuePanelFrameCallback();
		vebusSystem.ctrl.panelTimer = 20;
		logI("vebus", "mark panel frame");
	}

	/* Resume panel state, if no device is sending panel frames. */
	if (vebusSystem.ctrl.timer == 0 &&
			(vebusSystem.ctrl.state == VEBUS_PANEL_SYNC || vebusSystem.ctrl.state == VEBUS_PANEL_INIT))
	{
		logI("control", "resuming panel frames");
		vebusSystem.ctrl.state = VEBUS_PANEL_CONTROL;
		vebusQueuePanelFrameCallback();
	}
}

/**
 * The vebus system can only be controlled by one device at a time.
 * In order to allow control from more then 1 mk2, an attempt is made
 * to push away the other mk2 by sending its panel frames back. This
 * might be succesfull work but will fail e.g. if a PMC is present.
 */
void vebusControlDevice(VebusControlCallback cb, VebusSwitchPos mode)
{
	if (vebusSystem.ctrl.state == VEBUS_PANEL_DISABLED) {
		if (cb)
			cb(veFalse);
		return;
	}

	/* make sure vebus is powered for some time */
	vebusSystem.ctrl.vebusPowerTimer = 10 /* sec */ * 20;
	mk2EnsureVebusPower(veTrue);
	vebusSystem.ctrl.controlCb = cb;

	if (mode == SP_UNKNOWN) {
		if (vebusSystem.ctrl.state == VEBUS_PANEL_TAKE_OVER)
			mode = vebusSystem.ctrl.intendedMode;
		else
			mode = mk2.state.mode;
	}

	/* If this device is controlling the vebus system, just update */
	if (vebusSystem.ctrl.state == VEBUS_PANEL_CONTROL) {
		vebusSetModeVariable(mode);
		vebusQueuePanelFrameCallback();
		return;
	}

	/*
	 * Otherwise attempt to push the current device away.
	 * Start by sending panel frames with the current values,
	 * so the multi does not react to out attempt.
	 */
	logI("control", "attempting to take over");
	vebusSystem.ctrl.panelFrameReceived = veFalse;
	vebusSystem.ctrl.intendedMode = mode;
	vebusSystem.ctrl.state = VEBUS_PANEL_TAKE_OVER;
	vebusSystem.ctrl.timer = 255; /* wil be set when sending */
	vebusSystem.ctrl.panelTimer = 0;
}

/* look for other device's commanding the multi state. */
Mk2EventReply vebusControlHandler(Mk2Event mk2event, Mk2MsgRx *msg)
{
	un8 remotePos;

	if (mk2event != MK2_FRAME || msg->buf[0] != 0x40 || msg->length < 4)
		return MK2_CONTINUE;

	/*
	 * Update the multi "switch position", on / "charger only" etc received
	 * from another panel controlling the multi.
	 */
	remotePos = msg->buf[1] & 0x0F;

	/* adjust older format */
	if (remotePos & 0x08)
		remotePos = (remotePos & 0x3) + 1;

	logI("panel", "received panel frame %d", remotePos);

	vebusSystem.ctrl.panelFrameReceived = veTrue;

	if (vebusSystem.ctrl.state != VEBUS_PANEL_TAKE_OVER)
	{
		/*
		 * Stop controling the vebus power so the remote side can complete
		 * shutdown the system. The rand is to avoid two panels fighting forever
		 * over control and both backing of at the same time.
		 */
		vebusSystem.ctrl.timer = PANEL_TIMEOUT + veRand() / 10;
		mk2EnsureVebusPower(veFalse);

		if (vebusSystem.ctrl.state != VEBUS_PANEL_DISABLED && vebusSystem.ctrl.state != VEBUS_PANEL_SYNC) {
			logI("control", "panel frame received, backing off");
			vebusSystem.ctrl.state = VEBUS_PANEL_SYNC;
		}

		vebusSetModeVariable(remotePos);
	}

	return MK2_CONTINUE;
}

/** Instruct the multi to go in a certain state and set mains limit. */
void vebusSendPanelFrame(Mk2Callback* callback)
{
	switch (vebusSystem.ctrl.state)
	{
	case VEBUS_PANEL_DISABLED:
	case VEBUS_PANEL_INIT:
	case VEBUS_PANEL_SYNC:
		callback();
		break;

	case VEBUS_PANEL_TAKE_OVER:
		/* init the counter _after_ the first panel frame is being sent */
		if (vebusSystem.ctrl.timer > PANEL_TIMEOUT)
			vebusSystem.ctrl.timer = PANEL_TIMEOUT;
	case VEBUS_PANEL_CONTROL:
		if (vebusSystem.ctrl.state == VEBUS_PANEL_CONTROL)
		{
			/* Instruct the mk2 to send panel frames */
			mk2.state.limit.current.value = veGetCurrentLimitCb();

			/* Let the application know what happened. */
			if (vebusSystem.ctrl.controlCb) {
				vebusSystem.ctrl.controlCb(veTrue);
				vebusSystem.ctrl.controlCb = NULL;
			}
		}
		/**
		 * note: MK2_S_FLAG_AUTO_SEND_STATE is no longer used, since if a program
		 * is disconnected or a device powered off, without notification, the mk2
		 * keeps commanding the device.
		 */
		mk2.state.sFlags |= MK2_S_FLAG_NEW_FORMAT;
		mk2.state.sFlags &= ~(MK2_S_FLAG_DO_NOT_SEND_STATE | MK2_S_FLAG_AUTO_SEND_STATE);
		mk2SetState(&mk2.state, callback);
		break;

	default:
		veAssert(veFalse);
		return;
	}
}

#else

void vebusControlUpdate(void)
{
}

void vebusControlTick(void)
{
}

#endif

/** Figure out some additional alarmstates with are not reported by regular messages */
un16 vebusAlarmsFromState(VebusState* info)
{
	un16 ret = info->alarms;

	/* 0x1F / 0x1F means no flags at all. */
	if (info->leds == 0x1F && info->ledsBlink == 0x1F)
		return ret;

	/*
	 * Don't attempt to interpreted the leds on vebus error. Note, when in power assist
	 * mode, the inverter LED is reported as blinking as well, but this is not an error
	 * code, hence the check for leds as well.
	 */
	if (info->vebusError || VE_LED_ANTI(info, VE_LED_INVERTER))
		return ret | VE_ALARM_FLAG_VEBUS_ERROR;

	/*
	 * Absorption, bulk and float blink in phase -> forced state indication.
	 * This is used to enable the forced charge state from a panel.
	 */
	if ( VE_LED_BLINK( info, VE_LED_BULK | VE_LED_ABSORPTION | VE_LED_FLOAT ) )
		return ret;

	/* Absorption and float blink in phase -> temperature sensor error */
	if ( VE_LED_BLINK( info, VE_LED_ABSORPTION | VE_LED_FLOAT) )
		ret |= VE_ALARM_FLAG_TEMP_SENSOR;

	/* No vebus error and bulk and absorption blink in phase -> voltage sensor error */
	if ( VE_LED_BLINK( info, VE_LED_BULK | VE_LED_ABSORPTION) )
		ret |= VE_ALARM_FLAG_VOLTAGE_SENSOR;

	return ret;
}

/** a reminder that the acInConfig contains flags, only the last 2 bits are the active input */
un8 vebusActiveIn(VebusAcInConfig* config)
{
	return config->acInputConfig & 0x03;
}

veBool vebusCanBeControlled(void)
{
	if (vebusSystem.bmsVersion || vebusSystem.dmcVersion)
		return veFalse;

	return veTrue;
}

veBool vebusActiveCurrentLimitChangable(VebusAcInConfig* config)
{
	/* Current limit cannot be controled if either a DMC or BMS is present */
	if (!vebusCanBeControlled())
		return veFalse;

	/*
	 * Current limit can also not be controlled if the Ve.Bus system is configured
	 * to ignore the setpoints
	 */
	return (config->acInputConfig & 0x04 ? 1 : 0);
}


/**
 * Since power used to be only calculated based on the values the
 * master reported, it had not been noticed that since 2006 till
 * feb 2015 that the slave don't measure / report the input power.
 * Since this the total power is changed to sum the all, this was
 * revealed, since power values didn't make any sense it parallel
 * systems. Note system with several phases only more phases are
 * ok since phase masters keep measuring it. This workaround computes
 * the total power of a single phase by assuming the slaves do
 * roughly use the same amount of power.
 *
 * Once this issue is resolved some condition can go here to stop
 * using the master its value and use the actual power values (again).
 *
 * The Kwh counters are affected by the same issue.
 */
static veBool deviceReportsInvalidPower(VebusDevice *dev)
{
	veBool isPhaseMaster = dev->addr < 3;
	un16 version;

	/* phase master always have been reporting correct values */
	if (isPhaseMaster)
		return veFalse;

	version = dev->version.number % 1000;

	if (version >= 100 && version < 159)
		return veTrue;

	if (version >= 200 && version < 209)
		return veTrue;

	if (version >= 300 && version < 306)
		return veTrue;

	/* Assumed to be newer .. */
	return veFalse;
}

static veBool needPowerWorkaround(void)
{
	return vebusSystem.quirks & VEBUS_QUIRK_A_SLAVE_REPORTS_INVALID_POWER;
}

/* Update flags for needed workarounds */
void vebusUpdateTweaks(void)
{
	VebusDevice *dev;

	vebusSystem.quirks = 0;
	dev = vebusSystem.devices;
	while (dev) {
		if (deviceReportsInvalidPower(dev))
			vebusSystem.quirks |= VEBUS_QUIRK_A_SLAVE_REPORTS_INVALID_POWER;
		dev = dev->next;
	}
}

#if CFG_MK2_KWH_COUNTERS

/** handle kw counters */
static Mk2EventReply kwhFrameHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	un8 n;
	un8 *counters;

	if (ev != MK2_FRAME)
		return MK2_CONTINUE;

	if (mk2IsXYZ(msg, WM_RSP_UNKNOWN_COMMAND))
	{
		logI("mk2", "kWh counters not supported");
		vebusSystem.kwh.state = VE_KWH_NOT_SUPPORTED;
		return MK2_DONE;
	}

	if (!mk2IsXYZ(msg, WM_RSP_GET_KWH))
		return MK2_CONTINUE;

	logI("mk2", "got kWh counters");
	counters = (un8 *) mk2.fh.msgOut->ctx;
	logI("mk2", "counter %p", counters);
	for (n = 0; n < 10; n++) {
		un8 rolling = msg->buf[3 + n];
		if (vebusSystem.kwh.initialized) {
			un8 delta = rolling - counters[n];
			vebusSystem.kwh.totals[n] += delta;
		} else {
			logI("kwh", "[%d] init to %d", n, rolling);
		}
		counters[n] = rolling;
	}

	vebusSystem.kwh.state = VE_KWH_CHECK_NEXT_DEVICE;

	return MK2_DONE;
}

/*
 * Get Kw counters from the next device.
 *
 * return veTrue when queued, veFalse when done or not supported.
 */
veBool vebusKwhUpdate(Mk2Callback *callback)
{
	un32 mask;
	Mk2MsgTx *msg;
	veAssert(vebusSystem.kwh.state != VE_KWH_BUSY);

	if (vebusSystem.kwh.state == VE_KWH_NOT_SUPPORTED)
		return veFalse;

	/*
	 * Workaround: disable kwhcounters when slave reporting invalid data are present.
	 * It can be guessed, but better instruct the user to update the firmware.
	 */
	if (!vebusSystem.kwh.initialized) {
		vebusUpdateTweaks();
		if (needPowerWorkaround()) {
			vebusSystem.kwh.state = VE_KWH_NOT_SUPPORTED;
			vebusSystem.kwh.initialized = veTrue;
			return veFalse;
		}
	}

	/** start with master or next device */
	if (vebusSystem.kwh.state == VE_KWH_IDLE)
		vebusSystem.kwh.address = 0;
	else
		vebusSystem.kwh.address++;

	// find next device ..
	mask = 1 << vebusSystem.kwh.address;

	while (mask) {
		if (vebusSystem.state.shortIds & mask)
			break;
		mask <<= 1;
		vebusSystem.kwh.address++;
	}

	if (!mask) {
		vebusSystem.kwh.state = VE_KWH_IDLE;
		vebusSystem.kwh.initialized = veTrue;
		return veFalse;
	}

	msg = mk2MsgXYZ(vebusSystem.kwh.address, 0xF);
	if (msg == NULL)
		return veFalse;

	logI("mk2", "get kWh counters of addr=%d", vebusSystem.kwh.address);
	msg->handler = kwhFrameHandler;
	msg->ctx = &vebusSystem.kwh.counters[vebusSystem.kwh.address][0];
	msg->callback = callback;
	mk2FhQueueFrame(msg);

	// queue next message, ctx = device
	vebusSystem.kwh.state = VE_KWH_BUSY;

	return veTrue;
}

/*
 * Will return true if the are more device left for this round to query.
 * Calling vebusKwhUpdate will fetch this next value.
 */
veBool vebusKwhMoreTodo(void)
{
	return vebusSystem.kwh.state == VE_KWH_CHECK_NEXT_DEVICE;
}

#endif

#if CFG_MK2_AC_SENSOR

/***** assistant values **********/

/*
 * The AC sensor assistant is reports the (blind) current of a sensor connected
 * to a vebus device. There can be several of such assistant in multiple devices.
 * During the configuration of the assistant the user has to enter where the sensor
 * is connected to which is also available as a RAM id.
 *
 * The provided RAM vars are (from it is offset)
 *
 * 0: assistent ident (0014) id=1, 4 ramids
 * 1: location:
 *    4..0:
 *    6..5:
 *      - 00: L1
 *      - 01: L2
 *      - 10: L3
 * 2: assistant current (centiAmps)
 * 3: apparent power VA
 * 4: bits 31..16 of the VAs counter
 */
static void callbackAssistantKwhInit(void)
{
	VeCurrentSensor *sensor = &vebusSystem.iSense.sensors[vebusSystem.iSense.count];
	VE_UNUSED(sensor); // debug only

	logI("vas", "got initial Joules %d, sensor %d init done", sensor->kwhCounter, vebusSystem.iSense.count);
	vebusSystem.iSense.count++;

	// continue to find more assistants
	pollAssistant();
}

static void callbackAcSensorFlags(void)
{
	VeCurrentSensor *sensor = &vebusSystem.iSense.sensors[vebusSystem.iSense.count];

	logI("vas", "got AC sensors flags %X", sensor->infoFlags);
	logI("vas", "get initial VA's counter");
	vebusGetVar(vebusSystem.assistant.pollDev, sensor->ramVarOffset + 4, callbackAssistantKwhInit, &sensor->kwhCounter);
}

/*
 * runtime variables.
 *
 * Note: the received two bytes kwh counter can overflow. Hence kwhCounter
 * keeps track of the last received value. tmpCounter contains the new value
 * and hence overflows don't cause a problem. kwhTotal contains the total
 * starting from 0 since the last init.
 */
static void callbackAssistantKwh(void)
{
	un16 delta;
	VeCurrentSensor *sensor = &vebusSystem.iSense.sensors[vebusSystem.iSense.pollSensor];

	logI("vas", "got assistant Joules %d", vebusSystem.iSense.tmpCounter);
	delta = vebusSystem.iSense.tmpCounter - sensor->kwhCounter;
	sensor->kwhTotal += delta;
	sensor->kwhCounter = vebusSystem.iSense.tmpCounter;

	logI("vas", "total kWh %u 2^16 J", sensor->kwhTotal);

	/* this sensor is done, check the next one */
	vebusSystem.iSense.pollSensor++;
	vebusSystem.iSense.callback();
}

static void callbackAssistantVoltage(void)
{
	VeCurrentSensor *sensor = &vebusSystem.iSense.sensors[vebusSystem.iSense.pollSensor];

	logI("vas", "got assistant voltage %d", sensor->voltage);

	logI("vas", "get assistant VA's counter");
	vebusGetVar(sensor->dev, sensor->ramVarOffset + 4, callbackAssistantKwh, &vebusSystem.iSense.tmpCounter);
}

static void callbackAssistantCurrent(void)
{
	VeCurrentSensor *sensor = &vebusSystem.iSense.sensors[vebusSystem.iSense.pollSensor];
	un8 id;

	logI("vas", "got assistant current (%d)", sensor->current);

	id = ((sensor->infoFlags & 0x1F) == 0 ? VE_VAR_UINPUT_RMS_RAMVAR : VE_VAR_UINVERTER_RMS_RAMVAR);
	logI("vas", "get assistant voltage (id = %d)", id);
	vebusGetVar(sensor->dev, id, callbackAssistantVoltage, &sensor->voltage);
}

veBool vebusGetCurrentSensors(Mk2Callback *callback)
{
	VeCurrentSensor *sensor;

	veAssert(callback != NULL);
	vebusSystem.iSense.callback = callback;

	if (vebusSystem.iSense.pollSensor >= vebusSystem.iSense.count) {
		vebusSystem.iSense.pollSensor = 0;
		return veFalse;
	}

	sensor = &vebusSystem.iSense.sensors[vebusSystem.iSense.pollSensor];
	logI("vas", "get assistant current (%d) on device %d", vebusSystem.iSense.pollSensor, sensor->dev->addr);
	vebusGetVar(sensor->dev, sensor->ramVarOffset + 2, callbackAssistantCurrent, &sensor->current);

	return veTrue;
}

/* If there are more sensors to update */
veBool vebusCurrentSensorsMoreTodo(void)
{
	return vebusSystem.iSense.pollSensor < vebusSystem.iSense.count;
}

#endif

static void pollAssistantCallback(void)
{
	un8 scanId;

	logI("vas", "assistant id %X", vebusSystem.assistant.identifier);

	if (vebusSystem.assistant.identifier == 0) {
		logI("vas", "no assistant on %d", vebusSystem.assistant.scanId);
		vebusSystem.iSense.callback();
		return;
	}

	scanId = vebusSystem.assistant.scanId;
	vebusSystem.assistant.scanId += (vebusSystem.assistant.identifier & 0xF) + 1;

	switch (vebusSystem.assistant.identifier >> 4)
	{
#if CFG_MK2_AC_SENSOR
	case 1:
		{
			VeCurrentSensor *sensor;

			if (vebusSystem.iSense.count < ARRAY_LENGTH(vebusSystem.iSense.sensors)) {
				sensor = &vebusSystem.iSense.sensors[vebusSystem.iSense.count];
				logI("vas", "current sensor assistant found");
				sensor->dev = vebusSystem.assistant.pollDev;
				sensor->ramVarOffset = scanId;

				/* Get info flags and init kwhcounter first, thereafter continue with the next assistant */
				vebusGetVar(vebusSystem.assistant.pollDev, scanId + 1, callbackAcSensorFlags, &sensor->infoFlags);
				return;
			} else {
				logW("vas", "max current sensors reached!");
			}
		}
		break;
#endif
	case 2:
		vebusSystem.hub1.ramVarOffset = scanId;
		break;
	}

	vebusAssistantFoundCallback(vebusSystem.assistant.pollDev, vebusSystem.assistant.identifier >> 4,
								scanId, vebusSystem.assistant.identifier & 0xF);

	pollAssistant();
}

static void pollAssistant(void)
{
	logI("vas", "checking assistant presence at %X", vebusSystem.assistant.scanId);
	vebusSystem.assistant.identifier = 0;
	vebusGetVar(vebusSystem.assistant.pollDev, vebusSystem.assistant.scanId, pollAssistantCallback, &vebusSystem.assistant.identifier);
}

/*
 * Poll for the presence of the assistants. When veTrue is returned, the
 * callback will be invoke after checking one device. The callback can poll for
 * more assistants on the remaining devices by calling this function again till
 * it returns veFalse.
 */
veBool vebusAssistantsPoll(Mk2Callback *callback)
{
	veAssert(callback != NULL);
	vebusSystem.iSense.callback = callback;

	if (vebusSystem.assistant.pollDev == NULL) {
		/* start of discovery */
		vebusSystem.assistant.pollDev = vebusSystem.devices;
		vebusSystem.iSense.count = 0;
		vebusSystem.hub1.ramVarOffset = 0;
		veAssert(vebusSystem.devices != NULL);
	} else {
		/* poll next device */
		vebusSystem.assistant.pollDev = vebusSystem.assistant.pollDev->next;
		if (vebusSystem.assistant.pollDev == NULL) {
			logI("vas", "no devices left to poll...");
			return veFalse;
		}
	}

	/*
	 * Assistants use the RAM vars from 80 and contain a id and
	 * the number of variables they use. An empty record contains 0
	 * indicating that no more assistants are present.
	 */
	vebusSystem.assistant.scanId = 0x80;
	pollAssistant();

	return veTrue;
}


/** Power related state variables */
static un16 powerTemp;
static VebusDevice *powerDevice;
static Mk2Callback *powerCallback;
static VebusPower totalPower;

static veBool vebusGetPowerFields(void);

/**
 * Optional workaround for multi software (till feb 2015), see
 * needPowerWorkaround above for details.
 */
sn32 vebusUnpackRamPowerWorkaround(struct VebusDeviceS* dev, sn32 raw, un8 id, un16 mul)
{
	sn32 value = vebusUnpackRam(dev, raw, id, mul);

	if (needPowerWorkaround()) {
		/*
		 * Don't try to fixup ouput power since if can be derived from input +
		 * inverter, but only after the are fixup, so better fix the totals.
		 */
		veAssert(id != VE_VAR_POUTPUT_RAMVAR);

		if (id == VE_VAR_PINVERTER_RAMVAR || id == VE_VAR_PINPUT_RAMVAR)
		{
			un8 phase = dev->addr % 3;

			/* For slaves just report no power at all, all is reported from the phase master */
			if (dev->addr >= 3) {
				logI("power", "workaround for slave, addr = %hhu, phase = %hhu", dev->addr, phase);
				return 0;
			}

			/* To correct the total power, multiply the master by the number of devices per phase */
			if (id == VE_VAR_PINPUT_RAMVAR) {
				logI("power", "workaround for master PINPUT_RAMVAR, addr=%hhu, phase = %hhu, count %hhu", dev->addr, phase, vebusSystem.ac[phase].backfeedFactor);
				value *= vebusSystem.ac[phase].backfeedFactor;
			} else if (id == VE_VAR_PINVERTER_RAMVAR) {
				un8 n = 0;
				VebusDevice *itt = vebusSystem.devices;

				/* Count the number of devices in this phase and multiply the power by this factor */
				while (itt) {
					if ((itt->addr % 3) == phase)
						n++;
					itt = itt->next;
				}
				value *= n;

				logI("power", "workaround for master PINVERTER_RAMVAR, addr=%hhu, phase = %hhu count=%hhu", dev->addr, phase, n);
			}
		}
	}

	return value;
}

static Mk2EventReply powerHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	VebusDevice *dev = (VebusDevice *) mk2.fh.msgOut->ctx;
	sn16 value;
	un8 phase;

	if (ev == MK2_FRAME && mk2IsXYZ(msg, WM_RSP_UNKNOWN_COMMAND)) {
		vebusSystem.power.flags |= VE_SNAPSHOT_NOT_SUPPORTED;
		return MK2_DONE;
	}

	if (ev != MK2_FRAME || !mk2IsXYZ(msg, WM_RSP_SNAPSHOT))
		return MK2_CONTINUE;

	/*
	 * In case the snapshot is read, but not received it is reset.
	 * Restart by creating the snapshots again if an empty snapshot is encountered.
	 */
	if (msg->length < 9) {
		vebusCreatePowerSnapshot(mk2.fh.msgOut->callback);
		mk2.fh.msgOut->callback = NULL;
		return MK2_DONE;
	}

	/* Assume the short address modulus 3 represents the phase of the device */
	phase = dev->addr % 0x3;

	logI("vebus_system", "++++ got power snapshot for %hhu ++++", phase);

	/* battery / inverter */
	value = (sn16) le_u16_to_cpu_p(&msg->buf[3]);
	logI("snap-power", "pinverter: %hdW", value);
	totalPower.inverter[phase] += vebusUnpackRamPowerWorkaround(dev, value, VE_VAR_PINVERTER_RAMVAR, 1);

	value = (sn16) le_u16_to_cpu_p(&msg->buf[5]);
	logI("snap-power", "pinput: %hdW", value);
	totalPower.input[phase] -= vebusUnpackRamPowerWorkaround(dev, value, VE_VAR_PINPUT_RAMVAR, 1);

	/*
	 * Power workaround, if the pinverter / pinput is adjusted, make
	 * sure the total output is adjusted accordingly.
	 */
	if (needPowerWorkaround()) {
		totalPower.output[phase] = totalPower.input[phase] - totalPower.inverter[phase];
		logI("snap-power", "workaround, poutput: %hdW", value);
		totalPower.measurementType = VE_SYS_AC_POWER_REAL_SNAPSHOT_MASTERS;
	} else {
		value = (sn16) le_u16_to_cpu_p(&msg->buf[7]);
		logI("snap-power", "poutput: %hdW", value);
		totalPower.output[phase] += vebusUnpackRam(dev, value, VE_VAR_POUTPUT_RAMVAR, 1);
		totalPower.measurementType = VE_SYS_AC_POWER_REAL_SNAPSHOT;
	}

	/* Get the values from the next device if any */
	dev = (VebusDevice *) mk2.fh.msgOut->ctx;
	if (dev->next) {
		vebusGetPowerSnapshot(dev->next, mk2.fh.msgOut->callback);
		mk2.fh.msgOut->callback = NULL; /* still not done */
		return MK2_DONE;
	} else {
		/* last device, mark power as valid */
		vebusSystem.power = totalPower;
		vebusSystem.power.flags |= VE_POWER_VALID;
	}

	/* Done -> keep the callback */
	return MK2_DONE;
}

/* Get the snapshot */
static void vebusGetPowerSnapshot(VebusDevice *dev, Mk2Callback *callback)
{
	Mk2MsgTx *msg = mk2MsgXYZ(dev->addr, 0x38);
	veAssert(msg);
	if (!msg)
		return;
	msg->ctx = dev;
	msg->handler = powerHandler;
	msg->callback = callback;
	logI("vebus_system", "Get power snapshot results from %d", dev->addr);
	mk2FhQueueFrame(msg);
}

/* @note: There is no reply to a snapshot request */
static Mk2EventReply snapshotHandler(Mk2Event mk2event, Mk2MsgRx *msg)
{
	VE_UNUSED(msg);

	if (mk2event == MK2_TX_SENT) {
		/* Directly chain getting the snapshot values */
		vebusGetPowerSnapshot(vebusSystem.devices, mk2.fh.msgOut->callback);
		mk2.fh.msgOut->callback = NULL; /* not yet done */
		return MK2_DONE;
	}

	return MK2_CONTINUE;
}

/**
 * This function freezes the power values of the multi and
 * retreives them from all devices.
 *
 * Snapshots: 0xFF 'F' <6> <ID> [ <ID> [ <ID> [ <ID> [ <ID> [ <ID> ]]]]]
 *
 * This will take a snapshot of maximum 6 RAM IDs.
 * The snapshot can be read by the winmon command 0x38
 * Note that a snapshot result is always returned as 16 bits.
 * Scaling and info is identical as when reading the RAM value with the
 * CommandReadRAMVar (winmon command 0x30)
 *
 * @note: 14-03-2014: At the moment only the 300 version supports snapshot mode.
 */
static void vebusCreatePowerSnapshot(Mk2Callback *callback)
{
	Mk2MsgTx* msg = reqInfoFrameMsg(6); /* 6 opcode for snapshot */
	veAssert(msg);
	if (!msg)
		return;

	logI("vebus_system", "Create power snapshot");
	msg->callback = callback;
	msg->handler = snapshotHandler;
	msg->buf[3] = VE_VAR_PINVERTER_RAMVAR;
	msg->buf[4] = VE_VAR_PINPUT_RAMVAR;
	msg->buf[5] = VE_VAR_POUTPUT_RAMVAR;
	msg->length = 6;
	mk2FhQueueFrame(msg);
}

/* Next power device */
static void checkNextPowerDevice(void)
{
	if (powerDevice->next) {
		powerDevice = powerDevice->next;
		vebusGetPowerFields();
	} else {
		/* Keep flags and assign values */
		totalPower.flags = vebusSystem.power.flags | VE_POWER_VALID;
		vebusSystem.power = totalPower;
		vebusSystem.power.measurementType = needPowerWorkaround() ? VE_SYS_AC_POWER_REAL_SUMMED_MASTERS : VE_SYS_AC_POWER_REAL_SUMMED;
		powerCallback();
	}
}

/* Input power obtained -> derive output and check next device */
void inputPowerDone(void)
{
	un8 phase = powerDevice->addr % 0x3;
	sn32 value = vebusUnpackRamPowerWorkaround(powerDevice, powerTemp, VE_VAR_PINPUT_RAMVAR, 1);

	totalPower.input[phase] -= value;

	/*
	 * Update the output power accordingly, strictly speaking this is only needed once per
	 * phase, after the last device of the phase its power values are reached. Since the device
	 * list is not ordered it is more work to evaluate that, then to calcalute it multiple times.
	 */
	totalPower.output[phase] = totalPower.input[phase] - totalPower.inverter[phase];

	logI("power", "received %hhu, phase %hhu: INPUT POWER %dW", powerDevice->addr, phase, value);

	logI("power", "added %hhu, phase %hhu: in %dW, inverter %dW, out %dW", powerDevice->addr, phase,
		 totalPower.input[phase], totalPower.inverter[phase], totalPower.output[phase]);

	checkNextPowerDevice();
}

/* Inverter / DC power obtained */
void inverterPowerDone(void)
{
	un8 phase = powerDevice->addr % 0x3;
	sn32 value = vebusUnpackRamPowerWorkaround(powerDevice, powerTemp, VE_VAR_PINVERTER_RAMVAR, 1);
	totalPower.inverter[phase] += value;
	logI("power", "received %hhu, phase %hhu: INVERTER POWER %dW", powerDevice->addr, phase, value);

	/* always supported if inverter power was supported */
	vebusGetVar(powerDevice, VE_VAR_PINPUT_RAMVAR, inputPowerDone, &powerTemp);
}

/**
 * Request the seperate power fields, returns veFalse when not supported.
 *
 * @note needs valid powerDevice / PowerCallback, see below...
 */
static veBool vebusGetPowerFields(void)
{
	un8 id = VE_VAR_PINVERTER_RAMVAR;

	if (!vebusSupportedRamVar(powerDevice, id))
		return veFalse;
	vebusGetVar(powerDevice, id, inverterPowerDone, &powerTemp);

	return veTrue;
}

/**
 * vebusGetPower
 *
 * Gets the vebusPower from the system. Since this is added in later firmware
 * version, the exact method depends on the devices.
 *
 * returns veFalse when not supported.
 * true and calls the callback when the values in vebusPower are updated.
 * flags contains the valid flag.
 */
veBool vebusGetPower(Mk2Callback *callback)
{
	memset(&totalPower, 0, sizeof(totalPower));

	if (vebusSystem.power.flags & VE_SNAPSHOT_NOT_SUPPORTED) {
		powerDevice = vebusSystem.devices;
		powerCallback = callback;
		if (!vebusGetPowerFields())
			return veFalse;
		logI("power", "POWER LOOPING DEVICES");
	} else {
		/*
		 * The first time snapshot mode is always checked, if not
		 * supported it will be marked as such and individual values
		 * will be obtained. If that doesn't work the apparent power
		 * can be used.
		 */
		logI("power", "POWER SNAPSHOT");
		vebusCreatePowerSnapshot(callback);
	}

	return veTrue;
}

/**
 * Startup in syncing mode, if no panel is found after the timeout period
 * the state will be set to pos.
 */
void vebusControlInit(void)
{
	mk2.state.mode = SP_UNKNOWN;
	vebusSystem.ctrl.timer = PANEL_TIMEOUT;

	vebusSystem.ctrl.state = VEBUS_PANEL_DISABLED;
	mk2.state.limit.current.value = 0xFFFF;
	vebusSystem.kwh.state = VE_KWH_IDLE;
	vebusSystem.power.flags = 0;
}

/** update the shortIds */
static Mk2EventReply configHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	if (ev != MK2_FRAME || msg->length < 15 || msg->buf[0] != 0x20 /* VEBUS_RSP_INFO */)
		return MK2_CONTINUE;

	logI("vebus_system", "++++ got vebus setting info (for shortIds) ++++");
	vebusSystem.state.shortIds = le_u32_to_cpu_p(&msg->buf[8]);

	return MK2_DONE;
}

/**
 * Get the shortids.
 *
 * This will attempt to get the configuration of the system from the master.
 * vebusSystem.state.shortIds will hereafter have the bits set for devices
 * that should be present.
 */
void vebusGetShortIds(Mk2Callback *callback)
{
	Mk2MsgTx* msg = vebusGetConfigMsg(0);
	veAssert(msg);
	if (!msg)
		return;

	logI("vebus_system", "---- get vebus setting info ----");
	msg->callback = callback;
	msg->handler = configHandler;

	mk2FhQueueFrame(msg);
}
