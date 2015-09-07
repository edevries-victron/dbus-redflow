#include <velib/qt/v_busitem.h>
#include "batteryController.h"
#include "battery_summary.h"

static const QString ServiceName = "com.victronenergy.battery.zbm";

BatterySummary::BatterySummary(QObject *parent):
	AbstractMonitorService(ServiceName, parent),
	mVoltage(0),
	mCurrent(0),
	mPower(0),
	mTemperature(0),
	mConsumedAmphours(0),
	// mAlarms(0),
	mSoc(0)
{
	mVoltage = produce("/Dc/0/Voltage", QVariant(), "V", 1);
	mCurrent = produce("/Dc/0/Current", QVariant(), "A", 1);
	mPower = produce("/Dc/0/Power", QVariant(), "W", 0);
	mTemperature = produce("/Dc/0/Temperature", QVariant(), "C", 1);
	mConsumedAmphours = produce("/ConsumedAmphours", QVariant(), "Ah", 1);
	// mAlarms = produce("/Alarm/Alarms", QVariant());
	mSoc = produce("/Soc", QVariant(), "%", 0);
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
	foreach (const BatteryController *bc, controllers()) {
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
	int count = controllers().size();
	mVoltage->setValue(vCount == 0 ? QVariant() : vTot / vCount);
	mCurrent->setValue(vCount  == 0 ? QVariant() : iTot);
	mPower->setValue(vCount == 0 ? QVariant() : pTot);
	mTemperature->setValue(count == 0 ? QVariant() : tMax);
	mConsumedAmphours->setValue(count == 0 ? QVariant() : ampHours);
	// mAlarms->setValue(0);
	mSoc->setValue(socCount == 0 ? QVariant() : socTot / socCount);
}
