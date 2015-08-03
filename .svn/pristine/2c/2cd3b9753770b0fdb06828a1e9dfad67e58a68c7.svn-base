#ifndef _VELIB_VECAN_REGS_H_
#define _VELIB_VECAN_REGS_H_

#include <velib/types/types.h>

/// @addtogroup VELIB_VECAN_REG
/// @{

/// Generic OK.
#define VACK_OK						0x0000
/// Used by UDF to indicate it is ready.
#define VACK_BEGIN					0x0100
/// Used by UDF to poll for more data.
#define VACK_CONT					0x0200

/// Error flag.
#define VACK_ERR					0x8000
/// Erroneous request, register unknown.
#define VACK_ERR_REQ				0x8000
/// Erroneous command, register unknown.
#define VACK_ERR_CMD				0x8100

/// Temporary failure.
#define VACK_ERR_BUSY				0x8200
/// Fatal error, invalid format etc.
#define VACK_ERR_INVALID			0x8300
/// Remote timeout.
#define VACK_ERR_TIMEOUT			0x8400
/// Too many invalid requests etc.
#define VACK_ERR_OVERFLOW			0x8500
/// Temporary failure, not completely initialiazed yet.
#define VACK_ERR_INITIALIZING		0x8600

/// Specific for VE_REG_UPDATE_DATA, can't boot (no valid application)
#define VACK_ERR_UPD_INVALID		0xC000

/// Specific for VE_REG_UPDATE_DATA
#define VACK_ERR_ERASING			0xC100

/// Specific for VE_REG_UPDATE_DATA
#define VACK_ERR_WRITE				0xC200

/// Specific for VE_REG_CONTROL, multi's can only controlled by 1 panel.
#define VACK_ERR_PANEL_PRESENT		0xC001

/// Internal, local timeout; not response reveived in time.
#define VACK_ERR_LOCAL_TIMEOUT		0xFFFE
/// Internal
#define VACK_INVALID				0xFFFF
/// Internal as well, used to not automatically send the value after a command
#define VACK_DO_NOT_SEND_VREG		0xFFFF

/// Register id is un16
typedef un16 VeRegAckCode;

/// All errors are above 0x8000.
#define VACK_IS_ERROR(ack)			(ack&0x8000)

/**
 * Victron registers. This enum contains the common vregs and should
 * (in general) have low numbers. 0xEDXX is an exception.
 *
 * This file is leading. More details on VREGs are documented in the files:
 * - VE.Can registers - private.docx
 * - VE.Can registers - public.docx
 * - Several VE.Direct HEX documentation files
 */
