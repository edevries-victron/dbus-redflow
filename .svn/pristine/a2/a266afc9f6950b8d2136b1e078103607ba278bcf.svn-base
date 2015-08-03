#include <QsLog.h>
#include "battery_controller_settings.h"

BatteryControllerSettings::BatteryControllerSettings(int deviceType, const QString &serial,
										 QObject *parent) :
	QObject(parent),
	mDeviceType(deviceType),
	mSerial(serial)
{
}

int BatteryControllerSettings::deviceType() const
{
	return mDeviceType;
}

QString BatteryControllerSettings::serial() const
{
	return mSerial;
}

QString BatteryControllerSettings::customName() const
{
	return mCustomName;
}

void BatteryControllerSettings::setCustomName(const QString &n)
{
	if (mCustomName == n)
		return;
	mCustomName	= n;
	emit customNameChanged();
}

QString BatteryControllerSettings::serviceType() const
{
	return mServiceType;
}

void BatteryControllerSettings::setServiceType(const QString &t)
{
	if (mServiceType == t)
		return;
	mServiceType = t;
	emit serviceTypeChanged();
}

