#ifndef BATTERY_CONTROLLER_SETTINGS_H
#define BATTERY_CONTROLLER_SETTINGS_H

#include <QMetaType>
#include <QObject>
#include "defines.h"


class BatteryControllerSettings : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int deviceType READ deviceType)
	Q_PROPERTY(QString serial READ serial)
	Q_PROPERTY(QString customName READ customName WRITE setCustomName NOTIFY customNameChanged)
	Q_PROPERTY(QString serviceType READ serviceType WRITE setServiceType NOTIFY serviceTypeChanged)

public:
	BatteryControllerSettings(int deviceType, const QString &serial, QObject *parent = 0);

	int deviceType() const;

	QString serial() const;

	QString customName() const;

	void setCustomName(const QString &n);

	/*!
	 * This string is used to create the D-Bus service.
	 * D-Bus service name will be 'com.victronenergy.[serviceName]'. So
	 * `serviceType` tells us where the device is installed (ie. which power
	 * stream is measured).
	 * Typical values are 'grid', 'genset' (for generator), and 'pvinverter'.
	 */
	QString serviceType() const;

	void setServiceType(const QString &t);

signals:
	void customNameChanged();

	void serviceTypeChanged();


private:
	int mDeviceType;
	QString mSerial;
	QString mCustomName;
	QString mServiceType;
};

#endif // BATTERY_CONTROLLER_SETTINGS_H
