#include <math.h>
#include <string.h>

#include <velib/velib_inc_mk2.h>
#include <velib/base/endian.h>
#include <velib/mk2/frame_handler.h>
#include <velib/mk2/mk2.h>
#include <velib/mk2/vebus_device.h>
#include <velib/utils/ve_assert.h>
#include <velib/utils/ve_logger.h>

/**
 * Communication with a specific vebus device. Multiple vebus devices
 * can create a vebus system. System values are in vebus_system.c. This
 * contains the per device values. This is not a perfect seperation,
 * since there are system values which are requested from the master.
 * (SOC for example)
 */

static Mk2EventReply setSettingHandler(Mk2Event ev, Mk2MsgRx *msg);
static Mk2EventReply setVarHandler(Mk2Event ev, Mk2MsgRx *msg);

/** Copy context of one message to another */
static void vebusCopyMsgCtx(Mk2MsgTx* msg)
{
	msg->callback = mk2.fh.msgOut->callback;
	msg->ctx = mk2.fh.msgOut->ctx;
	msg->rsp = mk2.fh.msgOut->rsp;
	msg->id = mk2.fh.msgOut->id;
	msg->retries = mk2.fh.msgOut->retries;
	mk2.fh.msgOut->callback = NULL;
}

#if CFG_MK2_ALLOW_COMPAT_DISABLED

static veBool v2xxFixupNeeded(VebusDevice* dev)
{
	/*
	 * If the mk2 is not in backwards compatible mode v2xx and up must be
	 * treated seperately since the responses are different.
	 */
	return (mk2.state.sFlagsExt0 & MK2_S_FLAG_E0_COMPAT_SPLIT_FRAMES) == 0 &&
			(dev->version.features & VEBUS_VERION_F_COMBINED_FRAME);
}

static Mk2Handler* v2xxFixup(VebusDevice* dev, Mk2Handler* generic, Mk2Handler* v2xx)
{
	return (v2xxFixupNeeded(dev) ? v2xx : generic);
}

/*
 * Scaling to interprete a v2++ vebus response.
 *
 * > 05 FF 'Y 36 00 00 [6D]
 * < 07 FF 'Y 8E 9C 7F 00 00 [F8]
 * < 03 FF 'Z 8E [16] (not supported)
 */
static Mk2EventReply v2xxRamInfoHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	VebusRamInfo *info = (VebusRamInfo *) mk2.fh.msgOut->ctx;

	if (ev != MK2_FRAME || !mk2IsXYZNoData(msg, WM_RSP_VAR_SCALE))
		return MK2_CONTINUE;

	if (msg->length >= 7) {
		info->scale = le_u16_to_cpu_p(&msg->buf[3]);
		info->offset = le_u16_to_cpu_p(&msg->buf[5]);
	}
	logI("winmon", "ram info %d scale=%d offset=%d", mk2.fh.msgOut->id, info->scale, info->offset);

	return MK2_DONE;
}

/*
 * Setting info handler part for combined v2++ frames. These
 * are the same as the old format above but the response codes
 * removed. Furtermore, not supperted is now an empty message.
 *
 * < 0D FF 'Z 89 01 00 00 00 03 00 01 00 03 00 [09]
 * < 03 FF 'Y 89 [1C] (not supported)
 */
static Mk2EventReply v2xxSettingInfoHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	VebusSettingInfo *info;

	if (ev != MK2_FRAME || !mk2IsXYZNoData(msg, WM_RSP_SET_SCALE))
		return MK2_CONTINUE;

	info = (VebusSettingInfo*) mk2.fh.msgOut->ctx;

	if (msg->length >= 13) {
		info->scale = le_s16_to_cpu_p(&msg->buf[3]);
		info->offset = le_u16_to_cpu_p(&msg->buf[5]);
		info->defaultValue = le_u16_to_cpu_p(&msg->buf[7]);
		info->minimum = le_u16_to_cpu_p(&msg->buf[9]);
		info->maximum = le_u16_to_cpu_p(&msg->buf[11]);
		if (msg->length > 13)
			info->accessLevel = msg->buf[13]; /* multi firmware  >= 308*/
		else
			info->accessLevel = 0;

		logI("winmon", "setting info %d scale=%d offset=%d [%d..%d] def=%d access=0x%x", (int) mk2.fh.msgOut->id, (int) info->scale, (int) info->offset, (int) info->minimum, (int) info->maximum, (int) info->defaultValue, info->accessLevel);

	} else {
		info->scale = 0;
		logI("winmon", "setting info %d not supported", (int) mk2.fh.msgOut->id);
	}

	return MK2_DONE;
}

static Mk2EventReply v2xxSetIdHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	struct VebusDeviceS* dev = (struct VebusDeviceS*) mk2.fh.msgOut->ctx;

	if (ev != MK2_FRAME || (
					!mk2IsXYZNoData(msg, mk2.fh.msgOut->rsp) &&
					!mk2IsXYZNoData(msg, WM_RSP_UNKNOWN_COMMAND) &&
					!mk2IsXYZNoData(msg, WM_RSP_ACCESS_LEVEL_REQUIRED)
				)
		)
		return MK2_CONTINUE;

	if (msg->buf[2] == WM_RSP_UNKNOWN_COMMAND) {
		logE("winmon", "v2xxSetIdHandler command not supported!");
		return MK2_CONTINUE;
	}

	if (msg->buf[2] == WM_RSP_ACCESS_LEVEL_REQUIRED) {
		dev->accessLevelRequired = msg->buf[3];
		return MK2_DONE;
	}

	if (msg->length > 3) {
		if (msg->buf[3] & 0x1)
			dev->resetRequired = veTrue;
	}

	return MK2_DONE;
}

