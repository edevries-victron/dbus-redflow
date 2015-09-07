#ifndef BATTERYSUMMARY_H
#define BATTERYSUMMARY_H

#include <QList>
#include "abstract_monitor_service.h"

class BatteryController;

/// A statistical roundup of all connected Redflow batteries.
class BatterySummary : public AbstractMonitorService
{
	Q_OBJECT
	Q_PROPERTY(QList<int> deviceAddresses READ deviceAddresses NOTIFY deviceAddressesChanged)
public:
	BatterySummary(VeQItem *root, QObject *parent = 0);

	QList<int> deviceAddresses() const;

	void addBattery(BatteryController *c);

signals:
	void deviceAddressesChanged();

private slots:
	void onDeviceAddressChanged();

private:
	void updateValues();

	void updateDeviceAddresses();

	QList<BatteryController *> mControllers;

	VeQItem *mDeviceAddresses;
	VeQItem *mVoltage;
	VeQItem *mCurrent;
	VeQItem *mPower;
	VeQItem *mSoc;
	VeQItem *mOperationalMode;
	VeQItem *mRequestClearStatusRegister;
	VeQItem *mRequestDelayedSelfMaintenance;
	VeQItem *mRequestImmediateSelfMaintenance;
	VeQItem *mMaintenanceActive;
	VeQItem *mMaintenanceNeeded;
};

#endif // BATTERYSUMMARY_H
