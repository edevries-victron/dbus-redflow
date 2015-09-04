#include <velib/qt/v_busitem.h>
#include <velib/qt/v_busitems.h>
#include <QCoreApplication>
#include "batteryController.h"
#include "battery_summary.h"
#include "version.h"

static const QString ServiceName = "com.victronenergy.battery.zbm";

BatterySummary::BatterySummary(QObject *parent):
	QObject(parent),
	mConnection(VBusItems::getConnection("zbm")),
	mVoltage(0),
	mCurrent(0),
	mPower(0),
	mTemperature(0),
	mConsumedAmphours(0),
	// mAlarms(0),
	mSoc(0)
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

	mVoltage = produce("/Dc/0/Voltage", QVariant(), "V", 1);
	mCurrent = produce("/Dc/0/Current", QVariant(), "A", 1);
	mPower = produce("/Dc/0/Power", QVariant(), "W", 0);
	mTemperature = produce("/Dc/0/Temperature", QVariant(), "C", 1);
	mConsumedAmphours = produce("/ConsumedAmphours", QVariant(), "Ah", 1);
	// mAlarms = produce("/Alarm/Alarms", QVariant());
	mSoc = produce("/Soc", QVariant(), "%", 0);
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	timer->setInterval(1000);
	timer->start();

	updateValues();

	mConnection.registerService(ServiceName);
}

BatterySummary::~BatterySummary()
{
	mConnection.unregisterService(ServiceName);
}

void BatterySummary::addBattery(const BatteryController *c)
{
	mControllers.append(c);
	connect(c, SIGNAL(destroyed()), this, SLOT(onDestroyed()));
	updateValues();
}

void BatterySummary::onTimer()
{
	updateValues();
}

void BatterySummary::onDestroyed()
{
	mControllers.removeOne(static_cast<BatteryController *>(sender()));
}

void BatterySummary::updateValues()
{
	double vTot = 0;
	int vCount = 0;
	double iTot	= 0;
	double pTot = 0;
	double tMax = 0;
	double ampHours = 0;
	double socTot = 0;
	int socCount = 0;
	foreach (const BatteryController *bc, mControllers) {
		if (bc->BattVolts() > 0) {
			vTot += bc->BattVolts();
			++vCount;
		}
		iTot += bc->BattAmps();
		pTot += bc->BattPower();
		tMax = qMax(tMax, bc->BattTemp());
		ampHours += bc->SOCAmpHrs();
		socTot += bc->SOC();
		++socCount;
	}
	int count = mControllers.size();
	mVoltage->setValue(vCount == 0 ? QVariant() : vTot / vCount);
	mCurrent->setValue(vCount  == 0 ? QVariant() : iTot);
	mPower->setValue(vCount == 0 ? QVariant() : pTot);
	mTemperature->setValue(count == 0 ? QVariant() : tMax);
	mConsumedAmphours->setValue(count == 0 ? QVariant() : ampHours);
	// mAlarms->setValue(0);
	mSoc->setValue(socCount == 0 ? QVariant() : socTot / socCount);
}

VBusItem *BatterySummary::produce(const QString &path, const QVariant &value,
								  const QString &unit, int precision)
{
	VBusItem *item = new VBusItem(this);
	item->produce(mConnection, path, "", value, unit, precision);
	return item;
}
