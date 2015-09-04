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
public:
	BatteryControllerSettings(int deviceType, const QString &serial, QObject *parent = 0);

	int deviceType() const;

	QString serial() const;

	QString customName() const;

	void setCustomName(const QString &n);

signals:
	void customNameChanged();

private:
	int mDeviceType;
	QString mSerial;
	QString mCustomName;
};

#endif // BATTERY_CONTROLLER_SETTINGS_H
