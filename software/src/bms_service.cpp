#include <velib/qt/v_busitem.h>
#include <velib/qt/v_busitems.h>
#include <QCoreApplication>
#include "batteryController.h"
#include "bms_service.h"
#include "version.h"

static const QString ServiceName = "com.victronenergy.bms.zbm";

BmsService::BmsService(QObject *parent):
	QObject(parent),
	mConnection(VBusItems::getConnection("bms")),
	mAllowedToCharge(0),
	mAllowedToDischarge(0)
{
	QString processName = QCoreApplication::arguments()[0];
	produce("/Mgmt/ProcessName", processName);
	produce("/Mgmt/ProcessVersion", VERSION);
	produce("/Connection", 1);
	// produce("/FirmwareVersion", BatteryController->firmwareVersion());
	produce("/ProductName", "ZBM summary");
	// produce("/ProductId", VE_PROD_ID_REDFLOW_ZBM2);
	// produce("/DeviceType", BatteryController->deviceType());
	// produce("/Mgmt/Connection", portName);
	// produce("/DeviceInstance", deviceInstance);

	mAllowedToCharge = produce("/AllowedToCharge", 0);
	mAllowedToDischarge = produce("/AllowedToDischarge", 0);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	timer->setInterval(1000);
	timer->start();

	updateValues();

	mConnection.registerService(ServiceName);
}

BmsService::~BmsService()
{
	mConnection.unregisterService(ServiceName);
}

void BmsService::addBattery(const BatteryController *c)
{
	mControllers.append(c);
	connect(c, SIGNAL(destroyed()), this, SLOT(onDestroyed()));
	updateValues();
}

void BmsService::onTimer()
{
	updateValues();
}

void BmsService::onDestroyed()
{
	mControllers.removeOne(static_cast<BatteryController *>(sender()));
}

void BmsService::updateValues()
{
	bool allowCharge = false;
	bool allowDischarge = false;
	foreach (const BatteryController *bc, mControllers) {
		double soc = bc->SOC();
		allowCharge = allowCharge || soc < 100;
		allowDischarge = allowDischarge || soc > 0;
	}
	mAllowedToCharge->setValue(allowCharge ? 1 : 0);
	mAllowedToDischarge->setValue(allowDischarge ? 1 : 0);
}

VBusItem *BmsService::produce(const QString &path, const QVariant &value,
							  const QString &unit, int precision)
{
	VBusItem *item = new VBusItem(this);
	item->produce(mConnection, path, "", value, unit, precision);
	return item;
}
