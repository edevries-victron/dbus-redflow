#ifndef BMSSERVICE_H
#define BMSSERVICE_H

#include "abstract_monitor_service.h"

class VeQItem;

class BmsService : public AbstractMonitorService
{
	Q_OBJECT
public:
	BmsService(VeQItem *root, QObject *parent = 0);

protected:
	virtual void updateValues();

private:
	VeQItem *mAllowedToCharge;
	VeQItem *mAllowedToDischarge;
};

#endif // BMSSERVICE_H
