#include <QTimer>
#include <velib/qt/v_busitem.h>
#include <velib/qt/ve_qitem.hpp>
#include "batteryController.h"
#include "battery_summary.h"

// static const QString ServiceName = "com.victronenergy.battery.zbm";

BatterySummary::BatterySummary(VeQItem *root, QObject *parent):
	AbstractMonitorService(root, parent),
	mVoltage(0),
	mCurrent(0),
	mPower(0),
	mTemperature(0),
	mConsumedAmphours(0),
	mSoc(0)
{
	mVoltage = produce("/Dc/0/Voltage", QVariant(), "V", 1);
	mCurrent = produce("/Dc/0/Current", QVariant(), "A", 1);
	mPower = produce("/Dc/0/Power", QVariant(), "W", 0);
	mTemperature = produce("/Dc/0/Temperature", QVariant(), "C", 1);
	mConsumedAmphours = produce("/ConsumedAmphours", QVariant(), "Ah", 1);
	mSoc = produce("/Soc", QVariant(), "%", 0);
	mZbmCount = produce("/ZbmCount", 0);
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
	foreach (BatteryController *bc, mControllers) {
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
		// into maintance mode.
		maintenanceNeeded = maintenanceNeeded && bc->maintenanceAlarm() != 0;
		maintenanceActive = maintenanceActive && bc->maintenanceActiveAlarm() != 0;
		if (mRequestClearStatusRegister->getValue() == 1)
			bc->setClearStatusRegisterFlags(1);
		if (mRequestDelayedSelfMaintenance->getValue() == 1)
			bc->setRequestDelayedSelfMaintenance(1);
		if (mRequestImmediateSelfMaintenance->getValue() == 1)
			bc->setRequestImmediateSelfMaintenance(1);
	}
	int count = mControllers.size();
	mZbmCount->setValue(count);
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
//	mOperationalMode->setValue(-1);
//	mRequestClearStatusRegister->setValue(0);
//	mRequestDelayedSelfMaintenance->setValue(0);
//	mRequestImmediateSelfMaintenance->setValue(0);

//	mMaintenanceActive->setValue(maintenanceNeeded && count > 0 ? 1 : 0);
//	mMaintenanceNeeded->setValue(maintenanceActive && count > 0 ? 1 : 0);
}