/*
 * CommandWriteViaID,Flags,ID,DataLo,DataHi
 * Flags[0] = 1 => write Setting otherwise write RAM
 * - WM_FLAG_WRITE_RAMVAR			0x00
 * - WM_FLAG_WRITE_SETTINGS			0x01
 *
 * e.g.
 *   > 07 FF 'Y 37 01 40 D0 07 [52]
 *   < 03 FF 'Y 88 [1D]
 */
static Mk2MsgTx *mk2MsgSetId(un8 addr, un8 kind, un8 id, un16 value)
{
	Mk2MsgTx *msg = mk2MsgXYZ(addr, WM_CMD_WRITE_VIA_ID);

	if (msg == NULL)
		return NULL;

	msg->buf[3] = kind;
	msg->buf[4] = id;
	cpu_to_le_u16_tp(&msg->buf[5], value);
	msg->length = 7;

	return msg;
}

/**
 * Change a setting in a vebus firmware 2++ specific manner.
 * This only works for such vebus firmware and the mk2 backwards
 * compatibility must be disabled.
 */
static veBool vebus2xxSetSetting(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, un16 value)
{
	Mk2MsgTx* msg;

	msg = mk2MsgSetId(dev->addr, WM_FLAG_WRITE_SETTINGS, id, value);
	if (!msg)
		return veFalse;

	dev->accessLevelRequired = veFalse;
	msg->ctx = dev;
	msg->callback = cb;
	msg->handler = v2xxSetIdHandler;
	msg->rsp = WM_RSP_WRITE_SETTING_OK;
	mk2FhQueueFrame(msg);

	return veTrue;
}

static veBool vebus2xxSetVar(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, un16 value)
{
	Mk2MsgTx* msg;

	msg = mk2MsgSetId(dev->addr, WM_FLAG_WRITE_RAMVAR, id, value);
	if (!msg)
		return veFalse;
	msg->callback = cb;
	msg->handler = v2xxSetIdHandler;
	msg->rsp = WM_RSP_WRITE_RAMVAR_OK;
	mk2FhQueueFrame(msg);

	return veTrue;
}
#else

#define v2xxFixupNeeded(dev)				(veFalse)
#define v2xxFixup(dev, generic, v2xx)		(generic)

static veBool vebus2xxSetSetting(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, un16 value)
{
	VE_UNUSED(dev);
	VE_UNUSED(id);
	VE_UNUSED(cb);
	VE_UNUSED(value);
	return veFalse;
}

static veBool vebus2xxSetVar(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, un16 value)
{
	VE_UNUSED(dev);
	VE_UNUSED(id);
	VE_UNUSED(cb);
	VE_UNUSED(value);
	return veFalse;
}

#endif


/*=== version - two seperate requests ===*/
static Mk2EventReply versionPart1Handler(Mk2Event ev, Mk2MsgRx *msg)
{
	VebusVersion *value;

	if (ev != MK2_FRAME || !mk2IsXYZ(msg, WM_RSP_SEND_SOFTWARE_VERSION_PART1))
		return MK2_CONTINUE;

	value = (VebusVersion *) mk2.fh.msgOut->ctx;
	value->number |= ((un32) le_u16_to_cpu_p(&msg->buf[3]) << 16);

	return MK2_DONE;
}

/*
 * The response depends on the multi and mk2 version.
 *   Newer vebus devices can return the whole version at once, which
 *   might be followed by additional data for assistant support e.g.
 *   The mk2 compat mode must be disabled to receive these messages
 *      > 03 FF 'X 05 A1
 *      < 09 FF 'X 82 F0 68 28 00 07 01 96
 * - Older vebus device or mk2 in compat mode will only return 2 bytes.
 *   The second part must be requested seperatly, e.g:
 *      > 03 FF 'X 05 A1
 *      < 05 FF 'X 82 F0 68 CA
 *      > 03 FF 'Y 06 9F
 *      < 05 FF 'Y 83 28 00 F8
 */
static Mk2EventReply versionPart0Handler(Mk2Event ev, Mk2MsgRx *msg)
{
	Mk2MsgTx *txMsg;
	VebusVersion *value;

	if (ev != MK2_FRAME || !mk2IsXYZ(msg, WM_RSP_SEND_SOFTWARE_VERSION_PART0))
		return MK2_CONTINUE;

	value = (VebusVersion *) mk2.fh.msgOut->ctx;

	if (msg->length < 7) { /* +3 since  FF 'X 82 */
		/*
		 * If there are less then 4 data bytes, assume this is a
		 * partial software version response.
		 */
		value->number = le_u16_to_cpu_p(&msg->buf[3]);

		txMsg = mk2MsgXYZ(mk2.actualAddress, WM_CMD_SEND_SOFTWARE_VERSION_PART1);
		if (!txMsg)
			return MK2_CONTINUE;
		txMsg->handler = versionPart1Handler;
		vebusCopyMsgCtx(txMsg);
		mk2FhQueueFrame(txMsg);
	} else {
		value->number = le_u32_to_cpu_p(&msg->buf[3]);
		value->features |= VEBUS_VERION_F_COMBINED_FRAME;

#if CFG_MK2_VSC_SUPPORT
		/* copy remaining bytes for vsc support */
		value->remainderLength = msg->length - 7;
		if (value->remainderLength <= VEBUS_VERSION_REMAINDER_LEN)
			memcpy(value->remainder, &msg->buf[7], value->remainderLength);
		else
			value->remainderLength = 0;
#endif
	}

	return MK2_DONE;
}