typedef enum
{
	// special commands

	/// only internal!
	VE_REG_INVALID							= 0x0000,
	/// Request a register, VREQ.
	VE_REG_REQ								= 0x0001,
	/// Acknowlegde request, VACK.
	VE_REG_ACK								= 0x0002,
	/// PING
	VE_REG_PING								= 0x0003,
	/// Restore default (no data)
	VE_REG_RESTORE_DEFAULT					= 0x0004,

	/// Request an update (un8, vup instance)
	VE_REG_UPDATE_ENABLE					= 0x0010,
	/// Resume normal operation after update.
	VE_REG_UPDATE_END						= 0x0011,
	/// Send data to UDF. (fast-packet)
	VE_REG_UPDATE_DATA						= 0x0012,

	/// Request test mode (string4, write-only)
	/// @remark Send 'VTST' as payload to activate the test mode
	VE_REG_TEST_MODE						= 0x0020,

	/// Used for serial like communication, e.g. with a on board chip or to emulate a console.
	/// See io/ve_tunnel.c for the common payload. There might be protocol specific payload
	/// in the data. The CAN bus identifies instance of busses with a byte, while the dbus uses
	/// a path to identify them. The carried payload should be of identical format for all busses
	/// though.
	VE_REG_TUNNEL							= 0x0030,

	/// Unique ID of the chip field 0 (un16 = controller id, un16 = site id, read-only)
	VE_REG_UNIQUE_ID_0						= 0x0040,
	/// Unique ID of the chip field 1 (un32 = serial number part 1, read-only)
	VE_REG_UNIQUE_ID_1						= 0x0041,
	/// Unique ID of the chip field 2 (un32 = serial number part 2, read-only)
	VE_REG_UNIQUE_ID_2						= 0x0042,
	/// Unique ID of the chip field 3 (un32 = serial number part 3, read-only)
	VE_REG_UNIQUE_ID_3						= 0x0043,
	/// Unique ID of the chip field 4 (un32 = serial number part 4, read-only)
	VE_REG_UNIQUE_ID_4						= 0x0044,

	/// Random number field 0, used for programming (un32, read-only)
	/// @remark reading field 0 updates the random value, make sure to read the fields in the proper sequence
	VE_REG_RANDOM_0							= 0x0050,
	/// Random number field 1, used for programming (un32, read-only)
	VE_REG_RANDOM_1							= 0x0051,
	/// Random number field 2, used for programming (un32, read-only)
	VE_REG_RANDOM_2							= 0x0052,
	/// Random number field 3, used for programming (un32, read-only)
	VE_REG_RANDOM_3							= 0x0053,
	/// Random number field 4, used for programming (un32, read-only)
	VE_REG_RANDOM_4							= 0x0054,
	/// Random number field 5, used for programming (un32, read-only)
	VE_REG_RANDOM_5							= 0x0055,
	/// Random number field 6, used for programming (un32, read-only)
	VE_REG_RANDOM_6							= 0x0056,
	/// Random number field 7, used for programming (un32, read-only)
	VE_REG_RANDOM_7							= 0x0057,
	/// Random number field 8, used for programming (un32, read-only)
	VE_REG_RANDOM_8							= 0x0058,
	/// Random number field 9, used for programming (un32, read-only)
	VE_REG_RANDOM_9							= 0x0059,

	// Basic data
	/// Id of (part of a) product (un8 = identifier, un16 = product id, un8 = flags, read-only).
	/// @remark Identifier: 0=local product
	/// @remark Note that the ve.direct mppt chargers report this field incorrectly as an un16 with the product id in big-endian notation
	/// @remark See products.h for a list of product id's
	/// @remark The reserved flags should be set to '1' if not used (0xFE)
	VE_REG_PRODUCT_ID						= 0x0100,

	/// Hardware revision (un8 = identifier, un16 = hardware revision)
	/// Used for identifying hardware changes / incompatibilities. (update related)
	VE_REG_PRODUCT_REVISION					= 0x0101,

	/// Current firmware version (un8 = identifier, un24 = firmware version)
	/// @remark Identifier: see VE_REG_PRODUCT_ID
	/// @remark Firmware version: 0x123456=v12.34.56, 0xFFFFFF=no firmware present
	VE_REG_APP_VER							= 0x0102,
	/// Minimum possible firmware version (max downgrade) (un8 = identifier, un24 = firmware version)
	/// @remark Identifier: see VE_REG_PRODUCT_ID
	/// @remark Firmware version: see VE_REG_APP_VER
	VE_REG_APP_VER_MIN						= 0x0103,

	/**
	 * Group id. Can be used to group similar devices, eliminating the
	 * need for keeping track of various product ids (useful for
	 * parallel charging operation). (un8, read-only). The possible values
	 * are defined in products.h.
	 * @remark Also used for the BLE dongle
	 */
	VE_REG_GROUP_ID							= 0x0104,

	/// Serial number (read-only string32)
	/// @remark HEX protocol only (data present in regular NMEA pgn)
	VE_REG_SERIAL_NUMBER					= 0x010A,
	/// Model name (read-only string32)
	/// @remark HEX protocol only (data present in regular NMEA pgn)
	VE_REG_MODEL_NAME						= 0x010B,
	/// Installation description field 1 (string)
	/// @remark HEX protocol only (data present in regular NMEA pgn)
	VE_REG_DESCRIPTION1						= 0x010C,
	/// Installation description field 2 (string)
	/// @remark HEX protocol only (data present in regular NMEA pgn)
	VE_REG_DESCRIPTION2						= 0x010D,
	/// Identify mode (un8)
	/// Write a 1 to this register to enable identify mode.
	/// Write a 0 to this register to disable identify mode and return to normal operation.
	VE_REG_INDENTIFY						= 0x010E,

	/// Bootloader / Update Device version (un24=udf version, un8=flags, read-only)
	/// @remark udf version: see VE_REG_APP_VER
	/// @remark flags: (bit mask) 0x01=udf active, 0x02..0x80:=reserved
	VE_REG_UDF_VERSION						= 0x0110,

	/// uptime since boot in seconds (un32, read-only)
	VE_REG_UPTIME							= 0x0120,

	// Can hardware reception overflow event counter (un32, read-only)
	VE_REG_RX_HW_OVERFLOW_COUNT				= 0x0130,
	// Can software reception overflow event counter (un32, read-only)
	VE_REG_RX_SW_OVERFLOW_COUNT				= 0x0131,
	// Can error passive event counter (un32, read-only)
	VE_REG_ERROR_PASSIVE_COUNT				= 0x0132,
	// Can buf off event counter (un32, read-only)
	VE_REG_BUSOFF_COUNT						= 0x0133,

	/// Device identification register 1 - capabilities bit mask (un32 = capabilities, read-only)
	/// @remark see VE_REG_CAPABILITIES1 section for possible values
	VE_REG_CAPABILITIES1					= 0x0140,
	/// Device identification register 2 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES2					= 0x0141,
	/// Device identification register 3 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES3					= 0x0142,
	/// Device identification register 4 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES4					= 0x0143,
	/// Device identification register 5 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES5					= 0x0144,
	/// Device identification register 6 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES6					= 0x0145,
	/// Device identification register 7 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES7					= 0x0146,
	/// Device identification register 8 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES8					= 0x0147,
	/// Device identification register 9 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES9					= 0x0148,
	/// Device identification register 10 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES10					= 0x0149,
	/// Device identification register 11 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES11					= 0x014A,
	/// Device identification register 12 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES12					= 0x014B,
	/// Device identification register 13 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES13					= 0x014C,
	/// Device identification register 14 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES14					= 0x014D,
	/// Device identification register 15 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES15					= 0x014E,
	/// Device identification register 16 - capabilities bit mask (un32 = capabilities, read-only)
	VE_REG_CAPABILITIES16					= 0x014F,	

	/// Device mode (un8 = mode)
	/// @remark see VE_REG_DEVICE_MODE section for possible values
	/// @remark error code 0x8300: Invalid, value is out of range / not applicable for the device / system
	/// @remark error code 0xC001: VE.Bus specific error, the system cannot be controlled since another panel controls the mode. This will happen when a Digital Multi Control is connected
	VE_REG_DEVICE_MODE						= 0x0200,
	/// Device state (un8 = state, read-only)
	/// @remark state: 0=off, 1=low power mode, 2=fault, 3=bulk, 4=absorption,
	///         5=float, 6=storage, 7=equalize, 8=passthru, 9=inverting,
	///         10=assisting, 11=psu mode, 0xFC=hub1, 255=not available
	/// @remark state 11 psu mode is NOT in the NMEA2000 specification. It is a
	///         victron specific value. New states should be added from the top
	///         instead of from the bottom.
	/// @remark NMEA2000 equivalent: DD342 Converter Operating State as found
	///         in PGN 127750 (v2.000)
	/// @remark VE.Text equivalent: CS
	VE_REG_DEVICE_STATE						= 0x0201,
	/**
	 * Some features require activation. E.g. the control of AC input currents
	 * should only be used when there is a panel present which displays the
	 * actual current limit, so it is always clear why power is limited and not
	 * a defect. These flags are OR-ed with current enabled features and for
	 * compatibilty also accept flags not known to the device. (un32 = flags)
	 * see VE_REG_REMOTE_CONTROL_USED section for possible values
	 */
	VE_REG_REMOTE_CONTROL_USED				= 0x0202,

	/// active AC_IN actual setpoint (un16 = limit [100mA], read-only)
	/// @remark limit: 0xFFFF=not available
	VE_REG_AC_IN_CURRENT_LIMIT				= 0x0203,
	/// active AC input, for example for a quattro (un8, read-only)
	VE_REG_AC_IN_ACTIVE						= 0x0204,

	/// AC_IN1 actual setpoint (un16 = limit [100mA], read-only)
	/// @remark limit: 0xFFFF=not available
	VE_REG_AC_IN_1_CURRENT_LIMIT			= 0x0210,
	/// AC_IN1 range, minimum accepted value (un16 = limit [100mA], read-only)
	/// @remark limit: 0xFFFF=not available
	VE_REG_AC_IN_1_CURRENT_LIMIT_MIN		= 0x0211,
	/// AC_IN1 range, maximum accepted value (un16 = limit [100mA], read-only)
	/// @remark limit: 0xFFFF=not available
	VE_REG_AC_IN_1_CURRENT_LIMIT_MAX		= 0x0212,
	/**
	 * The internal current limit for ACIN1, used after reset if no command is received to use the
	 * remote value.. This is to ensure a device without panels connected behaves the same
	 * as initially shipped after a power cycle (un16 = limit [100mA])
	 * @remark limit: 0xFFFF=not available
	 */
	VE_REG_AC_IN_1_CURRENT_LIMIT_INTERNAL	= 0x0213,
	/// The current limit for ACIN1 received from remote parties. (un16 = limit [100mA])
	/// @remark limit: 0xFFFF=not available, 0=use minimum current
	/// The remote limit is not used unless enabled @see VE_REG_REMOTE_CONTROL_USED
	VE_REG_AC_IN_1_CURRENT_LIMIT_REMOTE		= 0x0214,

	/// AC_IN2 actual setpoint (un16 = limit [100mA], read-only)
	/// @remark limit: 0xFFFF=not available
	VE_REG_AC_IN_2_CURRENT_LIMIT			= 0x0220,
	/// AC_IN2 range, minimum accepted value (un16 = limit [100mA], read-only)
	/// @remark limit: 0xFFFF=not available
	VE_REG_AC_IN_2_CURRENT_LIMIT_MIN		= 0x0221,
	/// AC_IN2 range, maximum accepted value (un16 = limit [100mA], read-only)
	/// @remark limit: 0xFFFF=not available
	VE_REG_AC_IN_2_CURRENT_LIMIT_MAX		= 0x0222,
	/**
	 * The internal current limit for ACIN2, used after reset if no command is received to use the
	 * remote value.. This is to ensure a device without panels connected behaves the same
	 * as initially shipped after a power cycle (un16 = limit [100mA])
	 * @remark limit: 0xFFFF=not available
	 */
	VE_REG_AC_IN_2_CURRENT_LIMIT_INTERNAL	= 0x0223,
	/// The current limit for ACIN2 received from remote parties. (un16 = limit [100mA])
	/// @remark limit: 0xFFFF=not available, 0=use minimum current
	/// The remote limit is not used unless enabled @see VE_REG_REMOTE_CONTROL_USED
	VE_REG_AC_IN_2_CURRENT_LIMIT_REMOTE		= 0x0224,

	// History
	/// The depth of the deepest discharge (BMV H1) [SN32, 0.1 Ah].
	VE_REG_HIST_DEEPEST_DISCHARGE			= 0x0300,
	/// The depth of the last discharge (BMV H2) [SN32, 0.1 Ah].
	VE_REG_HIST_LAST_DISCHARGE				= 0x0301,
	/// The depth of the average discharge (BMV H3) [SN32, 0.1 Ah].
	VE_REG_HIST_AVERAGE_DISCHARGE			= 0x0302,
	/// The number of charge cycles (BMV H4) [SN32].
	VE_REG_HIST_NR_OF_CHARGE_CYCLES			= 0x0303,
	/// The number of full discharges (BMV H5) [SN32].
	VE_REG_HIST_NR_OF_FULL_DISCHARGES		= 0x0304,
	/// The cumulative number of Amp hours drawn from the battery (BMV H6) [SN32, 0.1 Ah].
	VE_REG_HIST_CUMULATIVE_AH				= 0x0305,
	/// The minimum battery voltage (BMV H7) [SN32, 0.01 V].
	VE_REG_HIST_MIN_VOLTAGE					= 0x0306,
	/// The maximum battery voltage (BMV H8) [SN32, 0.01 V].
	VE_REG_HIST_MAX_VOLTAGE					= 0x0307,
	/// The number of second since the last full charge (BMV H9) [SN32].
	VE_REG_HIST_SECS_SINCE_LAST_FULL_CHARGE	= 0x0308,
	/// The number of times the monitor has automatically synchronised (BMV H10) [SN32].
	VE_REG_HIST_NR_OF_AUTO_SYNCS			= 0x0309,
	/// The number of low voltage alarms (BMV H11) [SN32].
	VE_REG_HIST_NR_OF_LOW_VOLTAGE_ALARMS	= 0x030A,
	/// The number of high voltage alarms (BMV H12) [SN32].
	VE_REG_HIST_NR_OF_HIGH_VOLTAGE_ALARMS	= 0x030B,
	/// The number of low auxilary voltage alarms [SN32].
	VE_REG_HIST_NR_OF_LOW_VOLTAGE_2_ALARMS	= 0x030C,
	/// The number of high auxilary voltage alarms. [SN32].
	VE_REG_HIST_NR_OF_HIGH_VOLTAGE_2_ALARMS	= 0x030D,
	/// The minimum auxilary voltage [SN32, 0.01V].
	VE_REG_HIST_MIN_VOLTAGE_2				= 0x030E,
	/// The maximum auxilary voltage [SN32, 0.01V].
	VE_REG_HIST_MAX_VOLTAGE_2				= 0x030F,
	/// The amount of energy drawn from the source [UN32, 0.01 kWh].
	VE_REG_HIST_KWH_OUT						= 0x0310,
	/// The amount of energy put into the source [UN32, 0.01 kWh].
	VE_REG_HIST_KWH_IN						= 0x0311,

	/// Alarm status (un8 0: active; 1: inactive)
	/// @remark VE.Text equivalent: ALARM (ON/OFF)
	VE_REG_ALARM_STATUS						= 0x031D,
	/// Alarm reason, multiple flag can be or-ed together (un16)
	/// @remark VE.Text equivalent: AR
	VE_REG_ALARM_REASON						= 0x031E,
	/// Acknowlegde alarm command (no data)
	VE_REG_ALARM_ACK						= 0x31F,
	/// Low voltage lower threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_ALARM_LOW_VOLTAGE_SET			= 0x0320,
	/// Low voltage upper threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_ALARM_LOW_VOLTAGE_CLEAR			= 0x0321,
	/// High voltage upper threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_ALARM_HIGH_VOLTAGE_SET			= 0x0322,
	/// High voltage lower threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_ALARM_HIGH_VOLTAGE_CLEAR			= 0x0323,
	/// Low auxilary voltage lower threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_ALARM_LOW_VOLTAGE_2_SET			= 0x0324,
	/// Low auxilary voltage upper threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_ALARM_LOW_VOLTAGE_2_CLEAR		= 0x0325,
	/// High auxilary voltage upper threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_ALARM_HIGH_VOLTAGE_2_SET			= 0x0326,
	/// High auxilary voltage lower threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_ALARM_HIGH_VOLTAGE_2_CLEAR		= 0x0327,
	/// Low SOC lower threshold (un16 0.1%)
	VE_REG_ALARM_LOW_SOC_SET				= 0x0328,
	/// Low SOC upper threshold (un16 0.1%)
	VE_REG_ALARM_LOW_SOC_CLEAR				= 0x0329,
	/// Low battery temperature lower threshold (un16 0.01K)
	VE_REG_ALARM_LOW_BAT_TEMP_SET			= 0x032A,
	/// Low battery temperature upper threshold (un16 0.01K)
	VE_REG_ALARM_LOW_BAT_TEMP_CLEAR			= 0x032B,
	/// High battery temperature upper threshold (un16 0.01K)
	VE_REG_ALARM_HIGH_BAT_TEMP_SET			= 0x032C,
	/// High battery temperature lower threshold (un16 0.01K)
	VE_REG_ALARM_HIGH_BAT_TEMP_CLEAR		= 0x032D,
	/// High internal temperature alarm upper threshold (un16 0.01K)
	VE_REG_ALARM_HIGH_INT_TEMP_SET			= 0x032E,
	/// High internal temperature alarm lower threshold (un16 0.01K)
	VE_REG_ALARM_HIGH_INT_TEMP_CLEAR		= 0x032F,
	/// Fuse blown alarm enable (un8 0=enabled, 1=disabled)
	VE_REG_ALARM_FUSE_BLOWN					= 0x0330,
	/// Mid voltage alarm set. (un16 0.1%)
	VE_REG_ALARM_MID_VOLTAGE_SET			= 0x0331,
	/// Mid voltage alarm clear. (un16 0.1%)
	VE_REG_ALARM_MID_VOLTAGE_CLEAR			= 0x0332,

	/** Relay invert. (un8 0=not-invert, 1=invert) */
	VE_REG_RELAY_INVERT						= 0x034D,
	/**
	 * Relay control [UN8]. Reads active relay value.
	 * Writes the remote value. The value is stored in memory,
	 * but only applied when the remote relay mode is selected.
	 * @see VE_REG_RELAY_MODE.
	 */
	VE_REG_RELAY_CONTROL					= 0x034E,
	/**
	 * Relay mode [UN8]. Controls the function of the relay.
	 * 0=alarm, 1=charger mode, 2=remote control, 3=always open
	 */
	VE_REG_RELAY_MODE						= 0x034F,

	/// Relay thresholds
	/// Low voltage lower threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_RELAY_LOW_VOLTAGE_SET			= 0x0350,
	/// Low voltage upper threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_RELAY_LOW_VOLTAGE_CLEAR			= 0x0351,
	/// High voltage upper threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_RELAY_HIGH_VOLTAGE_SET			= 0x0352,
	/// High voltage lower threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_RELAY_HIGH_VOLTAGE_CLEAR			= 0x0353,
	/// Low auxilary voltage lower threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_RELAY_LOW_VOLTAGE_2_SET			= 0x0354,
	/// Low auxilary voltage upper threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_RELAY_LOW_VOLTAGE_2_CLEAR		= 0x0355,
	/// High auxilary voltage upper threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_RELAY_HIGH_VOLTAGE_2_SET			= 0x0356,
	/// High auxilary voltage lower threshold (un16, 0.01V)
	/// @remark BMV-70x is an exception, unit is 0.1V
	VE_REG_RELAY_HIGH_VOLTAGE_2_CLEAR		= 0x0357,
	/// Low SOC lower threshold (un16 0.1%)
	VE_REG_RELAY_LOW_SOC_SET				= 0x0358,
	/// Low SOC upper threshold (un16 0.1%)
	VE_REG_RELAY_LOW_SOC_CLEAR				= 0x0359,
	/// Low battery temperature lower threshold  (un16 0.01K)
	VE_REG_RELAY_LOW_BAT_TEMP_SET			= 0x035A,
	/// Low battery temperature upper threshold  (un16 0.01K)
	VE_REG_RELAY_LOW_BAT_TEMP_CLEAR			= 0x035B,
	/// High battery temperature upper threshold  (un16 0.01K)
	VE_REG_RELAY_HIGH_BAT_TEMP_SET			= 0x035C,
	/// High battery temperature lower threshold  (un16 0.01K)
	VE_REG_RELAY_HIGH_BAT_TEMP_CLEAR		= 0x035D,
	/// High internal temperature relay upper threshold (un16 0.01K)
	VE_REG_RELAY_HIGH_INT_TEMP_SET			= 0x035E,
	/// High internal temperature relay lower threshold (un16 0.01K)
	VE_REG_RELAY_HIGH_INT_TEMP_CLEAR		= 0x035F,
	/// Enable closing the relay when the fuse is blown (un8 0=enabled, 1=disabled)
	VE_REG_RELAY_FUSE_BLOWN					= 0x0360,
	/// Mid voltage relay set (un16 0.1%)
	VE_REG_RELAY_MID_VOLTAGE_SET			= 0x0361,
	/// Mid voltage relay clear (un16 0.1%)
	VE_REG_RELAY_MID_VOLTAGE_CLEAR			= 0x0362,

	/// These should be high numbers actually...
	VE_REG_STOLEN_BY_LYNX_ION_1				= 0x0370,
	VE_REG_STOLEN_BY_LYNX_ION_2	 			= 0x0371,
	VE_REG_STOLEN_BY_LYNX_ION_3				= 0x0372,

	/// Number of batteries and number of cells per battery (un8 nrOfBatteries, un8 cellsPerBattery)
	VE_REG_BATTERY_CONFIGURATION			= 0x0380,
	/// Voltages of cell n of battery m [n (un8)][m (un8)][voltage in 0.01V (un16)]
	VE_REG_BATTERY_CELL_VOLTAGE				= 0x0381,
	/// The mid-point voltage of a battery bank in 0.01V (un16)
	VE_REG_BATTERY_MID_POINT_VOLTAGE		= 0x0382,
	/// The mid-point deviation relative to the expected value in 0.1 % (sn16)
	VE_REG_BATTERY_MID_POINT_DEVIATION		= 0x0383,
	/// Maximum/minimum cell voltage (involatile) [min voltage in 0.01V (un16)][max voltage in 0.01V (un16)]
	VE_REG_BATTERY_CELL_VOLTAGE_MIN_MAX		= 0x0384,

	/// Backlight always on (un8). 0=off, 1=on
	VE_REG_BACKLIGHT_ALWAYS_ON				= 0x0400,

	/// Time to go in as a un16 minutes.
	VE_REG_TTG								= 0x0FFE,
	/// State of charge as a un16 in 0.01 %. (un16)
	VE_REG_SOC								= 0x0FFF,
	/// Battery capacity in Ah. (un16)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_BATTERY_CAPACITY					= 0x1000,
	/// Charged voltage in 0.1 V. (un16)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_CHARGED_VOLTAGE					= 0x1001,
	/// Charged current in 0.1 %. (un16)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_CHARGED_CURRENT					= 0x1002,
	/// Charged current in minutes. (un16)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_CHARGE_DETECTION_TIME			= 0x1003,
	/// Charged efficiency in in %. (un16)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_CHARGE_EFFICIENCY				= 0x1004,
	/// Peukert coefficient in steps of 0.01. (un16)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_PEUKERT_COEFFICIENT				= 0x1005,
	/// Current threshold in 0.01 A. (un16)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_CURRENT_THRESHOLD				= 0x1006,
	/// TTG Delta T in minutes. (un16)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_TTG_DELTA_T						= 0x1007,
	/// Low SOC (Discharge floor) in 0.1 %. (un16)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_LOW_SOC							= 0x1008,
	/// Low SOC Clear (Relay Low Soc Clear) in 0.1 %.
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_LOW_SOC_CLEAR					= 0x1009,
	/**
	 * The minimal number of minutes the relay in enabled.
	 * @remark Compatible with BMV HEX protocol.
	 */
	VE_REG_RELAY_MIN_ENABLED				= 0x100a,
	/**
	 * The number of minutes the relay remains enabled after the relay condition is false
	 * @remark Compatible with BMV HEX protocol.
	 */
	VE_REG_RELAY_DISABLE_DELAY				= 0x100b,

	/// User current zero in counts. (sn16)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_CURRENT_OFFSET					= 0x1034,

	/// Zero current command (no data)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_ZERO_CURRENT						= 0x1029,
	/// Synchronize monitor command (no data)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_SYNCHRONIZE						= 0x102c,
	/// Clear history command (no data)
	/// @remark Compatible with BMV HEX protocol.
	VE_REG_CLEAR_HISTORY					= 0x1030,
	/// Transmit history data (normally history data is sent on change only, write to this register to request a transmission)
	/// @deprecated Use vregs @ref VE_REG_HISTORY_TOTAL (0x104F), @ref VE_REG_HISTORY_DAY00 (0x1050) .. @ref VE_REG_HISTORY_DAY30 (0x106E) instead
	VE_REG_TRANSMIT_HISTORY					= 0x1031,

	/// History data total record (fast-packet: un8 = reserved, un8 = database, 4*un8 = errors, un32 = user yield [0.01kWh], un32 = system yield [0.01kWh], un16 = vpvmax [0.01V], un16 = vbatmax [0.01V], un8 = number of days)
	/// @remark reserved: for future extensions/version changes/etc
	/// @remark database: error database (devices may use different error codes)
	/// @remark number of days: 0=only today, 1=also yesterday, etc...
	/// @remark a value of 0xFF/0xFFFF/0xFFFFFFFF (depending on the type) indicates that a field is unknown/not available
	VE_REG_HISTORY_TOTAL					= 0x104F,
	/// History data day record day 0 = today (fast-packet: un8 = reserved, un32 = yield [0.01kWh], un32 = consumed [0.01kWh], un16 = vbatmax [0.01V], un16 = vbatmin [0.01V], un8 = database, 4*un8 = errors, un16 = time in bulk [1 minute], un16 = time in absorption [1 minute], un16 = time in float [1 minute], un32 = pmax [1W], un16 = ibatmax [0.1A], un16 = vpvmax [0.01V], un16 = day sequence number)
	/// @remark reserved: for future extensions/version changes/etc
	/// @remark database: error database (devices may use different error codes)
	/// @remark a value of 0xFF/0xFFFF/0xFFFFFFFF (depending on the type) indicates that a field is unknown/not available
	/// @remark requesting a day that is not (yet) available results in an error response
	VE_REG_HISTORY_DAY00					= 0x1050,
	/// History data day record day -1 = yesterday
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY01					= 0x1051,
	/// History data day record day -2
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY02					= 0x1052,
	/// History data day record day -3
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY03					= 0x1053,
	/// History data day record day -4
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY04					= 0x1054,
	/// History data day record day -5
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY05					= 0x1055,
	/// History data day record day -6
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY06					= 0x1056,
	/// History data day record day -7
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY07					= 0x1057,
	/// History data day record day -8
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY08					= 0x1058,
	/// History data day record day -9
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY09					= 0x1059,
	/// History data day record day -10
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY10					= 0x105A,
	/// History data day record day -11
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY11					= 0x105B,
	/// History data day record day -12
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY12					= 0x105C,
	/// History data day record day -13
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY13					= 0x105D,
	/// History data day record day -14
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY14					= 0x105E,
	/// History data day record day -15
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY15					= 0x105F,
	/// History data day record day -16
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY16					= 0x1060,
	/// History data day record day -17
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY17					= 0x1061,
	/// History data day record day -18
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY18					= 0x1062,
	/// History data day record day -19
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY19					= 0x1063,
	/// History data day record day -20
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY20					= 0x1064,
	/// History data day record day -21
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY21					= 0x1065,
	/// History data day record day -22
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY22					= 0x1066,
	/// History data day record day -23
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY23					= 0x1067,
	/// History data day record day -24
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY24					= 0x1068,
	/// History data day record day -25
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY25					= 0x1069,
	/// History data day record day -26
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY26					= 0x106A,
	/// History data day record day -27
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY27					= 0x106B,
	/// History data day record day -28
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY28					= 0x106C,
	/// History data day record day -29
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY29					= 0x106D,
	/// History data day record day -30
	/// @remark see @ref VE_REG_HISTORY_DAY00
	VE_REG_HISTORY_DAY30					= 0x106E,

	/// Charger link, algorithm version used in priority mechanism (un8 = algorithm, read-only)
	/// @remark algorithm: 0=lowest..0xFD=highest, 0xFE=reserved, 0xFF=not available
	VE_REG_LINK_ALGORITHM					= 0x2000,
	/// Charger link, voltage set-point (un16 = voltage [0.01V], read-only)
	/// @remark only the master broadcasts voltage set-point
	VE_REG_LINK_VSET						= 0x2001,
	/// Charger link, battery sense voltage (un16 = voltage [0.01V], read-only)
	/// @remark voltage: 0xFFFF=not available
	/// @remark any member broadcasts battery sense voltage when available
	VE_REG_LINK_VSENSE						= 0x2002,
	/// Charger link, battery temperature (sn16 = temperature [0.01C], read-only)
	/// @remark temperature: 0xFFFF=not available
	/// @remark any member broadcasts battery temperature when available
	VE_REG_LINK_TSENSE						= 0x2003,
	/// Charger link, command channel from slaves to master (un8 = command)
	/// @remark see VE_REG_LINK_COMMAND section for possible values
	VE_REG_LINK_COMMAND						= 0x2004,
	/// Charger link, binary switch bank status (un32 = switches, read-only)
	/// @remark switches: B0=switch 1, B1=switch 2, etc. bit value 1=on, 0=off
	VE_REG_LINK_BINARY_SWITCH_BANK_STATUS	= 0x2005,
	/// Charger link, binary switch bank mask (un32 = mask, read-only)
	/// @remark mask: B0=switch 1, B1=switch 2, etc. bit value 1=on, 0=off
	VE_REG_LINK_BINARY_SWITCH_BANK_MASK		= 0x2006,
	/// Charger link, state elapsed time (un32 = time [ms], read-only)
	VE_REG_LINK_ELAPSED_TIME				= 0x2007,
	/// Charger link, solar absorption time (un16 = time [0.01hours], read-only)
	VE_REG_LINK_ABSORPTION_TIME				= 0x2008,

	/// Charger link, device status (un8 = state, read-only)
	/// @remark state: B0=group master, B1=instance master, B2=stand-alone
	/// @remark for informational purpose only
	VE_REG_LINK_DEVICE_STATUS				= 0x200F,

	/// Charger link, maximum ac input current (un16 = current [0.1A], read-only)
	VE_REG_LINK_AC_INPUT_MAX_CURRENT		= 0x2010,
	/// Charger link, ac input current limit (un16 = current [0.1A])
	VE_REG_LINK_AC_INPUT_CURRENT_LIMIT		= 0x2011,
	/// Charger link, actual ac input current (un16 = current [0.1A], read-only)
	VE_REG_LINK_AC_INPUT_CURRENT			= 0x2012,

	/// Charger link, charge current percentage (un8 = percentage [%])
	/// @remark percentage: valid range 0..100, 0xFF=not available
	/// @remark 60s timeout
	VE_REG_LINK_CHARGE_CURRENT_PERCENTAGE	= 0x2014,
	/// Charger link, charge current (un16 = current [0.1A])
	/// @remark current: 0xFFFF=not available
	/// @remark 60s timeout
	VE_REG_LINK_CHARGE_CURRENT_LIMIT		= 0x2015,
	/// Charger link, charge voltage (un16 = voltage [0.01V])
	/// @remark voltage: 0xFFFF=not available
	/// @remark 60s timeout
	/// @deprecated Use register @ref VE_REG_LINK_VSET (0x2001) instead
	VE_REG_LINK_CHARGE_VOLTAGE_SETPOINT		= 0x2016,

	/// Charger link, charger system yield (un32 = yield [0.01kWh], read-only)
	VE_REG_LINK_SYSTEM_YIELD				= 0x2020,
	/// Charger link, charger group user yield (un32 = yield [0.01kWh], read-only)
	VE_REG_LINK_USER_YIELD					= 0x2021,
	/// Charger link, charger group yield today (un32 = yield [0.01kWh], read-only)
	VE_REG_LINK_TODAY_YIELD					= 0x2022,
	/// Charger link, charger group maximum power today 1W (un16 = power [1W], read-only)
	VE_REG_LINK_TODAY_PMAX					= 0x2023,
	/// Charger link, charger group yield yesterday (un32 = yield [0.01kWh], read-only)
	VE_REG_LINK_YESTERDAY_YIELD				= 0x2024,
	/// Charger link, charger group maximum power yesterday (un16 = power [1W], read-only)
	VE_REG_LINK_YESTERDAY_PMAX				= 0x2025,
	/// Charger link, charger group actual power (un32 = power [0.01W], read-only)
	VE_REG_LINK_ACTUAL_POWER				= 0x2026,

	/// Device link, real time clock light/dark information (un8 = state, read-only)
	/// @remark state: 0=dark, 1=light, 0xFF=unknown
	VE_REG_LINK_RTC_SOLAR					= 0x2030,
	/// Device link, real time clock time (un16 = time [1 minute])
	/// @remark time: valid range 0..1440, 0=mid-night, 0xFFFF=unknown/not set
	VE_REG_LINK_RTC_TIME					= 0x2031,

	// Device link, synchronise timebase (TBD)
	//VE_REG_LINK_SYNC_TIMEBASE				= 0x2032,
	// Device link, synchronise phase (TBD)
	//VE_REG_LINK_SYNC_PHASE				= 0x2033,

	/// Charger battery parameters - battery safe mode (un8 = mode)
	/// @remark mode: 0=off, 1=on (default)
	VE_REG_BAT_SAFE_MODE					= 0xEDFF,
	/// Charger battery parameters - adaptive mode (un8 = mode)
	/// @remark mode: 0=off, 1=on (default)
	VE_REG_BAT_ADAPTIVE_MODE				= 0xEDFE,
	/// Charger battery parameters - traction curve/auto equalize (un8 = mode)
	/// @remark mode: 0=off (default), 1..250=repeat every n days (1=every day, 2=every other day, etc)
	VE_REG_BAT_TRACTION_CURVE				= 0xEDFD,
	/// Charger battery parameters - bulk time limit (un16 = time [0.01 hours])
	/// @remark time: 0=off
	VE_REG_BAT_BULK_T_LIMIT					= 0xEDFC,
	/// Charger battery parameters - maximum absorption time (un16 = time [0.01 hours])
	/// @remark time: 0=off
	VE_REG_BAT_ABS_T_LIMIT					= 0xEDFB,
	/// Charger battery parameters - maximum float time (un16 = time [0.01 hours])
	/// @remark time 0=off
	VE_REG_BAT_FLOAT_T_LIMIT				= 0xEDFA,
	/// Charger battery parameters - repeated absorption time duration (un16 = time [0.01 hours])
	VE_REG_BAT_REPABS_T_DUR					= 0xEDF9,
	/// Charger battery parameters - repeated absorption time interval (un16 = time [0.01 days])
	VE_REG_BAT_REPABS_T_INT					= 0xEDF8,
	/// Charger battery parameters - absorption voltage (un16 = voltage [0.01V])
	VE_REG_BAT_ABSORPTION_V					= 0xEDF7,
	/// Charger battery parameters - float (un16 = voltage [0.01V])
	VE_REG_BAT_FLOAT_V						= 0xEDF6,
	/// Charger battery parameters - storage voltage (un16 = voltage [0.01V])
	VE_REG_BAT_STORAGE_V					= 0xEDF5,
	/// Charger battery parameters - equalization voltage (un16 = voltage [0.01V])
	VE_REG_BAT_EQUAL_V						= 0xEDF4,
	/// Charger battery parameters - low voltage alarm (un16 = voltage [0.01V])
	/// @deprecated: use @ref VE_REG_RELAY_LOW_VOLTAGE_SET (0x0350) and @ref VE_REG_RELAY_LOW_VOLTAGE_CLEAR (0x0351) instead
	VE_REG_BAT_DISCHARGE_V					= 0xEDF3,
	/// Charger battery parameters - temperature compensation (sn16 = temperature [0.01mV/degree K])
	VE_REG_BAT_TEMP_COMP					= 0xEDF2,
	/// Charger battery parameters - battery type (un8 = type)
	/// @remark type: 0xFF=user defined, 1..254=predefined type, 0=charger disabled
	VE_REG_BAT_TYPE							= 0xEDF1,
	/// Charger battery parameters - maximum charge current (un16 = current [0.1A])
	VE_REG_BAT_MAX_CURRENT					= 0xEDF0,
	/// Charger battery parameters - battery voltage selection (un8 = voltage [1V])
	/// @remark voltage: 0=AUTO, 12, 24, 36 and 48
	/// @remark this vreg reports the actual voltage used (relevant for devices with automatic voltage detection), use @ref VE_REG_BAT_VOLTAGE_SETTING to get the user setting
	VE_REG_BAT_VOLTAGE						= 0xEDEF,
	/// Charger battery parameters - storage mode (un8 = mode)
	/// @remark mode: 0=off, 1=on
	VE_REG_BAT_STORAGE_MODE					= 0xEDEE,
	/// Charger battery parameters - intelligent mode (un8 = mode)
	/// @remark mode: 0=off, 1=on (detect state of charge at startup)
	VE_REG_BAT_INTELLIGENT_MODE				= 0xEDED,
	/// Charger battery parameters - battery temperature (un16 = temperature [0.01K])
	VE_REG_BAT_TEMPERATURE					= 0xEDEC,
	/// Charger battery parameters - high voltage alarm (un16 = voltage [0.01V])
	/// @deprecated use @ref VE_REG_RELAY_HIGH_VOLTAGE_SET (0x0352) and @ref VE_REG_RELAY_HIGH_VOLTAGE_CLEAR (0x0353) instead
	VE_REG_BAT_HIGH_V						= 0xEDEB,
	/// Charger battery parameters - battery voltage setting (un8 = voltage [1V])
	/// @remark voltage: 0=AUTO, 12, 24, 36 and 48
	/// @remark this vreg reports the user setting when read
	VE_REG_BAT_VOLTAGE_SETTING				= 0xEDEA,
	/// Charger battery parameters - power supply mode voltage setting (un16 = voltage [0.01V])
	VE_REG_BAT_PSU_V						= 0xEDE9,
	/// Charger battery parameters - BMS present setting (un8 = bms)
	/// @remark bms: 0=absent, 1=present
	VE_REG_BAT_BMS_PRESENT					= 0xEDE8,
	/// Charger battery parameters - tail current (abs->float transition) (un16 = current [0.1A])
	VE_REG_BAT_TAIL_CURRENT					= 0xEDE7,

	/// Charger specific parameters - maximum charge current (un16 = current [0.1A], read-only)
	VE_REG_CHR_MAX_CURRENT					= 0xEDDF,
	/// Charger specific parameters - number of outputs (un8 = outputs, read-only)
	VE_REG_CHR_OUTPUTS						= 0xEDDE,
	/// Charger specific parameters - charger group yield (un32 = yield [0.01kWh], read-only)
	VE_REG_CHR_SYSTEM_YIELD					= 0xEDDD,
	/// Charger specific parameters - charger group yield (un32 = yield [0.01kWh]) user resettable
	/// @remark use @ref VE_REG_CLEAR_HISTORY to clear
	VE_REG_CHR_USER_YIELD					= 0xEDDC,
	/// Charger specific parameters - charger internal temperature (sn16 = temperature [0.01 degrees C], read-only)
	VE_REG_CHR_TEMPERATURE					= 0xEDDB,
	/// Charger specific parameters - charger error code (un8 = error, read-only)
	/// @remark error: 0=no error
	VE_REG_CHR_ERROR_CODE					= 0xEDDA,
	/// Charger specific parameters - charger relay mode (un8 = mode)
	/// @remark mode: 0=always open, 1..254=application defined, 255=remote control
	/// @deprecated use @ref VE_REG_RELAY_MODE for future products.
	VE_REG_CHR_RELAY_MODE					= 0xEDD9,
	/// Charger specific parameters - charger relay state (un8 = state)
	/// @remark state: 0=open, 1=closed
	/// @deprecated use @ref VE_REG_RELAY_CONTROL instead
	VE_REG_CHR_RELAY_CONTROL				= 0xEDD8,
	/// Charger specific parameters - actual charge current (sn16 = current [0.1A], read-only)
	/// @remark HEX protocol compat. data present in regular NMEA pgn
	VE_REG_CHR_CURRENT						= 0xEDD7,
	/// Charger specific parameters - actual charge power (un16 = power [1W], read-only)
	VE_REG_CHR_POWER						= 0xEDD6,
	/// Charger specific parameters - actual charge voltage (un16 = voltage [0.01V], read-only)
	/// @remark HEX protocol compat. data present in regular NMEA pgn
	VE_REG_CHR_VOLTAGE						= 0xEDD5,
	/// Charger specific parameters - additional charger state info (un8 = state, read-only)
	/// see VE_REG_CHR_CUSTOM_STATE section for possible values
	VE_REG_CHR_CUSTOM_STATE					= 0xEDD4,
	/// Charger specific parameters - charger group yield today (un32 = yield [0.01kWh], read-only)
	VE_REG_CHR_TODAY_YIELD					= 0xEDD3,
	/// Charger specific parameters - charger group maximum power today (un32 = power [1W], read-only)
	/// @remark attention this field has been promoted from un16 to un32, mppt chargers with older firmware may report this as an un16
	VE_REG_CHR_TODAY_PMAX					= 0xEDD2,
	/// Charger specific parameters - charger group yield yesterday (un32 = yield [0.01kWh], read-only)
	VE_REG_CHR_YESTERDAY_YIELD				= 0xEDD1,
	/// Charger specific parameters - charger group maximum power yesterday (un32 = power [1W], read-only)
	/// @remark attention this field has been promoted from un16 to un32, mppt chargers with older firmware may report this as an un16
	VE_REG_CHR_YESTERDAY_PMAX				= 0xEDD0,
	/// Charger specific parameters - battery voltage setting range (un16 low byte = min voltage [1V], high byte = max voltage [1V], read-only)
	VE_REG_CHR_VOLTAGE_SETTINGS_RANGE		= 0xEDCE,
	/// Charger specific parameters - history version (un8 = version, read-only)
	VE_REG_CHR_HISTORY_VERSION				= 0xEDCD,
	/// Charger specific parameters - streetlight application version (un8 = version, read-only)
	VE_REG_CHR_STREETLIGHT_VERSION			= 0xEDCC,

	/// Charger input specific parameters - maximum input current (un16 = current [0.1A], read-only)
	VE_REG_DC_INPUT_MAX_CURRENT				= 0xEDBF,
	/// Charger input specific parameters - maximum input current limit (un16 = current [0.1A])
	VE_REG_DC_INPUT_CURRENT_LIMIT			= 0xEDBE,
	/// Charger input specific parameters - actual input current (un16 = current [0.1A], read-only)
	VE_REG_DC_INPUT_CURRENT					= 0xEDBD,
	/// Charger input specific parameters - actual input power (un32 = power [0.01W], read-only)
	VE_REG_DC_INPUT_POWER					= 0xEDBC,
	/// Charger input specific parameters - actual input voltage (un16 = voltage [0.01V])
	VE_REG_DC_INPUT_VOLTAGE					= 0xEDBB,
	/// Charger input specific parameters - input high voltage upper threshold (un16 = voltage [0.01V])
	VE_REG_DC_INPUT_HIGH_VOLTAGE_SET		= 0xEDBA,
	/// Charger input specific parameters - input high voltage upper threshold (un16 = voltage [0.01V])
	VE_REG_DC_INPUT_HIGH_VOLTAGE_CLEAR		= 0xEDB9,
	/// Charger input specific parameters - input maximum voltage (un16 = voltage [0.01V])
	VE_REG_DC_INPUT_MAX_VOLTAGE				= 0xEDB8,
	
	/// Charger load output specific parameters - maximum allowed load current (un16 = current [0.1A], read-only)
	VE_REG_DC_OUTPUT_MAX_CURRENT			= 0xEDAF,
	/// Charger load output specific parameters - load current limit (un16 = current [0.1A])
	VE_REG_DC_OUTPUT_CURRENT_LIMIT			= 0xEDAE,
	/// Charger load output specific parameters - actual load current (un16 = current [0.1A], read-only)
	VE_REG_DC_OUTPUT_CURRENT				= 0xEDAD,
	/// Charger load output offset voltage - load battery life algorithm (un8 = voltage [0.01V], read-only)
	VE_REG_DC_OUTPUT_OFFSET_VOLTAGE         = 0xEDAC,
	/// Charger load output specific parameters - load control (un8 = mode)
	/// see VE_REG_DC_OUTPUT_CONTROL section for possible values
	VE_REG_DC_OUTPUT_CONTROL				= 0xEDAB,
	/// Charger load output specific parameters - actual load power (un16 = power [1W], read-only)
	VE_REG_DC_OUTPUT_POWER					= 0xEDAA,
	/// Charger load output specific parameters - actual load voltage (un16 = voltage [0.01V], read-only)
	VE_REG_DC_OUTPUT_VOLTAGE				= 0xEDA9,
	/// Charger load output specific parameters - actual load status (un8 = status, read-only)
	/// @remark status: 0=off, 1=on
	VE_REG_DC_OUTPUT_STATUS					= 0xEDA8,

	/// Charger load output specific parameters - lighting controller midpoint shift (sn16 = time [1 minute])
	VE_REG_DC_OUTPUT_MIDPOINT_SHIFT			= 0xEDA7,
	/// Charger load output specific parameters - lighting controller dim setting (un8 = dim [%])
	/// @remark dim: valid range 0..100 (0=load output off)
	/// @remark this vreg cna be used to query the current dimlevel when a timer program is active (e.g. to create an external pwm signal)
	VE_REG_DC_OUTPUT_LIGHTING_DIM			= 0xEDA6,
	/// Charger load output specific parameters - lighting controller event 5 (un8 = event code, un8 = dimlevel [%], sn16 = time offset [1 minute])
	/// @remark see @ref VE_REG_DC_OUTPUT_LIGHTING_EVENT0
	VE_REG_DC_OUTPUT_LIGHTING_EVENT5		= 0xEDA5,
	/// Charger load output specific parameters - lighting controller event 4 (un8 = event code, un8 = dimlevel [%], sn16 = time offset [1 minute])
	/// @remark see @ref VE_REG_DC_OUTPUT_LIGHTING_EVENT0
	VE_REG_DC_OUTPUT_LIGHTING_EVENT4		= 0xEDA4,
	/// Charger load output specific parameters - lighting controller event 3 (un8 = event code, un8 = dimlevel [%], sn16 = time offset [1 minute])
	/// @remark see @ref VE_REG_DC_OUTPUT_LIGHTING_EVENT0
	VE_REG_DC_OUTPUT_LIGHTING_EVENT3		= 0xEDA3,
	/// Charger load output specific parameters - lighting controller event 2 (un8 = event code, un8 = dimlevel [%], sn16 = time offset [1 minute])
	/// @remark see @ref VE_REG_DC_OUTPUT_LIGHTING_EVENT0
	VE_REG_DC_OUTPUT_LIGHTING_EVENT2		= 0xEDA2,
	/// Charger load output specific parameters - lighting controller event 1 (un8 = event code, un8 = dimlevel [%], sn16 = time offset [1 minute])
	/// @remark see @ref VE_REG_DC_OUTPUT_LIGHTING_EVENT0
	VE_REG_DC_OUTPUT_LIGHTING_EVENT1		= 0xEDA1,
	/// Charger load output specific parameters - lighting controller event 0 (un8 = event code, un8 = dimlevel [%], sn16 = time offset [1 minute])
	/// @remark event code: time offset relates to 1=sunset, 2=sunrise, 3=midnight
	/// @remark dimlevel: valid range 0..100 (0=load output off)
	VE_REG_DC_OUTPUT_LIGHTING_EVENT0		= 0xEDA0,

	/// Charger physical selection, useful for unit identification in a network (un8 mode)
	/// @remark mode: 0=normal operation (default), 1=blink/beep
	/// @deprecated Use register @ref VE_REG_IDENTIFY (0x010E) instead
	VE_REG_CAN_SELECT						= 0xED9F,

	/// Special function selection for the TX pin on the ve.direct port (un8 = mode)
	/// see VE_REG_VEDIRECT_PORT_TX_FUNCTION section for possible values
	VE_REG_VEDIRECT_PORT_TX_FUNCTION		= 0xED9E,

	/// Charger load output specific parameters - user defined switch high voltage level (un16 = voltage [0.01V])
	VE_REG_DC_OUTPUT_SWITCH_HIGH_LEVEL		= 0xED9D,
	/// Charger load output specific parameters - user defined switch low voltage level (un16 = voltage [0.01V])
	VE_REG_DC_OUTPUT_SWITCH_LOW_LEVEL		= 0xED9C,
	/// Charger load output specific parameters - lighting controller dim speed setting (un8)
	/// @remark dim: valid range 0..100 (0=instantaneous change, x=1% change per x seconds, e.g. 9 takes 15 minutes to dim from 0..100%)
	VE_REG_DC_OUTPUT_LIGHTING_DIM_SPEED		= 0xED9B,
	/// Charger load output specific parameters - panel night voltage detection level (un16 = voltage [0.01V])
	VE_REG_DC_OUTPUT_PANEL_VOLTAGE_NIGHT	= 0xED9A,
	/// Charger load output specific parameters - panel day voltage detection level (un16 = voltage [0.01V])
	VE_REG_DC_OUTPUT_PANEL_VOLTAGE_DAY		= 0xED99,


	/// Charger specific parameters - actual charge current channel 1 (sn16 = current [0.1A], read-only)
	VE_REG_DC_CHANNEL1_CURRENT				= 0xED8F,
	/// Charger specific parameters - actual charge power channel 1 (sn16 = power [1W], read-only)
	VE_REG_DC_CHANNEL1_POWER				= 0xED8E,
	/// Charger specific parameters - actual charge voltage channel 1 (sn16 = voltage [0.01V], read-only)
	VE_REG_DC_CHANNEL1_VOLTAGE				= 0xED8D,

	/// Charger specific parameters - actual charge current channel 2 (sn16 = current [0.1A], read-only)
	VE_REG_DC_CHANNEL2_CURRENT				= 0xED7F,
	/// Charger specific parameters - actual charge power channel 2 (sn16 = power [1W], read-only)
	VE_REG_DC_CHANNEL2_POWER				= 0xED7E,
	/// Charger specific parameters - actual charge voltage channel 2 (sn16 = voltage [0.01V], read-only)
	VE_REG_DC_CHANNEL2_VOLTAGE				= 0xED7D,

	/// Charger specific parameters - actual charge current channel 3 (sn16 = current [0.1A], read-only)
	VE_REG_DC_CHANNEL3_CURRENT				= 0xED6F,
	/// Charger specific parameters - actual charge power channel 3 (sn16 = power [1W], read-only)
	VE_REG_DC_CHANNEL3_POWER				= 0xED6E,
	/// Charger specific parameters - actual charge voltage channel 3 (sn16 = voltage [0.01V], read-only)
	VE_REG_DC_CHANNEL3_VOLTAGE				= 0xED6D,

	/**
	* These VREG id's are used by VE.Direct consumer stack
	* Range: 0xEC00 - 0xEC0F
	*/
	VE_REG_BMV 								= 0xEC04,
	VE_REG_PROD 							= 0xEC03,
	VE_REG_VER								= 0xEC02,
	VE_REG_HSDS								= 0xEC01,
	VE_REG_LAST_MESSAGE						= 0xEC00,
	VE_REG_CAH								= 0xEEFF /* equal to VE_REG_BMV_CE */

} VeRegId;

