#include <QTimer>
#include <velib/qt/v_busitem.h>
#include "batteryController.h"
#include "battery_summary.h"

BatterySummary::BatterySummary(QObject *parent):
	QObject(parent),
	mZbmCount(0),
	mAverageVoltage(0),
	mTotalCurrent(0),
	mTotalPower(0),
	mAverageStateOfCharge(0),
	mOperationalMode(-1),
	mRequestClearStatusRegister(0),
	mRequestDelayedSelfMaintenance(0),
	mRequestImmediateSelfMaintenance(0),
	mMaintenanceActive(0),
	mMaintenanceNeeded(0)
{
	QTimer *timer = new QTimer(this);
	timer->setInterval(1000);
	timer->start();
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
	updateValues();
}

QList<int> BatterySummary::deviceAddresses() const
{
	QList<int> result;
	foreach (BatteryController *bc, mControllers) {
		result.append(bc->DeviceAddress());
	}
	return result;
}

void BatterySummary::addBattery(BatteryController *c)
{
	if (mControllers.contains(c))
		return;
	mControllers.append(c);
	connect(c, SIGNAL(destroyed()), this, SLOT(onControllerDestroyed()));
	updateValues();
	emit deviceAddressesChanged();
}

int BatterySummary::zbmCount() const
{
	return mZbmCount;
}

void BatterySummary::setZbmCount(int v)
{
	if (mZbmCount == v)
		return;
	mZbmCount = v;
	emit zbmCountChanged();
}

double BatterySummary::averageVoltage() const
{
	return mAverageVoltage;
}

void BatterySummary::setAverageVoltage(double v)
{
	if (mAverageVoltage == v)
		return;
	mAverageVoltage = v;
	emit averageVoltageChanged();
}

double BatterySummary::totalCurrent() const
{
	return mTotalCurrent;
}

void BatterySummary::setTotalCurrent(double v)
{
	if (mTotalCurrent == v)
		return;
	mTotalCurrent = v;
	emit totalCurrentChanged();
}

double BatterySummary::totalPower() const
{
	return mTotalPower;
}

void BatterySummary::setTotalPower(double v)
{
	if (mTotalPower == v)
		return;
	mTotalPower = v;
	emit totalPowerChanged();
}

double BatterySummary::averageStateOfCharge() const
{
	return mAverageStateOfCharge;
}

void BatterySummary::setAverageStateOfCharge(double v)
{
	if (mAverageStateOfCharge == v)
		return;
	mAverageStateOfCharge = v;
	emit averageStateOfChargeChanged();
}

int BatterySummary::operationalMode() const
{
	return mOperationalMode;
}

void BatterySummary::setOperationalMode(int v)
{
	if (mOperationalMode == v)
		return;
	mOperationalMode = v;
	emit operationalModeChanged();
}

int BatterySummary::requestClearStatusRegister() const
{
	return mRequestClearStatusRegister;
}

void BatterySummary::setRequestClearStatusRegister(int v)
{
	if (mRequestClearStatusRegister == v)
		return;
	mRequestClearStatusRegister = v;
	emit requestClearStatusRegisterChanged();
}

int BatterySummary::requestDelayedSelfMaintenance() const
{
	return mRequestDelayedSelfMaintenance;
}

void BatterySummary::setRequestDelayedSelfMaintenance(int v)
{
	if (mRequestDelayedSelfMaintenance == v)
		return;
	mRequestDelayedSelfMaintenance = v;
	emit requestDelayedSelfMaintenanceChanged();
}

int BatterySummary::requestImmediateSelfMaintenance() const
{
	return mRequestImmediateSelfMaintenance;
}

void BatterySummary::setRequestImmediateSelfMaintenance(int v)
{
	if (mRequestImmediateSelfMaintenance == v)
		return;
	mRequestImmediateSelfMaintenance = v;
	emit requestImmediateSelfMaintenanceChanged();
}

int BatterySummary::maintenanceActive() const
{
	return mMaintenanceActive;
}

void BatterySummary::setMaintenanceActive(int v)
{
	if (mMaintenanceActive == v)
		return;
	mMaintenanceActive = v;
	emit maintenanceActiveChanged();
}

int BatterySummary::maintenanceNeeded() const
{
	return mMaintenanceNeeded;
}

void BatterySummary::setMaintenanceNeeded(int v)
{
	if (mMaintenanceNeeded == v)
		return;
	mMaintenanceNeeded = v;
	emit maintenanceNeededChanged();
}

void BatterySummary::onTimeout()
{
	updateValues();
}

void BatterySummary::onControllerDestroyed()
{
	if (mControllers.removeOne(static_cast<BatteryController *>(sender()))) {
		updateValues();
		emit deviceAddressesChanged();
	}
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
		int userOp = mOperationalMode;
		if (userOp != -1)
			bc->setOperationalMode(userOp);
		// We set maintenanceNeeded and maintenanceActive to false if any
		// battery has not been put in maintenance mode yet. We do this, so
		// the GUI can use these values to force all remaining batteries into
		// into maintance mode.
		maintenanceNeeded = maintenanceNeeded && bc->maintenanceAlarm() != 0;
		maintenanceActive = maintenanceActive && bc->maintenanceActiveAlarm() != 0;
		if (mRequestClearStatusRegister == 1)
			bc->setClearStatusRegisterFlags(1);
		if (mRequestDelayedSelfMaintenance == 1)
			bc->setRequestDelayedSelfMaintenance(1);
		if (mRequestImmediateSelfMaintenance == 1)
			bc->setRequestImmediateSelfMaintenance(1);
	}
	int count = mControllers.size();

	setZbmCount(count);
	setAverageVoltage(vTot / vCount);
	setTotalCurrent(iTot);
	setTotalPower(pTot);
	setAverageStateOfCharge(socTot / socCount);

	setOperationalMode(-1);
	setRequestClearStatusRegister(0);
	setRequestDelayedSelfMaintenance(0);
	setRequestImmediateSelfMaintenance(0);

	setMaintenanceActive(maintenanceNeeded && count > 0 ? 1 : 0);
	setMaintenanceNeeded(maintenanceActive && count > 0 ? 1 : 0);
}
