#include <QsLog.h>
#include "batteryController.h"

BatteryController::BatteryController(const QString &portName, int slaveAddress, QObject *parent) :
	QObject(parent),
	mConnectionState(Disconnected),
	mDeviceType(0),
	mDeviceSubType(0),
	mErrorCode(0),
	mFirmwareVersion(0),
	mPortName(portName),
	mSlaveAddress(slaveAddress)
{
QLOG_INFO() << "BatteryController:: " << slaveAddress;	
	resetValues();
}

ConnectionState BatteryController::connectionState() const
{
	return mConnectionState;
}

void BatteryController::setConnectionState(ConnectionState state)
{
	if (mConnectionState == state)
		return;
	mConnectionState = state;
	emit connectionStateChanged();
}

int BatteryController::deviceType() const
{
	return mDeviceType;
}

void BatteryController::setDeviceType(int t)
{
	if (mDeviceType == t)
		return;
	mDeviceType = t;
	emit deviceTypeChanged();
}

int BatteryController::deviceSubType() const
{
	return mDeviceSubType;
}

void BatteryController::setDeviceSubType(int t)
{
	if (mDeviceSubType == t)
		return;
	mDeviceSubType = t;
	emit deviceSubTypeChanged();
}

QString BatteryController::productName() const
{
	return "ZBM";
}

int BatteryController::errorCode() const
{
	return mErrorCode;
}

void BatteryController::setErrorCode(int code)
{
	if (mErrorCode == code)
		return;
	mErrorCode = code;
	emit errorCodeChanged();
}

QString BatteryController::portName() const
{
	return mPortName;
}

int BatteryController::slaveAddress() const
{
	return mSlaveAddress;
}

QString BatteryController::serial() const
{
	return mSerial;
}

void BatteryController::setSerial(const QString &s)
{
	if (mSerial == s)
		return;
	mSerial = s;
	emit serialChanged();
}

int BatteryController::firmwareVersion() const
{
	return mFirmwareVersion;
}

void BatteryController::setFirmwareVersion(int v)
{
	if (mFirmwareVersion == v)
		return;
	mFirmwareVersion = v;
	emit firmwareVersionChanged();
}

int BatteryController::BattVolts() const
{
	return mBattVolts /10;
}

void BatteryController::setBattVolts(int t)
{
	//if (mBattVolts == t)
	//	return;
	mBattVolts = t;
	emit battVoltsChanged();
}

int BatteryController::BussVolts() const
{
	return mBussVolts;
}

void BatteryController::setBussVolts(int t)
{
	if (mBussVolts == t)
		return;
	mBussVolts = t;
}

double BatteryController::BattAmps() const
{
	return (double) (((float)mBattAmps)/10)*(-1) ;
}

void BatteryController::setBattAmps(int t)
{
	//if (mBattAmps == t)
	//	return;
	mBattAmps = t;
	emit battAmpsChanged();
}

int BatteryController::BussAmps() const
{
	return mBussAmps;
}

void BatteryController::setBussAmps(int t)
{
	if (mBussAmps == t)
		return;
	mBussAmps = t;
}

double BatteryController::BattTemp() const
{
//DEBUG
QLOG_INFO() <<"read BattTemp";
	return (double) (((float) mBattTemp) / 10);
}

void BatteryController::setBattTemp(int t)
{
	//if (mBattTemp == t)
	//	return;
	mBattTemp = t;
	emit battTempChanged();
}

int BatteryController::AirTemp() const
{
	return mAirTemp;
}

void BatteryController::setAirTemp(int t)
{
	if (mAirTemp == t)
		return;
	mAirTemp = t;
}

int BatteryController::SOC() const
{
	return mSOC / 100;
}

void BatteryController::setSOC(int t)
{
	//if (mSOC == t)
	//	return;
	mSOC = t;
	emit socChanged();
}

int BatteryController::BattPower() const
{
	return (mBattAmps /10) * (mBattVolts / 10);
}

void BatteryController::setBattPower(int t)
{
	if (mBattPower == t)
		return;
	mBattPower = t;
}

void BatteryController::resetValues()
{

}