/*
 * VE_REG_PRODUCT_ID 0x0100
 *
 *	un8 instance
 *	un16 productId
 *	un8 flags
 *
 * multiple products can be reported by a single node.
 */

#define VE_PROD_ID_FLAGS(f)			( (f) & 0xff)
#define VE_PROD_ID_INSTANCE(i)		( ( (un32)(i) & 0xff) << 24)
#define VE_PROD_ID_PRODUCT_ID(pid) 					\
		( ( ( (un32)(pid)       & 0xff) << 16) | 	\
		( ( ( (un32)(pid) >> 8) & 0xff) <<  8))


// Properties of the device function performing the update e.g. a bootloader.
/// Whether the device can be update.
#define VE_PROD_ID_F_VUP_SUPPORT	0x01
#define VE_PROD_ID_F_RESERVED		0xFE

/*
 * VE_REG_APP_VER 		- 0x0102
 * VE_REG_APP_VER_MIN 	- 0x0103
 *
 *	un8 instance
 *	un24 version
 *
 * multiple versions can be reported by a single node.
 */
#define VREG_APP_VER_INSTANCE(i)	( ( (un32)(i) & 0xff) << 24)
#define VREG_APP_VER_VERSION(v)					\
		( ( ( (un32)(v)        & 0xff) << 16) |	\
		( ( ( (un32)(v) >>  8) & 0xff) <<  8) |	\
		( ( ( (un32)(v) >> 16) & 0xff)      ))


