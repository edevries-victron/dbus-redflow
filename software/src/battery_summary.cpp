#include <QTimer>
#include <velib/qt/v_busitem.h>
#include <velib/qt/ve_qitem.hpp>
#include "battery_controller.h"
#include "battery_summary.h"

// static const QString ServiceName = "com.victronenergy.battery.zbm";

BatterySummary::BatterySummary(VeQItem *root, QObject *parent):
	AbstractMonitorService(root, parent),
	mVoltage(0),
	mCurrent(0),
	mPower(0),
	mSoc(0),
	mOperationalMode(0),
	mRequestClearStatusRegister(0),
	mRequestDelayedSelfMaintenance(0),
	mRequestImmediateSelfMaintenance(0),
	mMaintenanceActive(0),
	mMaintenanceNeeded(0)
{
	produce("/DeviceInstance", 40);
	produce("/Connected", 1);
	produce("/Mgmt/Connection", "modbus");
	produce("/ProductName", "ZBM Summary");
	produce("/ProductId", 45059); // @todo EV get from velib

	mDeviceAddresses = produce("/DeviceAddresses", QVariant());
	mVoltage = produce("/Dc/0/Voltage", QVariant(), "V", 1);
	mCurrent = produce("/Dc/0/Current", QVariant(), "A", 1);
	mPower = produce("/Dc/0/Power", QVariant(), "W", 0);
	mSoc = produce("/Soc", QVariant(), "%", 0);
	mOperationalMode = produce("/OperationalMode", QVariant());
	mRequestClearStatusRegister = produce("/ClearStatusRegisterFlags", QVariant());
	mRequestDelayedSelfMaintenance = produce("/RequestDelayedSelfMaintenance", QVariant());
	mRequestImmediateSelfMaintenance = produce("/RequestImmediateSelfMaintenance", QVariant());
	mMaintenanceActive = produce("/Alarms/MaintenanceActive", QVariant());
	mMaintenanceNeeded = produce("/Alarms/MaintenanceNeeded", QVariant());
}

QList<int> BatterySummary::deviceAddresses() const
{
	QList<int> result;
	foreach (BatteryController *bc, mControllers) {
		int address = bc->DeviceAddress();
		if (!result.contains(address))
			result.append(address);
	}
	return result;
}

void BatterySummary::addBattery(BatteryController *c)
{
	if (mControllers.contains(c))
		return;
	mControllers.append(c);
	connect(c, SIGNAL(destroyed()), this, SLOT(onControllerDestroyed()));
	connect(c, SIGNAL(deviceAddressChanged()),
			this, SLOT(onDeviceAddressChanged()));
	updateValues();
	updateDeviceAddresses();
	emit deviceAddressesChanged();
}

void BatterySummary::onDeviceAddressChanged()
{
	updateDeviceAddresses();
	emit deviceAddressesChanged();
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
	int count = 0;
	bool maintenanceNeeded = true;
	bool maintenanceActive = true;
	foreach (BatteryController *bc, mControllers) {
		if (bc->connectionState() != Connected)
			continue;
		++count;
		if (bc->BattVolts() > 0) {
			vTot += bc->BattVolts();
			++vCount;
		}
		iTot += bc->BattAmps();
		pTot += bc->BattPower();
		tMax = qMax(tMax, bc->BattTemp());
		socTot += bc->SOC();
		++socCount;
		int userOp = mOperationalMode->getValue().toInt();
		if (userOp != -1)
			bc->setOperationalMode(userOp);
		// We set maintenanceNeeded and maintenanceActive to false if any
		// battery has not been put in maintenance mode yet. We do this, so
		// the GUI can use these values to force all remaining batteries into
		// into maintenance mode.
		maintenanceNeeded = maintenanceNeeded && bc->maintenanceAlarm() != 0;
		maintenanceActive = maintenanceActive && bc->maintenanceActiveAlarm() != 0;
		if (mRequestClearStatusRegister->getValue() == 1)
			bc->setClearStatusRegisterFlags(1);
		if (mRequestDelayedSelfMaintenance->getValue() == 1)
			bc->setRequestDelayedSelfMaintenance(1);
		if (mRequestImmediateSelfMaintenance->getValue() == 1)
			bc->setRequestImmediateSelfMaintenance(1);
	}

	if (vCount > 0)
		mVoltage->setValue(vTot / vCount);
	else
		mVoltage->setValue(QVariant());
	mCurrent->setValue(iTot);
	mPower->setValue(pTot);
	if (socCount > 0)
		mSoc->setValue(socTot / socCount);
	else
		mSoc->setValue(QVariant());

	mOperationalMode->setValue(-1);
	mRequestClearStatusRegister->setValue(0);
	mRequestDelayedSelfMaintenance->setValue(0);
	mRequestImmediateSelfMaintenance->setValue(0);

	mMaintenanceActive->setValue(maintenanceActive && count > 0 ? 1 : 0);
	mMaintenanceNeeded->setValue(maintenanceNeeded && count > 0 ? 1 : 0);
}

void BatterySummary::updateDeviceAddresses()
{
	QString r;
	foreach (BatteryController *bc, mControllers) {
		int address = bc->DeviceAddress();
		if (!r.isEmpty())
			r.append(',');
		r.append(address);
	}
	mDeviceAddresses->setValue(r);
}
