#include <QStringList>
#include <QVariant>
#include "settings.h"
#include "settings_bridge.h"

static const QString Service = "com.victronenergy.settings";
static const QString DeviceIdsPath = "/Settings/Redflow/DeviceIds";
static const QString AcPowerSetPointPath = "/Settings/Redflow/AcPowerSetPoint";

SettingsBridge::SettingsBridge(Settings *settings, QObject *parent):
	DBusBridge(parent)
{
	consume(Service, settings, "deviceIds", QVariant(""), DeviceIdsPath);
	//consume(Service, settings, "acPowerSetPoint", 0.0, -1e5, 1e5, AcPowerSetPointPath);
}

bool SettingsBridge::toDBus(const QString &path, QVariant &value)
{
	if (path == DeviceIdsPath) {
		value = value.value<QStringList>().join(",");
	}
	return true;
}

bool SettingsBridge::fromDBus(const QString &path, QVariant &value)
{
	if (path == DeviceIdsPath) {
		value = value.toString().split(',', QString::SkipEmptyParts);
	}
	return true;
}