#define VREG_VER_INVALID_VERSION	0xFFFFFF

/*
 * VE_REG_UDF_VERSION	- 0x0110
 *
 *	un24 version
 *	un8 flags
 */

#define VREG_UDF_VER_FLAGS(f)		( (f) & 0xff)
#define VREG_UDF_VER_VERSION(v)					\
		( ( ( (un32)(v)        & 0xff) << 24) |	\
		( ( ( (un32)(v) >>  8) & 0xff) << 16) |	\
		( ( ( (un32)(v) >> 16) & 0xff) <<  8))

#define VREG_UDF_VER_F_RESERVED		0xFE
/// Bootloader active.
#define VREG_UDF_VER_F_ACTIVE		0x01


/*
 * VE_REG_DEVICE_MODE	- 0x0200
 *
 *	un8 mode
 */
#define VE_REG_MODE_CHARGER				0x01
#define VE_REG_MODE_INVERTER			0x02
#define VE_REG_MODE_ON					0x03
#define VE_REG_MODE_OFF					0x04

/*
 * VE_REG_REMOTE_CONTROL_USED	- 0x0202
 *
 *	un32 flags
 */
#define VE_REG_RC_AC1_IN_LIMIT			0x00000001
#define VE_REG_RC_ON_OFF_MODE			0x00000002
#define VE_REG_RC_AC2_IN_LIMIT			0x00000004
#define VE_REG_RC_SEND_LEDS				0x00000100
#define VE_REG_RC_SEND_CELL_VOLTAGES	0x00010000
#define VE_REG_RC_SEND_RELAYS			0x00020000

