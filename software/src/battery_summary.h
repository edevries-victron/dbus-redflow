#ifndef BATTERYSUMMARY_H
#define BATTERYSUMMARY_H

#include <QList>
#include "abstract_monitor_service.h"

class BatteryController;

/// A statistical roundup of all connected Redflow batteries.
class BatterySummary : public AbstractMonitorService
{
	Q_OBJECT
public:
	BatterySummary(VeQItem *root, QObject *parent = 0);

private:
	void updateValues();

	QList<BatteryController *> mControllers;
	VeQItem *mZbmCount;
	VeQItem *mVoltage;
	VeQItem *mCurrent;
	VeQItem *mPower;
	VeQItem *mTemperature;
	VeQItem *mConsumedAmphours;
	VeQItem *mSoc;
	VeQItem *mOperationalMode;
	VeQItem *mRequestClearStatusRegister;
	VeQItem *mRequestDelayedSelfMaintenance;
	VeQItem *mRequestImmediateSelfMaintenance;
	VeQItem *mMaintenanceActive;
	VeQItem *mMaintenanceNeeded;
};

#endif // BATTERYSUMMARY_H