/*
 * Request the vebus device its software version. This can also
 * determine whether combined frames are used. The mk2 must have
 * backwards compatible disabled, see VEBUS_VERION_F_COMBINED_FRAME /
 * mk2SetState(). If a combined frame was received version->features
 * will have the VEBUS_VERION_F_COMBINED_FRAME flag set.
 *
 * When CFG_MK2_VSC_SUPPORT is enabled, additional version info is
 * stored in value->remainder.
 */
void vebusGetTargetVersion(struct VebusDeviceS* dev, Mk2Callback* callback, VebusVersion *version)
{
	Mk2MsgTx* msg = mk2MsgXYZ(dev->addr, WM_CMD_SEND_SOFTWARE_VERSION_PART0);
	if (!msg)
		return;
	logI("winmon", "requesting target version");
	memset(version, 0, sizeof(VebusVersion));
	msg->handler = versionPart0Handler;
	msg->callback = callback;
	msg->ctx = version;
	mk2FhQueueFrame(msg);
}

/* note the old devices will respond with 2 seperate frames */
static Mk2EventReply ramInfoHandlerPart(Mk2Event ev, un8 const* p)
{
	VebusRamInfo* info = (VebusRamInfo*) mk2.fh.msgOut->ctx;
	VE_UNUSED(ev); /* already checked by the caller */
	veAssert(ev == MK2_FRAME);

	if (mk2.fh.msgOut->rsp == WM_RSP_VAR_SCALE) {
		/* 0 = not supported */
		info = (VebusRamInfo*) mk2.fh.msgOut->ctx;
		info->scale = le_u16_to_cpu_p(&p[1]);
		if (!info->scale) {
			logI("winmon", "ram info %d not supported", mk2.fh.msgOut->id, info->scale, info->offset);
			return MK2_DONE;
		}
		mk2.fh.msgOut->rsp = WM_RSP_VAR_OFFSET;
	} else {
		if (p[0] != WM_RSP_VAR_OFFSET)
			return MK2_CONTINUE;

		info->offset = le_s16_to_cpu_p(&p[1]);
		logI("winmon", "ram info %d scale=%d offset=%d", mk2.fh.msgOut->id, info->scale, info->offset);
		return MK2_DONE;
	}

	return MK2_CONTINUE;
}

/*=== scaling to interprete a ram var (and availability) ===*/
static Mk2EventReply ramInfoHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	Mk2EventReply ret;

	if (ev == MK2_TIMEOUT) {
		mk2.fh.msgOut->rsp = WM_RSP_VAR_SCALE;
		return MK2_CONTINUE;
	}

	if (ev != MK2_FRAME || !mk2IsXYZ(msg, (un8) mk2.fh.msgOut->rsp))
		return MK2_CONTINUE;

	ret = ramInfoHandlerPart(ev, &msg->buf[2]);
	if (ret == MK2_DONE)
		return MK2_DONE;

	/* newer devices append the offset */
	if (msg->length >= 8)
		ret = ramInfoHandlerPart(ev, &msg->buf[5]);

	return ret;
}

void vebusGetRamInfo(struct VebusDeviceS* dev, un8 id, Mk2Callback* callback, VebusRamInfo* dst)
{
	Mk2MsgTx* msg = mk2MsgXYZInfo(dev->addr, WM_CMD_GET_RAMVAR_INFO, id);
	if (!msg)
		return;
	msg->handler = v2xxFixup(dev, ramInfoHandler, v2xxRamInfoHandler);
	msg->callback = callback;
	msg->ctx = dst;
	msg->id = id;
	msg->rsp = WM_RSP_VAR_SCALE;
	mk2FhQueueFrame(msg);
}