/*
 * VE_REG_CHR_CUSTOM_STATE	- 0xEDD4
 *
 *	un8 state
 */
#define VE_REG_CUSTOM_STATE_SAFE_MODE		0x01
#define VE_REG_CUSTOM_STATE_TPTB_MODE		0x02
#define VE_REG_CUSTOM_STATE_REPEATED_ABS	0x04
#define VE_REG_CUSTOM_STATE_TEMP_DIM		0x10
#define VE_REG_CUSTOM_STATE_SENSE_DIM		0x20
#define VE_REG_CUSTOM_STATE_INP_CUR_DIM		0x40
#define VE_REG_CUSTOM_STATE_LOW_POWER_MODE	0x80

/*
 * VE_REG_DC_OUTPUT_CONTROL	- 0xEDAB
 *
 *	un8 mode
 */
#define VE_REG_DC_OUTPUT_CTRL_MASK			0x0F
#define VE_REG_DC_OUTPUT_CTRL_FORCE_OFF		0x00
#define VE_REG_DC_OUTPUT_CTRL_AUTO			0x01
#define VE_REG_DC_OUTPUT_CTRL_ALT1			0x02
#define VE_REG_DC_OUTPUT_CTRL_ALT2			0x03
#define VE_REG_DC_OUTPUT_CTRL_FORCE_ON		0x04
#define VE_REG_DC_OUTPUT_CTRL_USER1			0x05
#define VE_REG_DC_OUTPUT_CTRL_USER2			0x06
#define VE_REG_DC_OUTPUT_CTRL_LIGHT_FLAG	0x80

/*
 * VE_REG_VEDIRECT_PORT_TX_FUNCTION - 0xED9E
 *
 * un8 mode
 */
#define VE_REG_PORT_TX_VE_DIRECT			0x00
#define VE_REG_PORT_TX_001KWH_PULSES		0x01
#define VE_REG_PORT_TX_LIGHT_PWM_NORMAL		0x02
#define VE_REG_PORT_TX_LIGHT_PWM_INVERTED	0x03

/*
 * VE_REG_LINK_COMMAND	- 0x2004
 *
 *	un8 command
 */
#define VE_REG_COMMAND_START_EQUALISE		0x01
#define VE_REG_COMMAND_STOP_EQUALISE		0x02
#define VE_REG_COMMAND_GUI_SYNC				0x03
#define VE_REG_COMMAND_DAY_SYNC				0x04

/*
 * VE_REG_RELAY_MODE	- 0x034F
 *
 *	un8 mode
 */
#define VE_REG_RELAY_MODE_ALARM				0x00
#define VE_REG_RELAY_MODE_CHARGER			0x01
#define VE_REG_RELAY_MODE_REMOTE			0x02
#define VE_REG_RELAY_MODE_ALWAYS_OPEN		0x03

/*
 * VE_REG_CAPABILITIES1	- 0x0140
 *
 *	un32 capabilities
 */
#define VE_REG_CAPABILITIES1_HAS_LOAD_OUTPUT		(1ul)
#define VE_REG_CAPABILITIES1_HAS_ROTARY_ENCODER		(1ul << 1)
#define VE_REG_CAPABILITIES1_HAS_HISTORY_SUPPORT	(1ul << 2)
#define VE_REG_CAPABILITIES1_HAS_BATTERYSAFE_MODE	(1ul << 3)
#define VE_REG_CAPABILITIES1_HAS_ADAPTIVE_MODE		(1ul << 4)
#define VE_REG_CAPABILITIES1_HAS_MANUAL_EQUALISE	(1ul << 5)
#define VE_REG_CAPABILITIES1_HAS_AUTO_EQUALISE		(1ul << 6)
#define VE_REG_CAPABILITIES1_HAS_STORAGE_MODE		(1ul << 7)
#define VE_REG_CAPABILITIES1_HAS_REMOTE_ONOFF		(1ul << 8)
#define VE_REG_CAPABILITIES1_HAS_SOLAR_TIMER		(1ul << 9)
#define VE_REG_CAPABILITIES1_HAS_ALT_TX_FUNCTION	(1ul << 10)
#define VE_REG_CAPABILITIES1_HAS_USER_LOAD_SWITCH	(1ul << 11)
#define VE_REG_CAPABILITIES1_HAS_LOAD_CURRENT		(1ul << 12)
#define VE_REG_CAPABILITIES1_HAS_PANEL_CURRENT		(1ul << 13)
#define VE_REG_CAPABILITIES1_HAS_BMS_SUPPORT		(1ul << 14)
#define VE_REG_CAPABILITIES1_HAS_HUB1_SUPPORT		(1ul << 15)
/// @}

/*
 * ==================================================================
 * DEVICE SPECIFIC VREGS, these should be high numbers, but not EDxx.
 * In general include the product in the define so it is clear which
 * product it belongs to.
 * ==================================================================
 */


/*
 * Phoenix inverter specific registers in "page" 0xEB..
 *  This list is generated by the Device-registers.xlsm excel file
 *  (the device specific types are defined in <project>\src\dev_types.h)
 */
