#ifndef _MK2_H_
#define _MK2_H_

#include <velib/mk2/frame_handler.h>

#define MK2_COMMAND_IDENTIFIER	0xFF

#define MK2_S_FLAG_AUTO_SEND_STATE				0x01
#define MK2_S_FLAG_LED_STATUS					0x02
#define MK2_S_FLAG_RESET_ON_BREAK				0x04
#define MK2_S_FLAG_RESERVED_0_KEEP_ZERO			0x08
/* Do not forward the state to the vebus side, not even once. */
#define MK2_S_FLAG_DO_NOT_SEND_STATE			0x10
#define MK2_S_FLAG_RESERVED_1_KEEP_ZERO			0x20
#define MK2_S_FLAG_AUTO_FORWARD_PANEL_FRAMES	0x40
#define MK2_S_FLAG_NEW_FORMAT					0x80

/*
 * Newer vebus device have some backwards incompatible changes:
 *  - CommandSendSoftwareVersionPart0 and CommandSendSoftwareVersionPart1 are
 *    combined in a single respone and some additional data is added.
 *  - Write ramvars and settings is combined in a single command also including
 *    the data in the same frame. This is a seperate opcode. Without the backwards
 *    enable flag set, the usual set ramvar / set setting will no longer work.
 *  - format of scale response is fitted in a single frame.
 *  - Winmon frames can be shorter (not quaranteed to contain data).
 * The MK2_S_FLAG_E0_COMPAT_SPLIT_FRAMES provides backwards compatibility for these changes.
 */
#define MK2_S_FLAG_E0_COMPAT_SPLIT_FRAMES		0x01
#define MK2_S_FLAG_E0_CONTINUES_ID_TX			0x02
#define MK2_S_FLAG_E0_FORWARD_ALL_CONFIG_RESP	0x04
#define MK2_S_FLAG_E0_BLOCK_MODE				0x08

Mk2MsgTx *mk2Msg(un8 command);
Mk2MsgTx *mk2MsgXYZ(un8 addr, un8 wcmd);
Mk2MsgTx *mk2MsgXYZInfo(un8 addr, un8 wcmd, un16 info);

typedef enum {
	MK2_OK,
	MK2_UPDATE_FAILED
} Mk2Result;

typedef struct {
	un8 leds;
	un8 ledsBlink;
	un8 alarms;
	un8 vebusError;
	un32 shortIds;
	un32 knownIds;
	un8 flags;
} VebusState;

/* High bits of vebusError, but stored in flags */
#define VEBUS_STATE_FLAG_MK2_BLOCKED		0x80

typedef struct {
	/*
	 * This is either the state as received or the state being send.
	 * It should not be updated during changing roles..
	 */
	un8 mode;
	union {
		struct {
			un8 potMeter;
			un8 panelScale;
		} old;
		struct {
			un16 value;
		} current;
	} limit;
	un8 xids;
	un8 dFlags;
	un8 sFlags;
	un8 sFlagsExt0;
} Mk2State;

typedef void Mk2ResultCallback(Mk2Result result);

void mk2Init(void);
void mk2SetAddress(un8 address, Mk2Callback* callback);
void mk2TalkToAddress(un8 address);
void mk2SetState(Mk2State* state, Mk2Callback* callback);
void mk2GetSettingInfo(un16 settingID, Mk2Callback* callback);
void mk2GetState(Mk2State* state, Mk2Callback* callback);
void mk2GetVebusState(VebusState* info, Mk2Callback* callback);
void mk2GetVersion(un32* version, Mk2Callback* callback);
void mk2SoftReset(Mk2Callback* callback);

veBool mk2IsXYZ(Mk2MsgRx* frm, un8 rsp);
veBool mk2IsXYZMinLength(Mk2MsgRx* frm, un8 rsp, un8 minLength);
veBool mk2IsXYZNoData(Mk2MsgRx* frm, un8 rsp);

/*
 * See mk2_init.c check for mk2 and vebus devices. When a new device
 * is found mk2NewDeviceFound will be called. Note that only the short
 * address is valid at that time. The passed callback cb, will be called
 * when all devices are found.
 */
void mk2Start(Mk2Callback* cb);

/* log to console */
void mk2Dump(char* str, Mk2MsgRx *frame);
void mk2DumpBuf(char* str, un8 const* buf, int len);

#endif
