#include <velib/qt/v_busitem.h>
#include <velib/vecan/products.h>
#include "batteryController.h"
#include "battery_summary.h"

static const QString ServiceName = "com.victronenergy.battery.zbm";

BatterySummary::BatterySummary(QObject *parent):
	AbstractMonitorService(ServiceName, parent),
	mZbmCount(0),
	mVoltage(0),
	mCurrent(0),
	mPower(0),
	mTemperature(0),
	mSoc(0),
	mClearStatusRegister(0),
	mOperationalMode(0),
	mDelayedSelfMaintenance(0),
	mImmediateSelfMaintenance(0),
	mMaintenanceActive(0),
	mMaintenanceNeeded(0)
{
	// The D-Bus paths /Mgmt/Connection, /ProductName, and /Connected are used
	// by system-calc to determine whether a service is connected.
	// /Connected must be 1. /Mgmt/Connection and /ProductName must exist and
	// be valid.
	produce("/Mgmt/Connection", "Modbus");
	produce("/ProductName", "Redflow ZBM");
	produce("/Connected", 1);
	produce("/ProductId", VE_PROD_ID_REDFLOW_ZBM2);
	produce("/DeviceInstance", 40);

	mZbmCount = produce("/ZbmCount", QVariant(), "", 0);
	mVoltage = produce("/Dc/0/Voltage", QVariant(), "V", 1);
	mCurrent = produce("/Dc/0/Current", QVariant(), "A", 1);
	mPower = produce("/Dc/0/Power", QVariant(), "W", 0);
	mTemperature = produce("/Dc/0/Temperature", QVariant(), "C", 1);
	mSoc = produce("/Soc", QVariant(), "%", 0);
	mClearStatusRegister = produce("/ClearStatusRegisterFlags", QVariant());
	mDelayedSelfMaintenance = produce("/RequestImmediateSelfMaintenance", QVariant());
	mImmediateSelfMaintenance = produce("/RequestDelayedSelfMaintenance", QVariant());
	mOperationalMode = produce("/OperationalMode", -1);
	mMaintenanceActive = produce("/Alarms/MaintenanceActive", QVariant());
	mMaintenanceNeeded = produce("/Alarms/Maintenance", QVariant());
}

void BatterySummary::updateValues()
{
	double vTot = 0;
	int vCount = 0;
	double iTot	= 0;
	double pTot = 0;
	double tMax = 0;
	double socTot = 0;
	int socCount = 0;
	bool maintenanceNeeded = true;
	bool maintenanceActive = true;
	foreach (BatteryController *bc, controllers()) {
		if (bc->BattVolts() > 0) {
			vTot += bc->BattVolts();
			++vCount;
		}
		iTot += bc->BattAmps();
		pTot += bc->BattPower();
		tMax = qMax(tMax, bc->BattTemp());
		socTot += bc->SOC();
		++socCount;
		QVariant userOp = mOperationalMode->getValue();
		if (userOp != -1)
			bc->setOperationalMode(userOp.toInt());
		// We set maintenanceNeeded and maintenanceActive to false if any
		// battery has not been put in maintenance mode yet. We do this, so
		// the GUI can use these values to force all remaining batteries into
		// into maintance mode.
		maintenanceNeeded = maintenanceNeeded && bc->maintenanceAlarm() != 0;
		maintenanceActive = maintenanceActive && bc->maintenanceActiveAlarm() != 0;
		if (mClearStatusRegister->getValue() == 1)
			bc->setClearStatusRegisterFlags(1);
		if (mDelayedSelfMaintenance->getValue() == 1)
			bc->setRequestDelayedSelfMaintenance(1);
		if (mImmediateSelfMaintenance->getValue() == 1)
			bc->setRequestImmediateSelfMaintenance(1);
	}
	int count = controllers().size();

	mZbmCount->setValue(controllers().size());
	mVoltage->setValue(vCount == 0 ? QVariant() : vTot / vCount);
	mCurrent->setValue(vCount  == 0 ? QVariant() : iTot);
	mPower->setValue(vCount == 0 ? QVariant() : pTot);
	mTemperature->setValue(count == 0 ? QVariant() : tMax);
	mSoc->setValue(socCount == 0 ? QVariant() : socTot / socCount);

	mOperationalMode->setValue(-1);
	mClearStatusRegister->setValue(0);
	mDelayedSelfMaintenance->setValue(0);
	mImmediateSelfMaintenance->setValue(0);

	mMaintenanceNeeded->setValue(maintenanceNeeded && count > 0 ? 1 : 0);
	mMaintenanceActive->setValue(maintenanceActive && count > 0 ? 1 : 0);
}