// Generated by Device_registers.xlsm at 2014-09-26 14:47:14
/// typeOperatingMode
#define VE_REG_INV_OPER_OPERATING_MODE              (0xEB00)
/// typeOperState readonly
#define VE_REG_INV_OPER_GET_STATE                   (0xEB01)
/// un16 readonly
#define VE_REG_INV_MOD_GET_WAVE_AMPLITUDE           (0xEB02)
/// un16
#define VE_REG_INV_OPER_ECO_MODE_IINV_MIN           (0xEB04)
/// un8
#define VE_REG_INV_OPER_ECO_MODE_IS_IINV_COUNT      (0xEB05)
/// un8
#define VE_REG_INV_OPER_ECO_MODE_RETRY_TIME         (0xEB06)
/// un16 tester
#define VE_REG_INV_MOD_FAST_IPEAK                   (0xEB07)
/// typeProtState readonly
#define VE_REG_INV_PROT_GET_STATE                   (0xEB08)
/// un16 tester
#define VE_REG_INV_MOD_FAST_UPEAK                   (0xEB09)
/// un16 readonly
#define VE_REG_INV_ADC_UINV_RAW                     (0xEB20)
/// un16 readonly
#define VE_REG_INV_ADC_UBAT_RAW                     (0xEB22)
/// un16 readonly
#define VE_REG_INV_ADC_UTRANSF_RAW                  (0xEB23)
/// un16 readonly
#define VE_REG_INV_ADC_UTEMP_FET_RAW                (0xEB24)
/// un16 readonly
#define VE_REG_INV_ADC_UTEMP_MICRO_RAW              (0xEB25)
/// un16 readonly
#define VE_REG_INV_ADC_XRAW                         (0xEB27)
/// un16 readonly
#define VE_REG_INV_ADC_UINV                         (0xEB28)
/// un16 readonly
#define VE_REG_INV_ADC_UBAT                         (0xEB2A)
/// un16 readonly
#define VE_REG_INV_ADC_UTEMP_TRANSF                 (0xEB2B)
/// un16 readonly
#define VE_REG_INV_ADC_UTEMP_FET                    (0xEB2C)
/// un16 readonly
#define VE_REG_INV_ADC_UTEMP_MICRO                  (0xEB2D)
/// un16 readonly
#define VE_REG_INV_ADC_X                            (0xEB2F)
/// un16 readonly
#define VE_REG_INV_ADC_UINV_MEAN                    (0xEB30)
/// un16 readonly
#define VE_REG_INV_ADC_UBAT_MEAN                    (0xEB32)
/// un16 readonly
#define VE_REG_INV_ADC_UREF_ADC                     (0xEB39)
/// un16 readonly
#define VE_REG_INV_ADC_UINV_MEAN_SQUARE             (0xEB3A)
/// un16 readonly
#define VE_REG_INV_ADC_UBAT_MEAN_SQUARE             (0xEB3C)
/// sn16 readonly
#define VE_REG_INV_ADC_TEMP_FET_C                   (0xEB3E)
/// sn16 readonly
#define VE_REG_INV_ADC_TEMP_MICRO_C                 (0xEB3F)
/// typeLoop tester
#define VE_REG_INV_LOOP_OPTIONS                     (0xEB40)
/// un16 tester
#define VE_REG_INV_LOOP_NULL_AMPLITUDE              (0xEB41)
/// un16 tester
#define VE_REG_INV_MOD_PHASEBEAT_PERIOD             (0xEB42)
/// un16
#define VE_REG_INV_LOOP_ERROR_GAIN                  (0xEB43)
/// un16 tester
#define VE_REG_INV_LOOP_SETPOINT                    (0xEB44)
/// un16 tester
#define VE_REG_INV_LOOP_UINV_REFERENCE              (0xEB45)
/// sn32 readonly
#define VE_REG_INV_LOOP_GET_UOUT                    (0xEB46)
/// un8 readonly
#define VE_REG_INV_PROT_GET_IPEAK_LOWLEVEL_EVENT    (0xEB48)
/// un8 readonly
#define VE_REG_INV_PROT_GET_IPEAK_HIGHLEVEL_EVENT   (0xEB49)
/// un8 readonly
#define VE_REG_INV_PROT_GET_UPEAK_EVENT             (0xEB4A)
/// un16 tester
#define VE_REG_INV_LOOP_UBAT_REFERENCE              (0xEB4B)
/// sn16 readonly
#define VE_REG_INV_LOOP_GET_ERROR                   (0xEB4C)
/// un16 readonly
#define VE_REG_INV_LOOP_GET_ERROR_INTEGRATED        (0xEB4D)
/// sn16 readonly
#define VE_REG_INV_LOOP_GET_IINV                    (0xEB4E)
/// un16 tester
#define VE_REG_INV_LOOP_COMPENSATE_ROUT_GIN         (0xEB4F)
/// un16 tester
#define VE_REG_INV_LOOP_AMPLITUDE_CLAMP             (0xEB50)
/// un16 tester
#define VE_REG_INV_LOOP_COMPUTE_IINV_GIN            (0xEB51)
/// typeProtections
#define VE_REG_INV_PROT_ENABLE_MASK                 (0xEB52)
/// sn8 readonly
#define VE_REG_INV_PROT_GET_ACTIVE_LEVEL            (0xEB53)
/// sn8 readonly
#define VE_REG_INV_PROT_GET_HICKUP_COUNT            (0xEB54)
/// typeProtWhat readonly
#define VE_REG_INV_PROT_GET_ACTIVE_PROTECTION       (0xEB55)
/// sn8 readonly
#define VE_REG_INV_PROT_GET_UINV_STATUS             (0xEB56)
/// sn8 readonly
#define VE_REG_INV_PROT_GET_IINV_STATUS             (0xEB57)
/// sn8 readonly
#define VE_REG_INV_PROT_GET_UBAT_STATUS             (0xEB58)
/// sn8 readonly
#define VE_REG_INV_PROT_GET_TEMP_STATUS             (0xEB59)
/// sn8 readonly
#define VE_REG_INV_PROT_GET_IPEAK_COUNT_STATUS      (0xEB5A)
/// sn8 readonly
#define VE_REG_INV_PROT_GET_UBAT_RIPPLE_STATUS      (0xEB5B)
/// typeProtections
#define VE_REG_INV_PROT_ENABLE_HICKUP               (0xEB5C)
/// typeProtections
#define VE_REG_INV_PROT_ENABLE_WAIT4EVER            (0xEB5D)
/// un8
#define VE_REG_INV_PROT_HICKUP_ATTEMPT_COUNT        (0xEB5E)
/// un16
#define VE_REG_INV_PROT_HICKUP_RESTART_DELAY        (0xEB5F)
/// un16
#define VE_REG_INV_PROT_HICKUP_TIME_OUT             (0xEB60)
/// typeGpioErrorLed readonly
#define VE_REG_INV_PROT_GPIO_WARNING_LED_STATE      (0xEB61)
/// un16
#define VE_REG_INV_PROT_UINV_LEVEL_LOW              (0xEB62)
/// un16
#define VE_REG_INV_PROT_UINV_IS_LOW_COUNT           (0xEB63)
/// un16
#define VE_REG_INV_PROT_IINV_LEVEL_ALARM_HIGH       (0xEB64)
/// un16
#define VE_REG_INV_PROT_IINV_IS_ALARM_HIGH_COUNT    (0xEB65)
/// un16
#define VE_REG_INV_PROT_IINV_LEVEL_HIGH             (0xEB66)
/// un16
#define VE_REG_INV_PROT_IINV_IS_HIGH_TIMEOUT        (0xEB67)
/// un16
#define VE_REG_INV_PROT_UBAT_LEVEL_HIGH             (0xEB68)
/// un16
#define VE_REG_INV_PROT_UBAT_IS_HIGH_COUNT          (0xEB69)
/// un16
#define VE_REG_INV_PROT_UBAT_LEVEL_ALARM_LOW        (0xEB6A)
/// un16
#define VE_REG_INV_PROT_UBAT_IS_ALARM_LOW_COUNT     (0xEB6B)
/// un16
#define VE_REG_INV_PROT_UBAT_LEVEL_LOW              (0xEB6C)
/// un16
#define VE_REG_INV_PROT_UBAT_IS_LOW_COUNT           (0xEB6D)
/// un16
#define VE_REG_INV_PROT_UBAT_LEVEL2LOW              (0xEB6E)
/// un8
#define VE_REG_INV_PROT_TEMP_FET_ALARM_HIGH_C       (0xEB6F)
/// un8
#define VE_REG_INV_PROT_TEMP_FET2HIGH_C             (0xEB70)
/// un8
#define VE_REG_INV_PROT_TEMP_TRANSF_THRESHOLD       (0xEB71)
/// un8
#define VE_REG_INV_PROT_IPEAK_LOW_LEVEL_INCREMENT   (0xEB72)
/// un8
#define VE_REG_INV_PROT_IPEAK_HIGH_LEVEL_INCREMENT  (0xEB73)
/// un8
#define VE_REG_INV_PROT_IPEAK_DECREMENT             (0xEB74)
/// un16
#define VE_REG_INV_PROT_IPEAK_COUNT                 (0xEB75)
/// un16
#define VE_REG_INV_PROT_UBAT_RIPPLE_LEVEL_HIGH      (0xEB76)
/// un16
#define VE_REG_INV_PROT_UBAT_RIPPLE_IS_HIGH_COUNT   (0xEB77)
/// un16
#define VE_REG_INV_PROT_UBAT_RIPPLE_LEVEL2HIGH      (0xEB78)
/// un16
#define VE_REG_INV_PROT_UBAT_RIPPLE_IS2HIGH_COUNT   (0xEB79)
/// un16 tester
#define VE_REG_INV_ADC_UINV_GAIN                    (0xEB80)
/// un16 tester
#define VE_REG_INV_ADC_UBAT_GAIN                    (0xEB82)
/// un16 tester
#define VE_REG_INV_ADC_UTEMP_TRANSF_GAIN            (0xEB83)
/// un16 tester
#define VE_REG_INV_ADC_UTEMP_FET_GAIN               (0xEB84)
/// sn16 tester
#define VE_REG_INV_ADC_UINV_OFFSET                  (0xEB88)
/// sn16 tester
#define VE_REG_INV_ADC_UBAT_OFFSET                  (0xEB8A)
/// sn16 tester
#define VE_REG_INV_ADC_UTEMP_TRANS_OFFSET           (0xEB8B)
/// sn16 tester
#define VE_REG_INV_ADC_UTEMP_FET_OFFSET             (0xEB8C)
/// un16
#define VE_REG_INV_PWM_DEBUG2                       (0xEB91)
/// un16 readonly
#define VE_REG_INV_NVM_SERIAL                       (0xEB98)
/// typeNvmCommand
#define VE_REG_INV_NVM_COMMAND                      (0xEB99)
/// typeFanMode tester
#define VE_REG_INV_FAN_MODE                         (0xEBA0)
/// un16 tester
#define VE_REG_INV_FAN_SPEED_MIN                    (0xEBA1)
/// un16 tester
#define VE_REG_INV_FAN_SPEED_MAX                    (0xEBA2)
/// un16 tester
#define VE_REG_INV_FAN_SPEED_PITCH                  (0xEBA3)
/// un16 tester
#define VE_REG_INV_FAN_TIME_PITCH                   (0xEBA4)
/// un16 tester
#define VE_REG_INV_FAN_SPEED_UP                     (0xEBA5)
/// un16 tester
#define VE_REG_INV_FAN_SPEED_DOWN                   (0xEBA6)
/// un8 tester
#define VE_REG_INV_FAN_TEMP_FET_LOW                 (0xEBA7)
/// un8 tester
#define VE_REG_INV_FAN_TEMP_FET_HIGH                (0xEBA8)
/// un16
#define VE_REG_INV_FAN_IINV_LOW                     (0xEBA9)
/// un16
#define VE_REG_INV_FAN_IINV_HIGH                    (0xEBAA)
/// un8
#define VE_REG_INV_FAN_TEMP_AMBIENT_LOW             (0xEBAB)
/// un8
#define VE_REG_INV_FAN_TEMP_AMBIENT_HIGH            (0xEBAC)
/// un16
#define VE_REG_INV_FAN_IINV_ON_COUNT                (0xEBAD)
/// un16
#define VE_REG_INV_FAN_TIME_KEEP_ON                 (0xEBAE)