static Mk2EventReply settingInfoHandlerPart(Mk2Event ev, un8 const* p)
{
	VebusSettingInfo* info;
	VE_UNUSED(ev); /* already checked by the caller */
	veAssert(ev == MK2_FRAME);

	if (p[0] != mk2.fh.msgOut->rsp)
		return MK2_CONTINUE;

	info = (VebusSettingInfo*) mk2.fh.msgOut->ctx;

	switch (mk2.fh.msgOut->rsp) {
	case WM_RSP_SET_SCALE:
		info->scale = le_s16_to_cpu_p(&p[1]);
		/* Zero scale -> setting not supported */
		if (!info->scale) {
			logI("winmon", "setting info %d not supported", (int) mk2.fh.msgOut->id);
			memset(info, 0, sizeof(*info));
			return MK2_DONE;
		}
		mk2.fh.msgOut->rsp = WM_RSP_SET_OFFSET;
		break;

	case WM_RSP_SET_OFFSET:
		info->offset = le_u16_to_cpu_p(&p[1]);
		mk2.fh.msgOut->rsp = WM_RSP_SET_DEFAULT;
		break;

	case WM_RSP_SET_DEFAULT:
		info->defaultValue = le_u16_to_cpu_p(&p[1]);
		mk2.fh.msgOut->rsp = WM_RSP_SET_MINIMUM;
		break;

	case WM_RSP_SET_MINIMUM:
		info->minimum = le_u16_to_cpu_p(&p[1]);
		mk2.fh.msgOut->rsp = WM_RSP_SET_MAXIMUM;
		break;

	case WM_RSP_SET_MAXIMUM:
		info->maximum = le_u16_to_cpu_p(&p[1]);
		/* Note this must be one line or the VVC can't get rid of it.. */
		logI("winmon", "setting info %d scale=%d offset=%d [%d..%d] def=%d", (int) mk2.fh.msgOut->id, (int) info->scale, (int) info->offset, (int) info->minimum, (int) info->maximum, (int) info->defaultValue);
		return MK2_DONE;
	}

	return MK2_CONTINUE;
}

/* The response (likely) contains multiple responses in one frame */
static Mk2EventReply settingInfoHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	Mk2EventReply ret = MK2_CONTINUE;
	un8 n;

	if (ev == MK2_TIMEOUT) {
		mk2.fh.msgOut->rsp = WM_RSP_SET_SCALE;
		return ret;
	}

	if (ev != MK2_FRAME || !mk2IsXYZ(msg, (un8) mk2.fh.msgOut->rsp))
		return MK2_CONTINUE;

	n = 2;
	while (n <= (msg->length - 3)) {
		ret = settingInfoHandlerPart(ev, &msg->buf[n]);
		if (ret == MK2_DONE)
			return MK2_DONE;
		n += 3;
	}

	return ret;
}

/*
 * Request for scaling information belonging to settingID id.
 * Obtained values are stored in ctx and cb is invoked afterwards.
 */
void vebusGetSettingInfo(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, VebusSettingInfo* ctx)
{
	Mk2MsgTx* msg = mk2MsgXYZInfo(dev->addr, WM_CMD_GET_SETTING_INFO, id);
	if (!msg)
		return;
	msg->handler = v2xxFixup(dev, settingInfoHandler, v2xxSettingInfoHandler);
	msg->callback = cb;
	msg->ctx = ctx;
	msg->id = id;
	msg->rsp = WM_RSP_SET_SCALE;
	mk2FhQueueFrame(msg);
}

/*=== get an unscaled setting ===*/
static Mk2EventReply settingHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	sn16* out = (sn16*) mk2.fh.msgOut->ctx;

	if ( ev != MK2_FRAME ||
			!(
				mk2IsXYZ(msg, WM_RSP_READ_SETTING_OK) ||
				mk2IsXYZNoData(msg, WM_RSP_SETTING_NOT_SUPPORTED)
			)
		)
		return MK2_CONTINUE;

	if (msg->buf[2] == WM_RSP_SETTING_NOT_SUPPORTED)
		*out = 0;
	else
		*out = le_s16_to_cpu_p(&msg->buf[3]);

	return MK2_DONE;
}

/* Get an actual setting */
void vebusGetSetting(struct VebusDeviceS* dev, un8 id, Mk2Callback* callback, un16 *out)
{
	Mk2MsgTx* msg = mk2MsgXYZInfo(dev->addr, WM_CMD_READ_SETTING, id);
	if (!msg)
		return;
	logI("mk2", "get setting %d from %d", id, dev->addr);
	msg->handler = settingHandler;
	msg->callback = callback;
	msg->ctx = out;
	msg->rsp = id;
	mk2FhQueueFrame(msg);
}

/*=== set an unscaled setting (two messages) ===*/
static Mk2EventReply setSettingDataHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	if (ev == MK2_TIMEOUT) {
		Mk2MsgTx *newMsg;

		if (mk2.fh.msgOut->retries == 0) {
			logW("vebus_device", "setSettingDataHandler no answer");
			return MK2_CONTINUE; // stop attempting
		}

		/* Since changing a setting needs 2 messages, resend the first again */
		logI("mk2", "resending set setting %d for dev %d", mk2.fh.msgOut->id, mk2.actualAddress);
		newMsg = mk2MsgXYZInfo(mk2.actualAddress, WM_CMD_WRITE_SETTING, mk2.fh.msgOut->id);
		vebusCopyMsgCtx(newMsg);
		newMsg->handler = setSettingHandler;
		mk2FhQueueFrame(newMsg);
		return MK2_DONE;
	}

	if (ev != MK2_FRAME || !mk2IsXYZ(msg, WM_RSP_WRITE_SETTING_OK) )
		return MK2_CONTINUE;

	return MK2_DONE;
}

static Mk2EventReply setSettingHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	Mk2MsgTx* txMsg;
	VE_UNUSED(msg);

	if (ev != MK2_TX_SENT)
		return MK2_CONTINUE;

	txMsg = mk2MsgXYZInfo(mk2.actualAddress, WM_CMD_WRITE_DATA, mk2.fh.msgOut->rsp);
	if (!txMsg)
		return MK2_CONTINUE;
	logI("mk2", "sending data for set %d to %d", mk2.fh.msgOut->id, mk2.fh.msgOut->rsp);
	vebusCopyMsgCtx(txMsg);
	txMsg->handler = setSettingDataHandler;
	mk2FhQueueFrame(txMsg);

	return MK2_DONE;
}

