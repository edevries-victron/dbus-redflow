#include <cmath>
#include <velib/vecan/products.h>
#include "battery_summary.h"
#include "battery_summary_bridge.h"

Q_DECLARE_METATYPE(QList<int>)

BatterySummaryBridge::BatterySummaryBridge(BatterySummary *summary,
										   QObject *parent):
	DBusBridge("com.victronenergy.battery.zbm", parent)
{
	// The D-Bus paths /Mgmt/Connection, /ProductName, and /Connected are used
	// by system-calc to determine whether a service is connected.
	// /Connected must be 1. /Mgmt/Connection and /ProductName must exist and
	// be valid.
	produce("/Mgmt/Connection", "Modbus");
	produce("/ProductName", "ZBM Summary");
	produce("/Connected", 1);
	produce("/ProductId", VE_PROD_ID_REDFLOW_ZBM2);
	produce("/DeviceInstance", 40);

	produce(summary, "zbmCount", "/ZbmCount");
	produce(summary, "averageVoltage", "/Dc/0/Voltage", "V", 1);
	produce(summary, "totalCurrent", "/Dc/0/Current", "A", 1);
	produce(summary, "totalPower", "/Dc/0/Power", "W", 0);
	produce(summary, "averageStateOfCharge", "/Soc", "%", 0);
	produce(summary, "operationalMode", "/OperationalMode");
	produce(summary, "requestClearStatusRegister", "/ClearStatusRegisterFlags");
	produce(summary, "requestImmediateSelfMaintenance", "/RequestImmediateSelfMaintenance");
	produce(summary, "requestDelayedSelfMaintenance", "/RequestDelayedSelfMaintenance");
	produce(summary, "maintenanceActive", "/Alarms/MaintenanceActive");
	produce(summary, "maintenanceNeeded", "/Alarms/Maintenance");
	produce(summary, "deviceAddresses", "/DeviceAddresses");
}

bool BatterySummaryBridge::toDBus(const QString &path, QVariant &value)
{
	if (path == "/DeviceAddresses") {
		QString s;
		foreach (int addr, qvariant_cast<QList<int> >(value)) {
			if (!s.isEmpty())
				s += ',';
			s += QString::number(addr);
		}
		value = s;
	} else if (value.type() == QVariant::Double && !std::isfinite(value.toDouble())) {
		value = QVariant();
	}
	return true;
}