/// @ingroup VELIB_VECAN_REG
/// @defgroup VELIB_VECAN_REG_BMV BMV Vregs
/// @{
/// @deprecated Only used to support the BMV, use VE_REG_HIST_DEEPEST_DISCHARGE instead.
/// @see VE_REG_HIST_DEEPEST_DISCHARGE
#define VE_REG_BMV_H1							0xEE00
/// @deprecated Only used to support the BMV, use VE_REG_HIST_LAST_DISCHARGE instead.
/// @see VE_REG_HIST_LAST_DISCHARGE
#define VE_REG_BMV_H2							0xEE01
/// @deprecated Only used to support the BMV, use VE_REG_HIST_AVERAGE_DISCHARGE instead.
/// @see VE_REG_HIST_AVERAGE_DISCHARGE
#define VE_REG_BMV_H3							0xEE02
/// @deprecated Only used to support the BMV, use VE_REG_HIST_NR_OF_CHARGE_CYCLES instead.
/// @see VE_REG_HIST_NR_OF_CHARGE_CYCLES
#define VE_REG_BMV_H4							0xEE03
/// @deprecated Only used to support the BMV, use VE_REG_HIST_NR_OF_FULL_DISCHARGES instead.
/// @see VE_REG_HIST_NR_OF_FULL_DISCHARGES
#define VE_REG_BMV_H5							0xEE04
/// @deprecated Only used to support the BMV, use VE_REG_HIST_CUMULATIVE_AH instead.
/// @see VE_REG_HIST_CUMULATIVE_AH
#define VE_REG_BMV_H6							0xEE05
/// @deprecated Only used to support the BMV, use VE_REG_HIST_MIN_VOLTAGE instead.
/// @see VE_REG_HIST_MIN_VOLTAGE
#define VE_REG_BMV_H7							0xEE06
/// @deprecated Only used to support the BMV, use VE_REG_HIST_MAX_VOLTAGE instead.
/// @see VE_REG_HIST_MAX_VOLTAGE
#define VE_REG_BMV_H8							0xEE07
/// @deprecated Only used to support the BMV, use VE_REG_HIST_SECS_SINCE_LAST_FULL_CHARGE instead.
/// @see VE_REG_HIST_SECS_SINCE_LAST_FULL_CHARGE
#define VE_REG_BMV_H9							0xEE08
/// @deprecated Only used to support the BMV, use VE_REG_HIST_NR_OF_AUTO_SYNCS instead.
/// @see VE_REG_HIST_NR_OF_AUTO_SYNCS
#define VE_REG_BMV_H10							0xEE09
/// @deprecated Only used to support the BMV, use VE_REG_HIST_NR_OF_LOW_VOLTAGE_ALARMS instead.
/// @see VE_REG_HIST_NR_OF_LOW_VOLTAGE_ALARMS
#define VE_REG_BMV_H11							0xEE0A
/// @deprecated Only used to support the BMV, use VE_REG_HIST_NR_OF_HIGH_VOLTAGE_ALARMS instead.
/// @see VE_REG_HIST_NR_OF_HIGH_VOLTAGE_ALARMS
#define VE_REG_BMV_H12							0xEE0B
/// @deprecated Only used to support the BMV, use VE_REG_HIST_NR_OF_LOW_VOLTAGE_2_ALARMS instead.
/// @see VE_REG_HIST_NR_OF_LOW_VOLTAGE_2_ALARMS
#define VE_REG_BMV_H13							0xEE0C
/// @deprecated Only used to support the BMV, use VE_REG_HIST_NR_OF_HIGH_VOLTAGE_2_ALARMS instead.
/// @see VE_REG_HIST_NR_OF_HIGH_VOLTAGE_2_ALARMS
#define VE_REG_BMV_H14							0xEE0D
/// @deprecated Only used to support the BMV, use VE_REG_HIST_MIN_VOLTAGE_2 instead.
/// @see VE_REG_HIST_MIN_VOLTAGE_2
#define VE_REG_BMV_H15							0xEE0E
/// @deprecated Only used to support the BMV, use VE_REG_HIST_MAX_VOLTAGE_2 instead.
/// @see VE_REG_HIST_MAX_VOLTAGE_2
#define VE_REG_BMV_H16							0xEE0F

/// Consumed AH (sn32, Ah)
#define VE_REG_BMV_CE							0xEEFF
/// Backlight intensity (un8): 0-9
#define VE_REG_BMV_BACKLIGHT					0xEEFE
/// Sound buzzer when alarm active (un8): 0=off, 1=on
#define VE_REG_BMV_ALARM_BUZZER_ON				0xEEFC
/// Shunt max current in A (un16)
#define VE_REG_BMV_SHUNT_AMPS					0xEEFB
/// Shunt max voltage in mV (un16)
#define VE_REG_BMV_SHUNT_VOLTS					0xEEFA
/// Software version (un16)
#define VE_REG_BMV_SW_VERSION					0xEEF9
/// Auxiliairy input mode (un8): 0=aux, 1=mid, 2=temperature
#define VE_REG_BMV_AUX_INPUT					0xEEF8
/// Temperature unit (un8): 0=celcius, 1=fahrenheit
#define VE_REG_BMV_TEMPERATURE_UNIT				0xEEF7
/// Lock setup (un8): 0=unlocked, 1=locked
#define VE_REG_BMV_SETUP_LOCK					0xEEF6
/// Scroll speed (un8): 1=slowest, 5=fastest
#define VE_REG_BMV_SCROLL_SPEED					0xEEF5
/// Temperature coefficient (un16), temperature compensation for battery capacity in 0.1%CAP
#define VE_REG_BMV_TEMPERATURE_COEFFICIENT		0xEEF4

/// Show voltage in status menu (un8): 0=off, 1=on
#define VE_REG_BMV_SHOW_VOLTAGE					0xEEE0
/// Show auxiliairy voltage in status menu (un8): 0=off, 1=on
#define VE_REG_BMV_SHOW_AUX_VOLTAGE				0xEEE1
/// Show mid voltage in status menu (un8): 0=off, 1=on
#define VE_REG_BMV_SHOW_MID_VOLTAGE				0xEEE2
/// Show current in status menu (un8): 0=off, 1=on
#define VE_REG_BMV_SHOW_CURRENT					0xEEE3
/// Show AH in status menu (un8): 0=off, 1=on
#define VE_REG_BMV_SHOW_AH						0xEEE4
/// Show SOC in status menu (un8): 0=off, 1=on
#define VE_REG_BMV_SHOW_SOC						0xEEE5
/// Show TTG in status menu (un8): 0=off, 1=on
#define VE_REG_BMV_SHOW_TTG						0xEEE6
/// Show temperature in status menu (un8): 0=off, 1=on
#define VE_REG_BMV_SHOW_TEMPERATURE				0xEEE7
/// Show power in status menu (un8): 0=off, 1=on
#define VE_REG_BMV_SHOW_POWER					0xEEE8

/// @remark The next block is only used for the tester
/// Voltage offset (float)
#define VE_REG_BMV_CAL_VOLTAGE_OFFSET			0xEED0
/// Voltage gain (float)
#define VE_REG_BMV_CAL_VOLTAGE_GAIN				0xEED1
/// Voltage2 offset (float)
#define VE_REG_BMV_CAL_VOLTAGE_2_OFFSET			0xEED2
/// Voltage2 gain (float)
#define VE_REG_BMV_CAL_VOLTAGE_2_GAIN			0xEED3
/// Current offset (float)
#define VE_REG_BMV_CAL_CURRENT_OFFSET			0xEED4
/// Current gain (float)
#define VE_REG_BMV_CAL_CURRENT_GAIN				0xEED5
/// Calibrated fields (un16): calibrated if LSB is 0
#define VE_REG_BMV_CAL_CALIBRATED				0xEED6

/// @remark The next block is only used for the tester
/// Voltage offset (float)
#define VE_REG_BMV_CAL2_VOLTAGE_OFFSET			0xEEC0
/// Voltage gain (float)
#define VE_REG_BMV_CAL2_VOLTAGE_GAIN			0xEEC1
/// Voltage2 offset (float)
#define VE_REG_BMV_CAL2_VOLTAGE_2_OFFSET		0xEEC2
/// Voltage2 gain (float)
#define VE_REG_BMV_CAL2_VOLTAGE_2_GAIN			0xEEC3
/// Current offset (float)
#define VE_REG_BMV_CAL2_CURRENT_OFFSET			0xEEC4
/// Current gain (float)
#define VE_REG_BMV_CAL2_CURRENT_GAIN			0xEEC5
/// Calibrated fields (un16): calibrated if LSB is 0
#define VE_REG_BMV_CAL2_CALIBRATED				0xEEC6
/// Serial number (string32)
#define VE_REG_BMV_CAL2_SERIAL					0xEEC7

/// @remark The next block is only used for the tester
/// Voltage (float)
#define VE_REG_BMV_VOLTAGE						0xEEB0
/// Auxiliary voltage (float)
#define VE_REG_BMV_AUX_VOLTAGE					0xEEB1
/// Current (float)
#define VE_REG_BMV_CURRENT						0xEEB2
/// Consumed AH (float)
#define VE_REG_BMV_CAH							0xEEB3
/// SOC (float)
#define VE_REG_BMV_SOC							0xEEB4
/// TTG (float)
#define VE_REG_BMV_TTG							0xEEB5
/// Synchronized (un8): 0=not synced, 1=synced
#define VE_REG_BMV_SYNCHRONIZED					0xEEB6
/// Temperature (float) in Fahrenheit
#define VE_REG_BMV_TEMPERATURE					0xEEB7

/// @remark The next block is only used for the tester
/// ADC voltage counts (float)
#define VE_REG_BMV_ADC_VOLTAGE_COUNTS			0xEEA0
/// ADC auxiliary voltage counts (float)
#define VE_REG_BMV_ADC_AUX_VOLTAGE_COUNTS		0xEEA1
/// ADC current counts (float)
#define VE_REG_BMV_ADC_CURRENT_COUNTS			0xEEA2

/// @remark The next block is only used for the tester
/// LCD memory (14 bytes)
#define VE_REG_BMV_TST_LCD_MEM					0xEE90
/// Button state (un16) 0x0004=BUTTON_SETUP, 0x0008=BUTTON_SELECT, 0x0010=BUTTON_MINUS, 0x0020=BUTTON_PLUS
#define VE_REG_BMV_TST_BTN_STATE				0xEE91
/// Store calibration to flash
#define VE_REG_BMV_TST_STORE_CAL				0xEE92
/// Test relay (un8): 0=close, 1=open
#define VE_REG_BMV_TST_RELAY					0xEE93
/// Test backlight (un8): 0-9
#define VE_REG_BMV_TST_BACKLIGHT				0xEE94
/// Test buzzer (un8): 0=off, 1=on
#define VE_REG_BMV_TST_BUZZER					0xEE95
/// Test clock (un16): counts
#define VE_REG_BMV_TST_CLOCK					0xEE96
/// @}


/// @ingroup VELIB_VECAN_REG
/// @defgroup VELIB_VECAN_REG_TESTER Tester Specific Vregs
/// @{
/// Tester raw ADC value (un16 = raw adc value)
#define VE_REG_TST_ADC_GET_RAW					0xEEE0
/// Tester ADC calibration gain (datatype depends on device implementation)
#define VE_REG_TST_ADC_CAL_GAIN					0xEEC0
/// Tester ADC calibration offset (datatype depends on device implementation)
#define VE_REG_TST_ADC_CAL_OFFSET				0xEEA0
/// Tester measured value (datatype depends on device implementation)
#define VE_REG_TST_MEAS_GET						0xEE80
/// Tester execute command (un16 = command)
#define VE_REG_TST_TST_EXEC_CMD    				0xEE7D
/// @}


/// @ingroup VELIB_VECAN_REG
/// @defgroup VELIB_VECAN_REG_LYNX_ION Lynx Ion Vregs
/// @{
/// Lynx Ion battery flags (un32)
/// These should be high numbers or redefined...
#define VE_REG_LYNX_ION_FLAGS					0x0370

#define VE_VDATA_ION_FLG_CHARGED						1
#define VE_VDATA_ION_FLG_ALMOST_CHARGED					(1ul << 1)
#define VE_VDATA_ION_FLG_DISCHARGED						(1ul << 2)
#define VE_VDATA_ION_FLG_ALMOST_DISCHARGED				(1ul << 3)
#define VE_VDATA_ION_FLG_CHARGING						(1ul << 4)
#define VE_VDATA_ION_FLG_DISCHARGING					(1ul << 5)
#define VE_VDATA_ION_FLG_BALANCE						(1ul << 6)
#define VE_VDATA_ION_FLG_RELAY_DISCHARGED				(1ul << 7)
#define VE_VDATA_ION_FLG_RELAY_CHARGED					(1ul << 8)
#define VE_VDATA_ION_FLG_ALRM_OVER_VOLTAGE				(1ul << 9)
#define VE_VDATA_ION_FLG_WARN_OVER_VOLTAGE				(1ul << 10)
#define VE_VDATA_ION_FLG_ALRM_UNDER_VOLTAGE				(1ul << 11)
#define VE_VDATA_ION_FLG_WARN_UNDER_VOLTAGE				(1ul << 12)
#define VE_VDATA_ION_FLG_WARN_CHARGE_CURRENT			(1ul << 13)
#define VE_VDATA_ION_FLG_WARN_DISCHARGE_CURRENT			(1ul << 14)
#define VE_VDATA_ION_FLG_ALRM_OVER_TEMPERATURE			(1ul << 15)
#define VE_VDATA_ION_FLG_WARN_OVER_TEMPERATURE			(1ul << 16)
#define VE_VDATA_ION_FLG_WARN_UNDR_TEMPERATURE_CHRG		(1ul << 17)
#define VE_VDATA_ION_FLG_ALRM_UNDR_TEMPERATURE_CHRG		(1ul << 18)
#define VE_VDATA_ION_FLG_WARN_UNDR_TEMPERATURE_DCHRG	(1ul << 19)
#define VE_VDATA_ION_FLG_ALRM_UNDR_TEMPARATURE_DCHRG	(1ul << 20)
#define VE_VDATA_ION_FLG_LOW_SOC						(1ul << 21)

