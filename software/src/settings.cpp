#include "settings.h"

Settings::Settings(QObject *parent) :
	QObject(parent)
{
}

const QStringList &Settings::deviceIds() const
{
	return mDeviceIds;
}

void Settings::setDeviceIds(const QStringList &deviceIds)
{
	if (mDeviceIds == deviceIds)
		return;
	mDeviceIds = deviceIds;
	emit deviceIdsChanged();
}

void Settings::registerDevice(const QString &serial)
{
	if (mDeviceIds.contains(serial))
		return;
	mDeviceIds.append(serial);
	emit deviceIdsChanged();
}
