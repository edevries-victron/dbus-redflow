#include <cmath>
#include <limits>
#include "battery_info.h"

static const double NaN = std::numeric_limits<double>::quiet_NaN();

BatteryInfo::BatteryInfo(QObject *parent) :
	QObject(parent),
	mBattVolts(NaN),
	mBattAmps(NaN),
	mBussVolts(NaN),
	mBussAmps(NaN),
	mBattTemp(NaN),
	mAirTemp(NaN),
	mSOC(NaN)
{
}

double BatteryInfo::BattVolts() const
{
	return mBattVolts;
}

void BatteryInfo::setBattVolts(double c)
{
	if (mBattVolts == t)
		return;
	mBattVolts = t;
	emit BattVoltsChanged();
}

int BatteryInfo::BussVolts() const
{
	return mBussVolts;
}

void BatteryInfo::setBussVolts(int t)
{
	if (mBussVolts == t)
		return;
	mBussVolts = t;
	emit BussVoltsChanged();
}

int BatteryInfo::BattAmps() const
{
	return mBattAmps;
}

void BatteryInfo::setBattAmps(int t)
{
	if (mBattAmps == t)
		return;
	mBattAmps = t;
	emit BattAmpsChanged();
}

int BatteryInfo::BussAmps() const
{
	return mBussAmps;
}

void BatteryInfo::setBussAmps(int t)
{
	if (mBussAmps == t)
		return;
	mBussAmps = t;
	emit BussAmpsChanged();
}

int BatteryInfo::BattTemp() const
{
	return mBattTemp;
}

void BatteryInfo::setBattTemp(int t)
{
	if (mBattTemp == t)
		return;
	mBattTemp = t;
	emit BattTempChanged();
}

int BatteryInfo::AirTemp() const
{
	return mAirTemp;
}

void BatteryInfo::setAirTemp(int t)
{
	if (mAirTemp == t)
		return;
	mAirTemp = t;
	emit AirTempChanged();
}

int BatteryInfo::SOC() const
{
	return mSOC;
}

void BatteryInfo::setSOC(int t)
{
	if (mSOC == t)
		return;
	mSOC = t;
	emit SOCChanged();
}










double PowerInfo::voltage() const
{
	return mVoltage;
}

void PowerInfo::setVoltage(double v)
{
	if (valuesEqual(mVoltage, v))
		return;
	mVoltage = v;
	emit voltageChanged();
}

double PowerInfo::power() const
{
	return mPower;
}

void PowerInfo::setPower(double p)
{
	if (valuesEqual(mPower, p))
		return;
	mPower = p;
	emit powerChanged();
}

double PowerInfo::energyForward() const
{
	return mEnergyForward;
}

void PowerInfo::setEnergyForward(double e)
{
	if (valuesEqual(mEnergyForward, e))
		return;
	mEnergyForward = e;
	emit energyForwardChanged();
}

double PowerInfo::energyReverse() const
{
	return mEnergyReverse;
}

void PowerInfo::setEnergyReverse(double e)
{
	if (valuesEqual(mEnergyReverse, e))
		return;
	mEnergyReverse = e;
	emit energyReverseChanged();
}

void PowerInfo::resetValues()
{
	setCurrent(NaN);
	setPower(NaN);
	setVoltage(NaN);
	setEnergyForward(NaN);
	setEnergyReverse(NaN);
}

bool PowerInfo::valuesEqual(double v1, double v2)
{
	return (std::isnan(v1) && std::isnan(v2)) || (v1 == v2);
}
