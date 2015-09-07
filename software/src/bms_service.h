#ifndef BMSSERVICE_H
#define BMSSERVICE_H

#include "abstract_monitor_service.h"

class VBusItem;

class BmsService : public AbstractMonitorService
{
	Q_OBJECT
public:
	BmsService(QObject *parent = 0);

protected:
	virtual void updateValues();

private:
	VBusItem *mAllowedToCharge;
	VBusItem *mAllowedToDischarge;
};

#endif // BMSSERVICE_H
