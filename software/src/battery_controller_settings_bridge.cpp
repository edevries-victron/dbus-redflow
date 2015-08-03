#include <QDBusConnection>
#include <QDBusVariant>
#include <velib/qt/v_busitems.h>
#include "battery_controller_settings.h"
#include "battery_controller_settings_bridge.h"

static const QString Service = "com.victronenergy.settings";

BatteryControllerSettingsBridge::BatteryControllerSettingsBridge(
	BatteryControllerSettings *settings, QObject *parent) :
	DBusBridge(parent)
{
	/*
	QString path = QString("/Settings/Redflow/Devices/D%1").
				   arg(settings->serial());
	consume(Service, settings, "deviceType", QVariant(settings->deviceType()),
			path + "/DeviceType");
	consume(Service, settings, "customName", QVariant(""),
			path + "/CustomName");
	consume(Service, settings, "serviceType", QVariant("grid"),
			path + "/ServiceType");
	consume(Service, settings, "isMultiPhase",
			QVariant(static_cast<int>(settings->isMultiPhase())),
			path + "/IsMultiPhase");
	consume(Service, settings, "hub4Mode", QVariant(0),
			path + "/Hub4Mode");
	consume(Service, settings, "position", QVariant(0),
			path + "/Position");
	consume(Service, settings, "l1ReverseEnergy", 0.0, 0.0, 1e6,
			path + "/L1ReverseEnergy");
	consume(Service, settings, "l2ReverseEnergy", 0.0, 0.0, 1e6,
			path + "/L2ReverseEnergy");
	consume(Service, settings, "l3ReverseEnergy", 0.0, 0.0, 1e6,
			path + "/L3ReverseEnergy");
	*/
}

bool BatteryControllerSettingsBridge::toDBus(const QString &path, QVariant &v)
{

	return true;
}

bool BatteryControllerSettingsBridge::fromDBus(const QString &path, QVariant &v)
{

	return true;
}
