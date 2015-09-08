#ifndef BATTERYSUMMARY_H
#define BATTERYSUMMARY_H

#include "abstract_monitor_service.h"

class VBusItem;

class BatterySummary : public AbstractMonitorService
{
	Q_OBJECT
public:
	BatterySummary(QObject *parent = 0);

protected:
	virtual void updateValues();

private:
	VBusItem *mZbmCount;
	VBusItem *mVoltage;
	VBusItem *mCurrent;
	VBusItem *mPower;
	VBusItem *mTemperature;
	VBusItem *mSoc;
};

#endif // BATTERYSUMMARY_H
