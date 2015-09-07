#include <velib/qt/v_busitem.h>
#include "batteryController.h"
#include "bms_service.h"

static const QString ServiceName = "com.victronenergy.bms.zbm";

BmsService::BmsService(QObject *parent):
	AbstractMonitorService(ServiceName, parent)
{
	mAllowedToCharge = produce("/AllowedToCharge", 0);
	mAllowedToDischarge = produce("/AllowedToDischarge", 0);
}

void BmsService::updateValues()
{
	bool allowCharge = false;
	bool allowDischarge = false;
	foreach (const BatteryController *bc, controllers()) {
		double soc = bc->SOC();
		allowCharge = allowCharge || soc < 100;
		allowDischarge = allowDischarge || soc > 0;
	}
	mAllowedToCharge->setValue(allowCharge ? 1 : 0);
	mAllowedToDischarge->setValue(allowDischarge ? 1 : 0);
}
