#include <velib/qt/charger_error.h>
#include <velib/vecan/charger_error.h>

/* Otherwise we have to include/create velib_config_app.h */
#ifndef ARRAY_LENGTH
# define ARRAY_LENGTH(a)	(sizeof(a) / sizeof(a[0]))
#endif

const VeChargerError ChargerError::errors[] =
{
	{ CHARGER_ERROR_NONE,						QT_TR_NOOP("No error") },

	{ CHARGER_ERROR_BATTERY_TEMP_TOO_HIGH,		QT_TR_NOOP("Battery high temperature") },
	{ CHARGER_ERROR_BATTERY_VOLTAGE_TOO_HIGH,	QT_TR_NOOP("Battery high voltage") },
	{ CHARGER_ERROR_BATTERY_TSENSE_PLUS_HIGH,	QT_TR_NOOP("Battery Tsense miswired") },
	{ CHARGER_ERROR_BATTERY_TSENSE_PLUS_LOW,	QT_TR_NOOP("Battery Tsense miswired") },
	{ CHARGER_ERROR_BATTERY_TSENSE_CONN_LOST,	QT_TR_NOOP("Battery Tsense missing") },
	{ CHARGER_ERROR_BATTERY_VSENSE_PLUS_LOW,	QT_TR_NOOP("Battery Vsense miswired") },
	{ CHARGER_ERROR_BATTERY_VSENSE_MIN_HIGH,	QT_TR_NOOP("Battery Vsense miswired") },
	{ CHARGER_ERROR_BATTERY_VSENSE_CONN_LOST,	QT_TR_NOOP("Battery Vsense missing") },
	{ CHARGER_ERROR_BATTERY_VSENSE_LOSSES,		QT_TR_NOOP("Battery high wire losses") },

	{ CHARGER_ERROR_CHARGER_TEMP_TOO_HIGH,		QT_TR_NOOP("Charger high temperature") },
	{ CHARGER_ERROR_CHARGER_OVER_CURRENT,		QT_TR_NOOP("Charger excessive current") },
	{ CHARGER_ERROR_CHARGER_CURRENT_REVERSED,	QT_TR_NOOP("Charger negative current") },
	{ CHARGER_ERROR_CHARGER_BULKTIME_EXPIRED,	QT_TR_NOOP("Charger bulk time expired") },
	{ CHARGER_ERROR_CHARGER_CURRENT_SENSE,		QT_TR_NOOP("Charger current sensor issue") },
	{ CHARGER_ERROR_CHARGER_TSENSE_SHORT,		QT_TR_NOOP("Internal Tsensor miswired") },
	{ CHARGER_ERROR_CHARGER_TSENSE_CONN_LOST,	QT_TR_NOOP("Internal Tsensor missing") },
	{ CHARGER_ERROR_CHARGER_FAN_MISSING,		QT_TR_NOOP("Charger fan not detected") },
	{ CHARGER_ERROR_CHARGER_FAN_OVER_CURRENT,	QT_TR_NOOP("Charger fan over-current") },
	{ CHARGER_ERROR_CHARGER_TERMINAL_OVERHEAT,	QT_TR_NOOP("Charger terminal overheat") },
	{ CHARGER_ERROR_CHARGER_SHORT_CIRCUIT,		QT_TR_NOOP("Charger short circuit") },

	{ CHARGER_ERROR_INPUT_VOLTAGE_TOO_HIGH,		QT_TR_NOOP("Input high voltage") },
	{ CHARGER_ERROR_INPUT_OVER_CURRENT,			QT_TR_NOOP("Input excessive current") },
	{ CHARGER_ERROR_INPUT_OVER_POWER,			QT_TR_NOOP("Input excessive power") },
	{ CHARGER_ERROR_INPUT_POLARITY,				QT_TR_NOOP("Input polarity issue") },
	{ CHARGER_ERROR_INPUT_VOLTAGE_ABSENT,		QT_TR_NOOP("Input voltage absent") },
	{ CHARGER_ERROR_INPUT_SHUTDOWN,				QT_TR_NOOP("Input shutdown") },

	{ CHARGER_ERROR_LOAD_TEMP_TOO_HIGH,			QT_TR_NOOP("Load high temperature") },
	{ CHARGER_ERROR_LOAD_OVER_VOLTAGE,			QT_TR_NOOP("Load high voltage") },
	{ CHARGER_ERROR_LOAD_OVER_CURRENT,			QT_TR_NOOP("Load excessive current") },
	{ CHARGER_ERROR_LOAD_CURRENT_REVERSED,		QT_TR_NOOP("Load negative current") },
	{ CHARGER_ERROR_LOAD_OVER_POWER,			QT_TR_NOOP("Load excessive power") },

	{ CHARGER_ERROR_LINK_DEVICE_MISSING,		QT_TR_NOOP("Device disappeared") },
	{ CHARGER_ERROR_LINK_CONFIGURATION,			QT_TR_NOOP("Incompatible device") },
	{ CHARGER_ERROR_LINK_BMS_MISSING,			QT_TR_NOOP("BMS connection lost") },

	{ CHARGER_ERROR_MEMORY_WRITE_FAILURE,		QT_TR_NOOP("Memory write error") },
	{ CHARGER_ERROR_CPU_TEMP_TOO_HIGH,			QT_TR_NOOP("CPU temperature too high") },
	{ CHARGER_ERROR_COMMUNICATION_LOST,			QT_TR_NOOP("Communication lost") },
	{ CHARGER_ERROR_CALIBRATION_DATA_LOST,		QT_TR_NOOP("Calibration data lost") },
	{ CHARGER_ERROR_INVALID_FIRMWARE,			QT_TR_NOOP("Incompatible firmware") },
	{ CHARGER_ERROR_INVALID_HARDWARE,			QT_TR_NOOP("Incompatible hardware") },
	{ CHARGER_ERROR_SETTINGS_DATA_INVALID,		QT_TR_NOOP("Settings invalid") }
};

ChargerError::ChargerError()
{
}

QString ChargerError::getDescription(int error)
{
	QString description = "#" + QString::number(error) + " ";

	size_t i;
	for (i = 0; i < ARRAY_LENGTH(errors); i++) {
		if (errors[i].errorId == error)
			description += tr(errors[i].description);
	}

	return description;
}

bool ChargerError::isWarning(int error)
{
	return error == CHARGER_ERROR_LINK_DEVICE_MISSING || error == CHARGER_ERROR_LINK_CONFIGURATION;
}