/// Lynx Ion BMS state (un8)
#define VE_REG_LYNX_ION_BMS_STATE 						0x0371

#define VE_VDATA_ION_BMS_STATE_WAIT_START				0
#define VE_VDATA_ION_BMS_STATE_BEFORE_BOOT				1
#define VE_VDATA_ION_BMS_STATE_BEFORE_BOOT_DELAY		2
#define VE_VDATA_ION_BMS_STATE_WAIT_BOOT				3
#define VE_VDATA_ION_BMS_STATE_INIT						4
#define VE_VDATA_ION_BMS_STATE_MEAS_BAT_VOLTAGE			5
#define VE_VDATA_ION_BMS_STATE_CALC_BAT_VOLTAGE			6
#define VE_VDATA_ION_BMS_STATE_WAIT_BUS_VOLTAGE			7
#define VE_VDATA_ION_BMS_STATE_WAIT_SHUNT				8
#define VE_VDATA_ION_BMS_STATE_RUNNING					9
#define VE_VDATA_ION_BMS_STATE_ERROR					10
#define VE_VDATA_ION_BMS_STATE_STANDBY					11

/// Lynx Ion BMS error flags (un32)
#define VE_REG_LYNX_ION_BMS_ERROR_FLAGS					0x0372

#define VE_VDATA_ION_BMS_ERROR_NONE 					0	/* Note that this is a value, not a bit mask. */
#define VE_VDATA_ION_BMS_ERROR_RESERVED_1 				(1ul << 0)
#define VE_VDATA_ION_BMS_ERROR_BAT_INIT 				(1ul << 1)
#define VE_VDATA_ION_BMS_ERROR_NO_BAT 					(1ul << 2)
#define VE_VDATA_ION_BMS_ERROR_UNKNOWN_PC				(1ul << 3)
#define VE_VDATA_ION_BMS_ERROR_BAT_TYPE					(1ul << 4)
#define VE_VDATA_ION_BMS_ERROR_NR_OF_BAT				(1ul << 5)
#define VE_VDATA_ION_BMS_ERROR_NO_SHUNT_FND				(1ul << 6)
#define VE_VDATA_ION_BMS_ERROR_MEASURE					(1ul << 7)
#define VE_VDATA_ION_BMS_ERROR_CALCULATE				(1ul << 8)
#define VE_VDATA_ION_BMS_ERROR_BAT_NR_SER				(1ul << 9)
#define VE_VDATA_ION_BMS_ERROR_BAT_NR					(1ul << 10)
#define VE_VDATA_ION_BMS_ERROR_HW						(1ul << 11)
#define VE_VDATA_ION_BMS_ERROR_HW_WDT					(1ul << 12)
#define VE_VDATA_ION_BMS_ERROR_OV						(1ul << 13)
#define VE_VDATA_ION_BMS_ERROR_UV						(1ul << 14)
#define VE_VDATA_ION_BMS_ERROR_OTEMP					(1ul << 15)
#define VE_VDATA_ION_BMS_ERROR_UTEMP					(1ul << 16)
#define VE_VDATA_ION_BMS_ERROR_HW_CHARGE_PLUG			(1ul << 17)
#define VE_VDATA_ION_BMS_ERROR_HW_CHARGE				(1ul << 18)
#define VE_VDATA_ION_BMS_ERROR_HW_PRE_CHARGE_CHARGE		(1ul << 19)
#define VE_VDATA_ION_BMS_ERROR_HW_DISCHARGE				(1ul << 20)
#define VE_VDATA_ION_BMS_ERROR_HW_PRE_CHARGE_DISCHARGE	(1ul << 21)
#define VE_VDATA_ION_BMS_ERROR_HW_ADC_DATA				(1ul << 22)
#define VE_VDATA_ION_BMS_ERROR_SLAVE					(1ul << 23)
#define VE_VDATA_ION_BMS_ERROR_SLAVE_WARNING			(1ul << 24)
#define VE_VDATA_ION_BMS_ERROR_HW_PRE_CHARGE_CONTACTOR	(1ul << 25)
#define VE_VDATA_ION_BMS_ERROR_HW_CONTACTOR				(1ul << 26)
#define VE_VDATA_ION_BMS_ERROR_OUTPUT_OC				(1ul << 27)
#define VE_VDATA_ION_BMS_STATUS_SHUTDOWN				(1ul << 28)
#define VE_VDATA_ION_BMS_STATUS_INIT					(1ul << 29)
#define VE_VDATA_ION_BMS_STATUS_BALANCE					(1ul << 30)
#define VE_VDATA_ION_BMS_STATUS_RUN						(1ul << 31)


/// Lynx Ion consumed AH (same as VE_REG_BMV_CE)
#define VE_REG_LYNX_ION_CE						0xEEFF
/// Lynx Ion last errors [err0 (un8)][err1 (un8)][err2 (un8)][err3 (un8)]
#define VE_REG_LYNX_ION_LAST_ERROR				0xEEFE
/// Lynx Ion battery extended flags (un32)
#define VE_REG_LYNX_ION_FLAGS_EXT				0xEEFD
/// Lynx Ion number of shutdowns caused by error (un16)
#define VE_REG_LYNX_ION_SHUTDOWNS_DUE_ERROR		0xEEFC
/// @}

/// @ingroup VELIB_VECAN_REG
/// @defgroup VELIB_VECAN_REG_LYNX_ION Lynx Ion Vregs
/// @{
	/// Internal temperature. (un16 0.01K)
#define VE_REG_LYNX_SHUNT_INTERNAL_TEMPERATURE	0xEE60

/// Use to send NACK on CAN bus to indicate invalid calibration values (no data)
#define VE_REG_LYNX_SHUNT_CALIBRATION			0xEE61
/// @}


/// @ingroup VELIB_VECAN_REG
/// @defgroup VELIB_VECAN_REG_SCB Skylla-i Control Board Vregs
/// @{
/// Tester rotary switch value (un8 = value)
/// @remark value: bit mask B7=update,B6=equalize,B5=remote,B4=power,B3..B0=rotary encoder
#define VE_REG_SCB_TST_GET_ROTARY  				0xEE7F
/// Tester dip-switch value (un8 = value)
/// @remark value: bit mask B5..B0=dip switches 1..6
#define VE_REG_SCB_TST_GET_DIPSW   				0xEE7E
/// Tester dio pin value (un8 = value)
/// @remark value: B1=control, B0=mains
#define VE_REG_SCB_TST_GET_DIO   				0xEE7C
/*
 * VE_REG_TST_TST_EXEC_CMD	- 0xEE7D
 *
 *	un16 command
 */
#define VE_REG_SCB_TST_CMD_INIT_CAL			0xF000 //Initialise factory calibration (reset to defaults)
#define VE_REG_SCB_TST_CMD_RELEASE			0xEF00 //Release set-points to application
#define VE_REG_SCB_TST_CMD_SET_RELAY		0xEE00 //LOW BYTE: 0=OFF, 1=ON
#define VE_REG_SCB_TST_CMD_SET_LEDS			0xED00 //LOW BYTE: 0=OFF, 1=ON (all)
#define VE_REG_SCB_TST_CMD_SET_DISPLAY		0xED00 //LOW BYTE: 0=OFF, 1=ON (all)
#define VE_REG_SCB_TST_CMD_SET_BUZZER		0xEC00 //LOW BYTE: 0=OFF, 1=ON
#define VE_REG_SCB_TST_CMD_SET_FAN			0xEB00 //LOW BYTE: SPEED (0=OFF)
#define VE_REG_SCB_TST_CMD_SET_ISET			0xEA00 //LOW BYTE: VALUE (0=OFF)
#define VE_REG_SCB_TST_CMD_SET_BACKLIGHT	0xE900 //LOW BYTE: 0=OFF, 1=ON
#define VE_REG_SCB_TST_CMD_SET_VSET			0xE800 //LOW BYTE: VALUE (0=OFF)
#define VE_REG_SCB_TST_CMD_SET_REM			0xE700 //LOW BYTE: VALUE (0=OFF)
/// @}


/// @ingroup VELIB_VECAN_REG
/// @defgroup VELIB_VECAN_REG_SRP Skylla-i Remote Panel Vregs
/// @{
/// Tester switch value (un16 = value)
/// @remark value: B2=setup (rear),B1=power,B0=encoder (push)
#define VE_REG_SRP_TST_GET_SWITCH				0xEE7F
/// Tester potmeter/encoder value (un16 = value)
#define VE_REG_SRP_TST_GET_POT					0xEE7E
/// Tester ambient light value (un16 = value)
#define VE_REG_SRP_TST_GET_LDR					0xEE7C
/*
 * VE_REG_TST_TST_EXEC_CMD	- 0xEE7D
 *
 *	un16 command
 */
#define VE_REG_SRP_TST_CMD_RELEASE			0xEF00 //Release set-points to application
#define VE_REG_SRP_TST_CMD_SET_LEDS			0xED00 //LOW BYTE: 0=OFF, 1=ON (all)
#define VE_REG_SRP_TST_CMD_SET_DISPLAY		0xEC00 //LOW BYTE: 0=OFF, 1=ON
/// @}


/// @ingroup VELIB_VECAN_REG
/// @defgroup VELIB_VECAN_REG_MPPT Blue Solar MPPT Vregs
/// @{
/// Tester keypad analog scan value (un8)
#define VE_REG_MPPT_TST_GET_KEYBRD       		0xEE7F
/// Tester potmeter/encoder value (un8)
#define VE_REG_MPPT_TST_GET_DIO					0xEE7E
/*
 * VE_REG_TST_TST_EXEC_CMD	- 0xEE7D
 *
 *	un16 command
 */
#define VE_REG_MPPT_TST_CMD_INIT_CAL		0xF000 //Initialise factory calibration (reset to defaults)
#define VE_REG_MPPT_TST_CMD_RELEASE			0xEF00 //Release set-points to application
#define VE_REG_MPPT_TST_CMD_SET_RELAY		0xEE00 //LOW BYTE: 0=OFF, 1=ON
#define VE_REG_MPPT_TST_CMD_SET_DISPLAY		0xED00 //LOW BYTE: 0=OFF, 1=ON (all)
#define VE_REG_MPPT_TST_CMD_SET_BUZZER		0xEC00 //LOW BYTE: 0=OFF, 1=ON
#define VE_REG_MPPT_TST_CMD_SET_FAN			0xEB00 //LOW BYTE: SPEED (0=OFF)
#define VE_REG_MPPT_TST_CMD_SET_PWM1		0xEA00 //LOW BYTE: VALUE (0=OFF)
#define VE_REG_MPPT_TST_CMD_SET_BACKLIGHT	0xE900 //LOW BYTE: 0=OFF, 1=ON
#define VE_REG_MPPT_TST_CMD_SET_PWM2		0xE800 //LOW BYTE: VALUE (0=OFF)
#define VE_REG_MPPT_TST_CMD_SET_MODE		0xE700 //LOW BYTE: VALUE (0=DISCONTINUOUS,1=CONTINUOUS)
#define VE_REG_MPPT_TST_CMD_SET_PVSHORT		0xE600 //LOW BYTE: VALUE (0=OPEN,1=SHORT)

/// @ingroup VELIB_VECAN_REG
/// @defgroup VELIB_VECAN_REG_VEBUS Vebus vregs
/// @{
/**
 * Read / write. Reports / sets the maximum charge current in 0.1A. (un16)
 * @note the unit is to be compatible with the charger settings in 0xED.
 * Vebus actually uses 1A.
 */
#define VE_REG_MK2CAN_MAX_CHARGE_CURRENT 0xEEFE

/// Reports the vebus error as an un8.
#define VE_REG_MK2CAN_VEBUS_ERROR 0xEEFF
/// @}

/// @}

#endif