/* note: 2 seperate message, the first hooks into the tx_sent event */
veBool vebusSetSetting(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, un16 value)
{
	Mk2MsgTx* msg;

	if (!vebusValidRawSetting(dev, id, value))
		return veFalse;

	if (v2xxFixupNeeded(dev))
		return vebus2xxSetSetting(dev, id, cb, value);

	msg = mk2MsgXYZInfo(dev->addr, WM_CMD_WRITE_SETTING, id);
	if (!msg)
		return veFalse;

	logI("mk2", "set setting %d for dev %d to %hu", id, dev->addr, value);
	msg->handler = setSettingHandler;
	msg->callback = cb;
	msg->rsp = value;
	msg->id = id;
	mk2FhQueueFrame(msg);
	return veTrue;
}

/** */
static Mk2EventReply converterStateHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	VebusConverterState* info = mk2.fh.msgOut->ctx;

	if (ev == MK2_FRAME && mk2IsXYZ(msg, WM_RSP_UNKNOWN_COMMAND)) {
		info->state = VE_STATE_UNKNOWN;
		info->substate = VE_SUBSTATE_CHRG_UNKNOWN;
		return MK2_DONE;
	}

	if (ev != MK2_FRAME || !mk2IsXYZ(msg, WM_RSP_GET_DEVICE_STATE))
		return MK2_CONTINUE;

	logI("mk2", "got device state (bulk / inverting..) ");
	info->state = msg->buf[3];
	if (info->state > VE_STATE_UNKNOWN)
		info->state = VE_STATE_UNKNOWN;

	info->substate = msg->buf[4];
	if (info->substate > VE_SUBSTATE_CHRG_UNKNOWN)
		info->substate = VE_SUBSTATE_CHRG_UNKNOWN;
	return MK2_DONE;
}

void vebusGetConverterState(struct VebusDeviceS* dev, Mk2Callback* callback, VebusConverterState* info)
{
	Mk2MsgTx* msg = mk2MsgXYZ(dev->addr, WM_CMD_GET_SET_DEVICE_STATE);
	if (!msg)
		return;

	logI("mk2", "get device state (bulk / inverting..) ");
	msg->buf[3] = 0; /* request */
	msg->length++;
	msg->ctx = info;
	msg->callback = callback;
	msg->handler = converterStateHandler;
	mk2FhQueueFrame(msg);
}

veBool vebusValidRawSetting(struct VebusDeviceS* dev, un8 id, un16 value)
{
	VebusSettingInfo* settingInfo = vebusGetSettingInfoOfDevice(dev);
	VebusSettingInfo* info;

	if (!settingInfo)
		return veFalse;

	info = &settingInfo[id];
	/*
	 * old: When changing flags, do not set bits for unsupported settings, as
	 * this can cause the values of other flags to be changed.
	 *
	 * new: Flags should always be considered valid. The maximum setting indicates
	 * the supported bit. Unsupported bits will be ignored by the vebus devices
	 * anyway, so sending more flags is harmless.
	 */
	if (vebusGetUnit(id, VE_UNITS_MULTI) == VE_U_FLAGS)
		return veTrue;

	return value >= info->minimum && value <= info->maximum;
}

/*=== get an unscaled variable ===*/
static Mk2EventReply varHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	un16* out = (un16*) mk2.fh.msgOut->ctx;

	if ( ev != MK2_FRAME ||
			!(
				mk2IsXYZ(msg, WM_RSP_READ_RAM_OK) ||
				mk2IsXYZNoData(msg, WM_RSP_VAR_NOT_SUPPORTED)
			)
		)
		return MK2_CONTINUE;

	if (msg->buf[2] == WM_RSP_VAR_NOT_SUPPORTED) {
		*out = 0;
		logW("mk2", "++++ ramvar %d not supported ----", mk2.fh.msgOut->rsp);
	} else {
		*out = le_s16_to_cpu_p(&msg->buf[3]);
		logI("mk2", "++++ got ramvar %d value %d ----", mk2.fh.msgOut->rsp, *out);
	}

	return MK2_DONE;
}

void vebusGetVar(struct VebusDeviceS* dev, un8 id, Mk2Callback* callback, un16* out)
{
	Mk2MsgTx* msg = mk2MsgXYZInfo(dev->addr, WM_CMD_READ_RAMVAR, id);
	if (!msg)
		return;
	logI("mk2", "---- get ramvar %d from %d ----", id, dev->addr);
	msg->handler = varHandler;
	msg->callback = callback;
	msg->ctx = out;
	msg->rsp = id;
	mk2FhQueueFrame(msg);
}

