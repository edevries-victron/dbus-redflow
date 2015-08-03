#ifndef _VELIB_VECAN_CHARGER_ERROR_H_
#define _VELIB_VECAN_CHARGER_ERROR_H_

#define ERROR_DB_CHARGERS	 0x00					/*!< Condition / Description */

#define CHARGER_ERROR_NONE						0	/*!< No error */

#define CHARGER_ERROR_BATTERY_TEMP_TOO_HIGH		1	/*!< Error   / Battery temperature too high */
#define CHARGER_ERROR_BATTERY_VOLTAGE_TOO_HIGH	2	/*!< Error   / Battery voltage too high */
#define CHARGER_ERROR_BATTERY_TSENSE_PLUS_HIGH	3	/*!< Error   / Battery temperature sensor miswired */
#define CHARGER_ERROR_BATTERY_TSENSE_PLUS_LOW	4	/*!< Error   / Battery temperature sensor miswired */
#define CHARGER_ERROR_BATTERY_TSENSE_CONN_LOST	5	/*!< Error   / Battery temperature sensor connection lost */
#define CHARGER_ERROR_BATTERY_VSENSE_PLUS_LOW	6	/*!< Error   / Battery voltage sense miswired */
#define CHARGER_ERROR_BATTERY_VSENSE_MIN_HIGH	7	/*!< Error   / Battery voltage sense miswired */
#define CHARGER_ERROR_BATTERY_VSENSE_CONN_LOST	8	/*!< Error   / Battery voltage sense connection lost */
#define CHARGER_ERROR_BATTERY_VSENSE_LOSSES		9	/*!< Error   / Battery voltage wire losses too high */

#define CHARGER_ERROR_CHARGER_TEMP_TOO_HIGH		17	/*!< Error   / Charger temperature too high */
#define CHARGER_ERROR_CHARGER_OVER_CURRENT		18	/*!< Error   / Charger excessive current */
#define CHARGER_ERROR_CHARGER_CURRENT_REVERSED	19	/*!< Error   / Changer current polarity reversed */
#define CHARGER_ERROR_CHARGER_BULKTIME_EXPIRED	20	/*!< Error   / Charger bulk time expired */
#define CHARGER_ERROR_CHARGER_CURRENT_SENSE		21	/*!< Error   / Charger current sensor issue */
#define CHARGER_ERROR_CHARGER_TSENSE_SHORT		22	/*!< Error   / Charger temperature sensor miswired */
#define CHARGER_ERROR_CHARGER_TSENSE_CONN_LOST	23	/*!< Error   / Charger temperature sensor connection lost */
#define CHARGER_ERROR_CHARGER_FAN_MISSING		24	/*!< Error   / Charger internal fan not detected */
#define CHARGER_ERROR_CHARGER_FAN_OVER_CURRENT	25	/*!< Error   / Charger internal fan over-current */
#define CHARGER_ERROR_CHARGER_TERMINAL_OVERHEAT	26	/*!< Error   / Charger terminal overheated */
#define CHARGER_ERROR_CHARGER_SHORT_CIRCUIT		27	/*!< Error   / Charger short circuit */

#define CHARGER_ERROR_INPUT_VOLTAGE_TOO_HIGH	33	/*!< Error   / Input voltage too high */
#define CHARGER_ERROR_INPUT_OVER_CURRENT		34	/*!< Error   / Input excessive current */
#define CHARGER_ERROR_INPUT_OVER_POWER			35	/*!< Error   / Input excessive power */
#define CHARGER_ERROR_INPUT_POLARITY			36	/*!< Error   / Input polarity issue */
#define CHARGER_ERROR_INPUT_VOLTAGE_ABSENT      37  /*!< Error   / Input voltage absent (mains removed, fuse blown?) */
#define CHARGER_ERROR_INPUT_SHUTDOWN			38  /*!< Error   / Input shutdown (converter broken, short input to avoid battery overcharge) */

#define CHARGER_ERROR_LOAD_TEMP_TOO_HIGH		49	/*!< Error   / Load temperature too high */
#define CHARGER_ERROR_LOAD_OVER_VOLTAGE			50	/*!< Error   / Load voltage too high */
#define CHARGER_ERROR_LOAD_OVER_CURRENT			51	/*!< Error   / Load excessive current */
#define CHARGER_ERROR_LOAD_CURRENT_REVERSED		52	/*!< Error   / Load current polarity reversed */
#define CHARGER_ERROR_LOAD_OVER_POWER			53	/*!< Error   / Load excessive power */

#define CHARGER_ERROR_LINK_DEVICE_MISSING		65	/*!< Warning / Device disappeared during parallel operation (broken cable?) */
#define CHARGER_ERROR_LINK_CONFIGURATION		66	/*!< Warning / Incompatible device encountered for parallel operation (e.g. old firmware/different settings) */
#define CHARGER_ERROR_LINK_BMS_MISSING			67	/*!< Error   / BMS connection lost */

#define CHARGER_ERROR_MEMORY_WRITE_FAILURE		113	/*!< Error   / Non-volatile storage write error */
#define CHARGER_ERROR_CPU_TEMP_TOO_HIGH			114	/*!< Error   / CPU temperature too high */
#define CHARGER_ERROR_COMMUNICATION_LOST		115	/*!< Error   / CAN/SCI communication lost (when critical) */
#define CHARGER_ERROR_CALIBRATION_DATA_LOST		116	/*!< Error   / Non-volatile calibration data lost */
#define CHARGER_ERROR_INVALID_FIRMWARE			117	/*!< Error   / Incompatible firmware encountered */
#define CHARGER_ERROR_INVALID_HARDWARE			118	/*!< Error   / Incompatible hardware encountered */
#define CHARGER_ERROR_SETTINGS_DATA_INVALID		119	/*!< Error   / Non-volatile settings data invalid/corrupted */

#endif
