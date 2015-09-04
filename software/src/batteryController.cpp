#include <QsLog.h>
#include "batteryController.h"

BatteryController::BatteryController(const QString &portName, int deviceAddress,
									 QObject *parent) :
	QObject(parent),
	mConnectionState(Disconnected),
	mDeviceType(0),
	mErrorCode(0),
	mFirmwareVersion(0),
	mPortName(portName),
	mBattVolts(0),
	mBussVolts(0),
	mBattAmps(0),
	mBussAmps(0),
	mBattTemp(0),
	mAirTemp(0),
	mSOC(0),
	mStsRegSummary(0),
	mStsRegHardwareFailure(0),
	mStsRegOperationalFailure(0),
	mStsRegWarning(0),
	mOperationalMode(0),
	mSOCAmpHrs(0),
	mHealthIndication(0),
	mState(0),
	mDeviceAddress(deviceAddress),
	mClearStatusRegisterFlags(0),
	mRequestDelayedSelfMaintenance(0),
	mRequestImmediateSelfMaintenance(0)
{
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

QString BatteryController::productName() const
{
	return "ZBM " + serial();
}

QString BatteryController::portName() const
{
	return mPortName;
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
	emit productName();
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

double BatteryController::BattVolts() const
{
	return mBattVolts;
}

void BatteryController::setBattVolts(double t)
{
	if (mBattVolts == t)
		return;
	mBattVolts = t;
	emit battVoltsChanged();
	emit battPowerChanged();
}

double BatteryController::BussVolts() const
{
	return mBussVolts;
}

void BatteryController::setBussVolts(double t)
{
	if (mBussVolts == t)
		return;
	mBussVolts = t;
	emit bussVoltsChanged();
}

double BatteryController::BattAmps() const
{
	return mBattAmps;
}

void BatteryController::setBattAmps(double t)
{
	if (mBattAmps == t)
		return;
	mBattAmps = t;
	emit battAmpsChanged();
	emit battPowerChanged();
}

double BatteryController::BussAmps() const
{
	return mBussAmps;
}

void BatteryController::setBussAmps(double t)
{
	if (mBussAmps == t)
		return;
	mBussAmps = t;
	emit bussAmpsChanged();
}

double BatteryController::BattTemp() const
{
	return mBattTemp;
}

void BatteryController::setBattTemp(double t)
{
	if (mBattTemp == t)
		return;
	mBattTemp = t;
	emit battTempChanged();
}

double BatteryController::AirTemp() const
{
	return mAirTemp;
}

void BatteryController::setAirTemp(double t)
{
	if (mAirTemp == t)
		return;
	mAirTemp = t;
	emit airTempChanged();
}

double BatteryController::SOC() const
{
	return mSOC;
}

void BatteryController::setSOC(double t)
{
	if (mSOC == t)
		return;
	mSOC = t;
	emit socChanged();
}

double BatteryController::BattPower() const
{
	return mBattAmps * mBattVolts;
}

int BatteryController::StsRegSummary() const
{
	return mStsRegSummary;
}

void BatteryController::setStsRegSummary(int t)
{
	if (mStsRegSummary == t)
		return;
	mStsRegSummary = t;
	emit stsRegSummaryChanged();
}

int BatteryController::StsRegHardwareFailure() const
{
	return mStsRegHardwareFailure;
}

void BatteryController::setStsRegHardwareFailure(int t)
{
	if (mStsRegHardwareFailure == t)
		return;
	mStsRegHardwareFailure = t;
	emit stsRegHardwareFailureChanged();
}

int BatteryController::StsRegOperationalFailure() const
{
	return mStsRegOperationalFailure;
}

void BatteryController::setStsRegOperationalFailure(int t)
{
	if (mStsRegOperationalFailure == t)
		return;
	mStsRegOperationalFailure = t;
	emit stsRegOperationalFailureChanged();
}

int BatteryController::StsRegWarning() const
{
	return mStsRegWarning;
}

void BatteryController::setStsRegWarning(int t)
{
	if (mStsRegWarning == t)
		return;
	mStsRegWarning = t;
	emit stsRegWarningChanged();
}

int BatteryController::operationalMode() const
{
	return mOperationalMode;
}

void BatteryController::setOperationalMode(int t)
{
	if (mOperationalMode == t)
		return;
	mOperationalMode = t;
	emit operationalModeChanged();
}

double BatteryController::SOCAmpHrs() const
{
	return mSOCAmpHrs;
}

void BatteryController::setSOCAmpHrs(double t)
{
	if (mSOCAmpHrs == t)
		return;
	mSOCAmpHrs = t;
	emit socAmpHrsChanged();
}

double BatteryController::HealthIndication() const
{
	return mHealthIndication;
}

void BatteryController::setHealthIndication(double t)
{
	if (mHealthIndication == t)
		return;
	mHealthIndication = t;
	emit healthIndicationChanged();
}

int BatteryController::State() const
{
	return mState;
}

void BatteryController::setState(int t)
{
	if (mState == t)
		return;
	mState = t;
	emit stateChanged();
}

int BatteryController::DeviceAddress() const
{
	return mDeviceAddress;
}

void BatteryController::setDeviceAddress(int t)
{
	if (mDeviceAddress == t)
		return;
	mDeviceAddress = t;
	emit deviceAddressChanged();
}

int BatteryController::ClearStatusRegisterFlags() const
{
	return mClearStatusRegisterFlags;
}

void BatteryController::setClearStatusRegisterFlags(int t)
{
	if (mClearStatusRegisterFlags == t)
		return;
	mClearStatusRegisterFlags = t;
	emit clearStatusRegisterFlagsChanged();
}

int BatteryController::RequestDelayedSelfMaintenance() const
{
	return mRequestDelayedSelfMaintenance;
}

void BatteryController::setRequestDelayedSelfMaintenance(int t)
{
	if (mRequestDelayedSelfMaintenance == t)
		return;
	mRequestDelayedSelfMaintenance = t;
	emit requestDelayedSelfMaintenanceChanged();
}

int BatteryController::RequestImmediateSelfMaintenance() const
{
	return mRequestImmediateSelfMaintenance;
}

void BatteryController::setRequestImmediateSelfMaintenance(int t)
{
	if (mRequestImmediateSelfMaintenance == t)
		return;
	mRequestImmediateSelfMaintenance = t;
	emit requestImmediateSelfMaintenanceChanged();
}

int BatteryController::maintenanceAlarm() const
{
	return mMaintenanceAlarm;
}

void BatteryController::setMaintenanceAlarm(int v)
{
	if (mMaintenanceAlarm == v)
		return;
	mMaintenanceAlarm = v;
	emit maintenanceAlarmChanged();
}

int BatteryController::maintenanceActiveAlarm() const
{
	return mMaintenanceActiveAlarm;
}

void BatteryController::setMaintenanceActiveAlarm(int v)
{
	if (mMaintenanceActiveAlarm == v)
		return;
	mMaintenanceActiveAlarm = v;
	emit maintenanceActiveAlarmChanged();
}

int BatteryController::overCurrentAlarm() const
{
	return mOverCurrentAlarm;
}

void BatteryController::setOverCurrentAlarm(int v)
{
	if (mOverCurrentAlarm == v)
		return;
	mOverCurrentAlarm = v;
	emit overCurrentAlarmChanged();
}

int BatteryController::overVoltageAlarm() const
{
	return mOverVoltageAlarm;
}

void BatteryController::setOverVoltageAlarm(int v)
{
	if (mOverVoltageAlarm == v)
		return;
	mOverVoltageAlarm = v;
	emit overVoltageAlarmChanged();
}

int BatteryController::batteryTemperatureAlarm() const
{
	return mBatteryTemperatureAlarm;
}

void BatteryController::setBatteryTemperatureAlarm(int v)
{
	if (mBatteryTemperatureAlarm == v)
		return;
	mBatteryTemperatureAlarm = v;
	emit batteryTemperatureAlarmChanged();
}

int BatteryController::zincPumpAlarm() const
{
	return mZincPumpAlarm;
}

void BatteryController::setZincPumpAlarm(int v)
{
	if (mZincPumpAlarm == v)
		return;
	mZincPumpAlarm = v;
	emit zincPumpAlarmChanged();
}

int BatteryController::bromidePumpAlarm() const
{
	return mBromidePumpAlarm;
}

void BatteryController::setBromidePumpAlarm(int v)
{
	if (mBromidePumpAlarm == v)
		return;
	mBromidePumpAlarm = v;
	emit bromidePumpAlarmChanged();
}

int BatteryController::leakSensorsAlarm() const
{
	return mLeakSensorsAlarm;
}

void BatteryController::setLeakSensorsAlarm(int v)
{
	if (mLeakSensorsAlarm == v)
		return;
	mLeakSensorsAlarm = v;
	emit leakSensorsAlarmChanged();
}

int BatteryController::internalFailureAlarm() const
{
	return mInternalFailureAlarm;
}

void BatteryController::setInternalFailureAlarm(int v)
{
	if (mInternalFailureAlarm == v)
		return;
	mInternalFailureAlarm = v;
	emit internalFailureAlarmChanged();
}

int BatteryController::electricBoardAlarm() const
{
	return mElectricBoardAlarm;
}

void BatteryController::setElectricBoardAlarm(int v)
{
	if (mElectricBoardAlarm == v)
		return;
	mElectricBoardAlarm = v;
	emit electricBoardAlarmChanged();
}

int BatteryController::batteryTemperatureSensorAlarm() const
{
	return mBatteryTemperatureSensorAlarm;
}

void BatteryController::setBatteryTemperatureSensorAlarm(int v)
{
	if (mBatteryTemperatureSensorAlarm == v)
		return;
	mBatteryTemperatureSensorAlarm = v;
	emit batteryTemperatureSensorAlarmChanged();
}

int BatteryController::airTemperatureSensorAlarm() const
{
	return mAirTemperatureSensorAlarm;
}

void BatteryController::setAirTemperatureSensorAlarm(int v)
{
	if (mAirTemperatureSensorAlarm == v)
		return;
	mAirTemperatureSensorAlarm = v;
	emit airTemperatureSensorAlarmChanged();
}

int BatteryController::stateOfHealthAlarm() const
{
	return mStateOfHealthAlarm;
}

void BatteryController::setStateOfHealthAlarm(int v)
{
	if (mStateOfHealthAlarm == v)
		return;
	mStateOfHealthAlarm = v;
	emit stateOfHealthAlarmChanged();
}

int BatteryController::leak1TripAlarm() const
{
	return mLeak1TripAlarm;
}

void BatteryController::setLeak1TripAlarm(int v)
{
	if (mLeak1TripAlarm == v)
		return;
	mLeak1TripAlarm = v;
	emit leak1TripAlarmChanged();
}

int BatteryController::leak2TripAlarm() const
{
	return mLeak2TripAlarm;
}

void BatteryController::setLeak2TripAlarm(int v)
{
	if (mLeak2TripAlarm == v)
		return;
	mLeak2TripAlarm = v;
	emit leak2TripAlarmChanged();
}

int BatteryController::unknownAlarm() const
{
	return mUnknownAlarm;
}

void BatteryController::setUnknownAlarm(int v)
{
	if (mUnknownAlarm == v)
		return;
	mUnknownAlarm = v;
	emit unknownAlarmChanged();
}