/*=== set an unscaled ramvar (two messages) ===*/
static Mk2EventReply setVarDataHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	if (ev == MK2_TIMEOUT) {
		Mk2MsgTx* newMsg;

		if (mk2.fh.msgOut->retries == 0) {
			logW("vebus_device", "setVarDataHandler timeout");
			return MK2_CONTINUE;
		}

		/* Since changing a setting needs 2 messages, resend the first again */
		newMsg = mk2MsgXYZInfo(mk2.actualAddress, WM_CMD_WRITE_RAMVAR, mk2.fh.msgOut->id);
		newMsg->handler = setVarHandler;
		vebusCopyMsgCtx(newMsg);
		mk2FhQueueFrame(newMsg);
		return MK2_DONE;
	}

	if (ev != MK2_FRAME || !mk2IsXYZ(msg, WM_RSP_WRITE_RAMVAR_OK) )
		return MK2_CONTINUE;

	return MK2_DONE;
}

static Mk2EventReply setVarHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	Mk2MsgTx* txMsg;
	VE_UNUSED(msg);

	if (ev != MK2_TX_SENT)
		return MK2_CONTINUE;

	txMsg = mk2MsgXYZInfo(mk2.actualAddress, WM_CMD_WRITE_DATA, mk2.fh.msgOut->rsp);
	if (!txMsg)
		return MK2_CONTINUE;
	vebusCopyMsgCtx(txMsg);
	txMsg->handler = setVarDataHandler;
	mk2FhQueueFrame(txMsg);

	return MK2_DONE;
}

/*
 * Update the ramvarId to the raw value given.
 * Call callback when done.
 */
veBool vebusSetVar(struct VebusDeviceS* dev, un8 id, Mk2Callback* cb, un16 value)
{
	Mk2MsgTx* msg;

	if (v2xxFixupNeeded(dev))
		return vebus2xxSetVar(dev, id, cb, value);

	msg = mk2MsgXYZInfo(dev->addr, WM_CMD_WRITE_RAMVAR, id);
	if (!msg)
		return veFalse;

	msg->handler = setVarHandler;
	msg->callback = cb;
	msg->rsp = value;
	msg->id = id;
	mk2FhQueueFrame(msg);
	return veTrue;
}

un16 vebusGetFlagRawDefault(struct VebusDeviceS* dev, un8 id)
{
	un8 bit = id & 0xF;
	un8 flags = vebusGetSettingIdFromFlag(id);
	VebusSettingInfo* settingInfo = vebusGetSettingInfoOfDevice(dev);

	if (flags == VEBUS_ID_INVALID)
		return 0;
	return (settingInfo[flags].defaultValue & (1 << bit) ? 1 : 0);
}

un16 vebusGetSettingRawDefault(struct VebusDeviceS* dev, un8 id)
{
	VebusSettingInfo* settingInfo = vebusGetSettingInfoOfDevice(dev);

	if (id >= VEBUS_SETTINGS_COUNT || !settingInfo)
		return 0;

	return settingInfo[id].defaultValue;
}

un16 vebusGetSettingRawMinimum(struct VebusDeviceS* dev, un8 id)
{
	VebusSettingInfo* settingInfo = vebusGetSettingInfoOfDevice(dev);

	if (id >= VEBUS_SETTINGS_COUNT || !settingInfo)
		return 0;
	return settingInfo[id].minimum;
}

un16 vebusGetSettingRawMaximum(struct VebusDeviceS* dev, un8 id)
{
	VebusSettingInfo* settingInfo = vebusGetSettingInfoOfDevice(dev);

	if (id >= VEBUS_SETTINGS_COUNT || !settingInfo)
		return 1;
	return settingInfo[id].maximum;
}

veBool vebusSupportedSetting(struct VebusDeviceS* dev, un8 id)
{
	VebusSettingInfo* settingInfo;

	if ( (settingInfo = vebusGetSettingInfoOfDevice(dev)) )
		return settingInfo[id].scale != 0;
	return veFalse;
}

veBool vebusSupportedFlag(struct VebusDeviceS *dev, un8 id)
{
	un8 bit = id & 0xF;
	un8 flags = vebusGetSettingIdFromFlag(id);
	VebusSettingInfo* settingInfo = vebusGetSettingInfoOfDevice(dev);

	if (flags == VEBUS_ID_INVALID || vebusGetFlagUnit(id) == VE_U_DFLAG || settingInfo)
		return veFalse;

	return (settingInfo[flags].maximum & (1 << bit) ? veTrue : veFalse);
}

veBool vebusSupportedRamVar(struct VebusDeviceS* dev, un8 id)
{
	VebusRamInfo* ramInfo = vebusGetRamInfoOfDevice(dev);

	return ramInfo && id < VEBUS_RAMVAR_COUNT && ramInfo[id].scale != 0;
}

veBool vebusSupportedSystemVar9Bit(struct VebusDeviceS* dev, un16 nr)
{
	VE_UNUSED(dev);

	if (vebusGetAcIn(nr) != 0 || vebusGetPhase(nr))
		return veFalse;

	return veTrue;
}

veBool vebusSupportedSystemVar(struct VebusDeviceS* dev, un8 id)
{
	VebusValue nr;

	if (id >= VEBUS_SYSTEM_COUNT)
		return veFalse;
	nr = VEBUS_SYS_OFFSET + id;

	if (!mk2HasAcIn(dev, vebusGetAcIn(nr)))
		return veFalse;

	if (vebusGetPhase(nr) > vebusSystem.ac[0].phaseCount)
		return veFalse;

	return id < VEBUS_SYSTEM_COUNT;
}

/**
 * Converts a raw vebus value to a normal unit.
 * mul determines the number of decimals, e.g. 1000 for milli.
 */
sn32 vebusUnpackRam(struct VebusDeviceS* dev, sn32 raw, un8 id, un16 mul)
{
	VebusRamInfo *ramInfo = vebusGetRamInfoOfDevice(dev);
	VebusRamInfo *info = &ramInfo[id];
	sn16 scale;
	sn32 value;

	if (info->scale == 0)
		return 0;

	/* bit field support */
	if (info->offset == (sn16) 0x8000)
		return (raw & (1 << (info->scale - 1)) ? 1 : 0);

	/* this will make an normal unit of it.. */
	if (info->scale < 0) {
		raw = (sn16) raw;
		scale = -info->scale;
	} else
		scale = info->scale;

	value = raw + info->offset;

	if (scale >= 0x4000)
	{
		scale = 0x8000 - scale;
		value *= mul;
		value /= scale;
	} else {
		value *= scale;
	}

	return value;
}

/** store the setting info / config frame. */
static Mk2EventReply configHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	VebusDevice *dev;
	un8 type;

	if (ev != MK2_FRAME || msg->length < 15 || msg->buf[0] != 0x20 /* VEBUS_RSP_INFO */)
		return MK2_CONTINUE;

	type = msg->buf[5];
	/* type should be either 2,3,4 or between 0x20 and 0x2F */
	if (type < 2)
		return MK2_CONTINUE;
	if ((type > 4) && (type < 0x20))
		return MK2_CONTINUE;
	if (type > 0x2F)
		return MK2_CONTINUE;

	logI("vebus_system", "++++ got vebus setting info ++++");

	dev = (VebusDevice *) mk2.fh.msgOut->ctx;
	dev->uniqueNumber = le_u32_to_cpu_p(&msg->buf[1]);
	dev->type = type;

#if CFG_MK2_VSC_SUPPORT
	memcpy(dev->config.raw, &msg->buf[6], sizeof(VebusConfig));
#endif

	return MK2_DONE;
}

/* The message to request a config from a device. */
Mk2MsgTx *vebusGetConfigMsg(un8 addr)
{
	Mk2MsgTx *msg = mk2Msg('F');
	veAssert(msg);
	if (!msg)
		return NULL;

	msg->buf[2] = 0x07; /* GET_SETTING_INFO */;
	msg->buf[3] = addr;
	msg->length = 4;

	return msg;
}

/**
 * Get the vebus setting info / configuration frame. Since the name
 * settings is already taken, config* is used to refer to this data.
 */
void vebusGetConfig(VebusDevice *dev, Mk2Callback *callback)
{
	Mk2MsgTx* msg = vebusGetConfigMsg(dev->addr);
	veAssert(msg);
	if (!msg)
		return;

	logI("vebus_system", "---- get vebus setting info ----");
	msg->ctx = dev;
	msg->callback = callback;
	msg->handler = configHandler;

	mk2FhQueueFrame(msg);
}

static Mk2EventReply resetDeviceHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	VE_UNUSED(msg);

	if (ev == MK2_TX_SENT)
		return MK2_DONE;

	return MK2_CONTINUE;
}

/**
 * Create a message to reset VE.Bus device.
 *
 * @param addr - The address of the VE.Bus device.
 *				0 will reset all devices.
 */
static Mk2MsgTx* resetDeviceMsg(un32 addr)
{
	/* ‘F’ , 8,  ID2, ID3, ID0, ID1 */
	Mk2MsgTx* msg = mk2Msg('F');
	veAssert(msg);
	if (!msg)
		return NULL;
	msg->buf[2] = 8; /* RESET_VEBUS_DEVICE */;
	msg->buf[3] = (un8)(addr >> 16); 	/* ID2 */
	msg->buf[4] = (un8)(addr >> 24);	/* ID3 */
	msg->buf[5] = (un8)(addr); 			/* ID0 */
	msg->buf[6] = (un8)(addr >> 8);		/* ID1 */
	msg->length = 7;
	msg->handler = resetDeviceHandler;

	return msg;
}

/**
 * Reset the vebus device by long ID. If addr == 0 reset all devices
 * There is no acknowledgment
 */
void vebusResetDevice(VebusDevice *dev, Mk2Callback *callback)
{
	Mk2MsgTx* msg = resetDeviceMsg(dev->uniqueNumber);
	veAssert(msg);
	if (!msg)
		return;

	logI("vebus_system", "---- reset vebus device %X----", dev->uniqueNumber);
	msg->callback = callback;
	mk2FhQueueFrame(msg);
}

void vebusResetDevices(Mk2Callback* callback)
{
	Mk2MsgTx* msg = resetDeviceMsg(0);
	veAssert(msg);
	if (!msg)
		return;

	logI("vebus_system", "---- reset all vebus devices----");
	msg->callback = callback;
	mk2FhQueueFrame(msg);
}

static Mk2EventReply vebusSendAccessPasswordHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	struct VebusDeviceS *dev = (struct VebusDeviceS *) mk2.fh.msgOut->ctx;

	if (ev != MK2_FRAME)
		return MK2_CONTINUE;

	if (mk2IsXYZNoData(msg, WM_RSP_ACCESS_PASSWORD_ACCEPTED)) {
		dev->accessLevelRequired = 0; /* Correct password */
		return MK2_DONE;
	}

	if (mk2IsXYZNoData(msg, WM_RSP_UNKNOWN_COMMAND) && (msg->buf[3] == 0x01)) {
		/* Wrong password */
		if (dev->accessLevelRequired == 0)
			/* If accessLevel was not previously set, set all bits to indicate wrong password. */
			dev->accessLevelRequired = 0xFF;
		return MK2_DONE;
	}

	return MK2_CONTINUE;
}

/* Access passwords are valid for 30 seconds. */
void vebusSendAccessPassword(struct VebusDeviceS *dev, un8 *passwd, un8 len, Mk2Callback *callback)
{
	/* when successfull clear dev->accessLevelRequired */
	Mk2MsgTx *msg = mk2MsgXYZ(dev->addr, WM_CMD_ACCESS_PASSWORD);
	if (!msg)
		return;

	if (len != 8) {
		if (callback)
			callback();
		return;
	}

	memcpy(&msg->buf[3], passwd, len);
	msg->length += len;
	msg->ctx = dev;
	msg->callback = callback;
	msg->handler = vebusSendAccessPasswordHandler;
	mk2FhQueueFrame(msg);
}


static Mk2EventReply vebusGetExtraFlagAccessInfoHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	un8 j;
	VebusExtraFlagAccessInfo *info;

	if (ev != MK2_FRAME)
		return MK2_CONTINUE;

	if (mk2IsXYZNoData(msg, WM_RSP_UNKNOWN_COMMAND))
		return MK2_DONE;

	if (!mk2IsXYZ(msg, (un8) mk2.fh.msgOut->rsp))
		return MK2_CONTINUE;

	info = (VebusExtraFlagAccessInfo *) mk2.fh.msgOut->ctx;

	if (msg->length >= 12) {
		if (msg->buf[3] != info->flagWordId)
			return MK2_CONTINUE;

		if (msg->buf[2] == WM_RSP_EXTRA_FLAG_ACCESS_INFO_LO)
			j = 0;
		else
			j = 8;

		memcpy(&info->accessLevel[j], &msg->buf[4], 8);

		if (j == 0) {
			logI("winmon", "extra flags access info low received for flagWordId %d",info->flagWordId);
			mk2.fh.msgOut->rsp = WM_RSP_EXTRA_FLAG_ACCESS_INFO_HI; /* Get next */
			return MK2_CONTINUE;
		}

		info->valid = veTrue;
		logI("winmon", "extra flags access info high received for flagWordId %d",info->flagWordId);
		return MK2_DONE;
	}

	return MK2_CONTINUE;
}

/*
 * Request for extra flag access information belonging to settingID id.
 * Obtained values are stored in ctx and cb is invoked afterwards.
 */
void vebusGetExtraFlagAccessInfo(struct VebusDeviceS *dev, un8 id, Mk2Callback *cb, VebusExtraFlagAccessInfo *ctx)
{
	Mk2MsgTx *msg;

	ctx->valid = veFalse;
	ctx->flagWordId = id;

	msg = mk2MsgXYZInfo(dev->addr, WM_CMD_GET_EXTRA_FLAG_ACCESS_INFO, id);
	if (!msg)
		return;

	logI("vebus_system", "---- get vebus extra flag access info for setting id = %d ----", id);

	msg->handler = vebusGetExtraFlagAccessInfoHandler;
	msg->callback = cb;
	msg->ctx = ctx;
	msg->id = id;
	msg->rsp = WM_RSP_EXTRA_FLAG_ACCESS_INFO_LO;
	mk2FhQueueFrame(msg);
}

#define VEBUS_SERIAL_NUMBER_LEN		11

static Mk2EventReply vebusGetSerialNumberHandler(Mk2Event ev, Mk2MsgRx *msg)
{
	un8 *serialNumber;

	if (ev != MK2_FRAME)
		return MK2_CONTINUE;

	serialNumber = (un8 *) mk2.fh.msgOut->ctx;
	if (mk2IsXYZNoData(msg, WM_RSP_UNKNOWN_COMMAND)) {
		/* Multi frimware < 308 does not support getSerialNumber */
		memset(serialNumber, 0, VEBUS_SERIAL_NUMBER_LEN);
		return MK2_DONE;
	}

	if (!mk2IsXYZ(msg, (un8) mk2.fh.msgOut->rsp) || msg->length != 14)
		return MK2_CONTINUE;

	memcpy(serialNumber, &msg->buf[3], VEBUS_SERIAL_NUMBER_LEN);

	return MK2_DONE;
}

/* serialNumber should be a buffer of at least VEBUS_SERIAL_NUMBER_LEN (11) bytes long */
void vebusGetSerialNumber(struct VebusDeviceS *dev, Mk2Callback *cb, un8 *serialNumber)
{
	Mk2MsgTx *msg = mk2MsgXYZ(dev->addr, WM_CMD_GET_SERIAL_NUMBER);
	if (!msg)
		return;

	logI("vebus_system", "---- get vebus serial number ----");
	msg->handler = vebusGetSerialNumberHandler;
	msg->callback = cb;
	msg->ctx = serialNumber;
	msg->rsp = WM_RSP_SERIAL_NUMBER;
	mk2FhQueueFrame(msg);
}
