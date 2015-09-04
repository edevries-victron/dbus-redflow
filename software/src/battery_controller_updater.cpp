#include <QsLog.h>
#include <QTimer>
#include <velib/qt/ve_qitem.hpp>
#include "batteryController.h"
#include "battery_controller_settings.h"
#include "battery_controller_updater.h"
#include "modbus_rtu.h"

static const int MaxTimeoutCount = 5;
static const int MeasurementWaitInterval = 5000;
static const int ConnectionLostWaitInterval = 60 * 1000;  // 60 seconds in ms

enum ModbusRegisters {
	RegStatusSummary = 0x9001,
	RegHardwareFailure = 0x9002,
	RegOperationalFailure = 0x9003,
	RegWarningIndicator = 0x9004,
	RegOperationalMode = 0x9008,
	RegSerial = 0x9010,
	RegStateOfCharge = 0x9011,
	RefAmpHours = 0x9012,
	RegBatteryVoltage = 0x9013,
	RegBatteryCurrent = 0x9014,
	RegBatteryTemperature = 0x9015,
	RegAirTemperature = 0x9016,
	RegStateOfHealth = 0x9017,
	RegBusVoltage = 0x9018,
	RegBatteryState = 0x9019,
	RegDeviceAddress = 0x9030,
	RegClearStatusFlags = 0x9031,
	RegDelayedSelfMaintenance = 0x9032,
	RegEnterRunCommand = 0x9033,
	RegImmediateSelfMaintenance = 0x9034
};

BatteryControllerUpdater::BatteryControllerUpdater(int deviceAddress, VeQItem *root,
												   ModbusRtu *modbus,
												   QObject *parent):
	QObject(parent),
	mRoot(root),
	mService(0),
//	mBatteryController(mBatteryController),
	mDeviceAddress(deviceAddress),
	mUpdatingController(false),
	mModbus(0),
	mAcquisitionTimer(new QTimer(this)),
	mTimeoutCount(0),
	mState(Init),
	mTmpState(Wait)
{
	// Q_ASSERT(mBatteryController != 0);
	mModbus = modbus;
	connect(mModbus, SIGNAL(readCompleted(int, quint8, const QList<quint16> &)),
			this, SLOT(onReadCompleted(int, quint8, QList<quint16>)));
	connect(mModbus, SIGNAL(writeCompleted(int, quint8, quint16, quint16)),
			this, SLOT(onWriteCompleted(int, quint8, quint16, quint16)));
	connect(mModbus, SIGNAL(errorReceived(int, quint8, int)),
			this, SLOT(onErrorReceived(int, quint8, int)));
	connect(mAcquisitionTimer, SIGNAL(timeout()),
			this, SLOT(onWaitFinished()));
//	connect(mBatteryController, SIGNAL(clearStatusRegisterFlagsChanged()),
//			this, SLOT(onClearStatusRegisterFlagsChanged()));
//	connect(mBatteryController, SIGNAL(operationalModeChanged()),
//			this, SLOT(onOperationalModeChanged()));
//	connect(mBatteryController, SIGNAL(requestDelayedSelfMaintenanceChanged()),
//			this, SLOT(onRequestDelayedSelfMaintenanceChanged()));
//	connect(mBatteryController, SIGNAL(requestImmediateSelfMaintenanceChanged()),
//			this, SLOT(onRequestImmediateSelfMaintenanceChanged()));
	mAcquisitionTimer->setSingleShot(true);
	mStopwatch.start();
	startNextAction();
}

void BatteryControllerUpdater::onErrorReceived(int errorType, quint8 slaveAddress,
											   int exception)
{
	if (slaveAddress != mDeviceAddress)
		return;
	QLOG_WARN() << __FUNCTION__ << errorType << slaveAddress << exception;
	if (errorType == ModbusRtu::Timeout) {
		if (mTimeoutCount == MaxTimeoutCount) {
			//if (!mBatteryController->serial().isEmpty()) {
				QLOG_ERROR() << "Lost connection to battery controller";
			// }
			mState = WaitOnConnectionLost;
			delete mService;
			mService = 0;
//			delete mSettings;
//			mSettings = 0;
			mTimeoutCount = MaxTimeoutCount - 1;
			// mBatteryController->setSerial(QString());
			// mBatteryController->setConnectionState(Disconnected);
		} else {
			++mTimeoutCount;
		}
	}
	startNextAction();
}

static bool isBitSet(quint16 value, int bit)
{
	return (value & (1 << bit)) != 0;
}

static int getErrorState(quint16 errorValue, int bit)
{
	return isBitSet(errorValue, bit) ? 2 : 0;
}

static int getWarningState(quint16 errorValue, int bit)
{
	return isBitSet(errorValue, bit) ? 1 : 0;
}

static int getAlarmState(quint16 errorValue, quint16 warningValue, int bit)
{
	return isBitSet(errorValue, bit) ? 2 : (isBitSet(warningValue, bit) ? 1 : 0);
}

void BatteryControllerUpdater::onReadCompleted(int function, quint8 slaveAddress,
											   const QList<quint16> &registers)
{
	if (slaveAddress != mDeviceAddress)
		return;
	// QLOG_WARN() << __FUNCTION__ << slaveAddress;
	Q_UNUSED(function)
	switch (mState) {
	case Serial:
	{
		QString serial = QString::number(registers[0]);
		QString serviceName = QString("com.victronenergy.zbmnode.modbus%1").arg(serial);
		mService = mRoot->itemGetOrCreate(serviceName, false);
		mService->itemGetOrCreateAndProduce("Serial", serial);
		QLOG_INFO() << "Serial number:" << serial;
		// mSettings = new BatteryControllerSettings(0, serial, this);
		mState = Start;
		// mBatteryController->setSerial(serial);
		// mBatteryController->setConnectionState(Detected);
		break;
	}
	case DeviceState:
	{
//		qint16 hwFailure = registers[1];
//		qint16 opFailure = registers[2];
//		qint16 warning = registers[3];
//		mBatteryController->setMaintenanceAlarm(getWarningState(warning, 11));
//		mBatteryController->setMaintenanceActiveAlarm(getWarningState(warning, 10));
//		mBatteryController->setOverCurrentAlarm(getAlarmState(opFailure, warning, 15));
//		mBatteryController->setOverVoltageAlarm(getAlarmState(opFailure, warning, 14));
//		mBatteryController->setBatteryTemperatureAlarm(getAlarmState(opFailure, warning, 13));
//		mBatteryController->setZincPumpAlarm(getErrorState(hwFailure, 15));
//		mBatteryController->setBromidePumpAlarm(getErrorState(hwFailure, 14));
//		mBatteryController->setLeakSensorsAlarm(getErrorState(hwFailure, 11));
//		mBatteryController->setInternalFailureAlarm(getErrorState(hwFailure, 9));
//		mBatteryController->setElectricBoardAlarm(getErrorState(hwFailure, 8));
//		mBatteryController->setElectricBoardAlarm(getErrorState(hwFailure, 8));
//		mBatteryController->setBatteryTemperatureSensorAlarm(getErrorState(hwFailure, 7));
//		mBatteryController->setAirTemperatureSensorAlarm(getErrorState(hwFailure, 6));
//		mBatteryController->setStateOfHealthAlarm(getErrorState(hwFailure, 6));
//		mBatteryController->setLeak1TripAlarm(getErrorState(hwFailure, 4));
//		mBatteryController->setLeak2TripAlarm(getErrorState(hwFailure, 3));
//		mBatteryController->setUnknownAlarm(getAlarmState(hwFailure | opFailure, warning, 0));

		//		mBatteryController->setStsRegSummary(registers[0]);
//		mBatteryController->setStsRegHardwareFailure(registers[1]);
//		mBatteryController->setStsRegOperationalFailure(registers[2]);
//		mBatteryController->setStsRegWarning(registers[3]);
		mState = Measurements;
		break;
	}
	case Measurements:
		mService->itemGetOrCreateAndProduce("Soc", registers[0] / 100.0);
		mService->itemGetOrCreateAndProduce("Dc/0/Voltage", registers[2] / 10.0);
		// mBatteryController->setSOC(registers[0] / 100.0);
//		mBatteryController->setSOCAmpHrs(static_cast<qint16>(registers[1]) / 10.0);
//		mBatteryController->setBattVolts(registers[2] / 10.0);
//		mBatteryController->setBattAmps(-static_cast<qint16>(-registers[3]) / 10.0);
//		mBatteryController->setBattTemp(static_cast<qint16>(registers[4]) / 10.0);
//		mBatteryController->setAirTemp(static_cast<qint16>(registers[5]) / 10.0);
		mState = OperationalMode;
		break;
	case OperationalMode:
		// QLOG_WARN() << __FUNCTION__ << "opmode:" << registers[0];
		mUpdatingController = true;
//		mBatteryController->setOperationalMode(registers[0]);
		mUpdatingController = false;
		mState = Health;
		break;
	case Health:
//		mBatteryController->setHealthIndication(registers[0]);
//		mBatteryController->setBussVolts(registers[1] / 10.0);
//		mBatteryController->setState(registers[2]);
//		mBatteryController->setConnectionState(Connected);
		mState = Wait;
		break;
	case Wait:
		mState = DeviceState;
		break;
	default:
		QLOG_ERROR() << "Unknown updater state" << mState;
		mState = mService == 0 ? Init : Start;
		break;
	}
	mTimeoutCount = 0;
	startNextAction();
}

void BatteryControllerUpdater::onWriteCompleted(int function, quint8 slaveAddress,
										  quint16 address, quint16 value)
{
	if (slaveAddress != mDeviceAddress)
		return;
	QLOG_WARN() << __FUNCTION__ << slaveAddress << address << value;
	Q_UNUSED(function)
	Q_UNUSED(address)
	Q_UNUSED(value)
	switch (mState) {
	case SetAddress:
		mTmpState = Init;
		break;
	case ClearStatus:
//		mBatteryController->setClearStatusRegisterFlags(0);
		break;
	case SetOperationalMode:
		// This is a workaround: the ZBM takes some time to change the
		// operational mode. By setting the mode to wait, we ensure that the
		// operational mode will not be retrieved for 5 seconds, preventing the
		// displayed to switch back temporarily to the previous value.
		mTmpState = Wait;
		break;
	case RequestDelayedMaintenance:
//		mBatteryController->setRequestDelayedSelfMaintenance(0);
		break;
	case RequestImmediateMaintenance:
//		mBatteryController->setRequestImmediateSelfMaintenance(0);
		break;
	default:
		mState = DeviceState;
		break;
	}
	mState = mTmpState;
	mTmpState = Wait;
	mTimeoutCount = 0;
	startNextAction();
}

void BatteryControllerUpdater::onWaitFinished()
{
	switch (mState) {
	case Wait:
		mStopwatch.start();
		mState = Start;
		break;
	case WaitOnConnectionLost:
		mState = Init;
		break;
	default:
		mState = Start;
		break;
	}
	startNextAction();
}

void BatteryControllerUpdater::onClearStatusRegisterFlagsChanged()
{
//	if (mBatteryController->ClearStatusRegisterFlags() != 0)
//		mTmpState = ClearStatus;
}

void BatteryControllerUpdater::onOperationalModeChanged()
{
//	QLOG_WARN() << __FUNCTION__ << mBatteryController->operationalMode() << mUpdatingController;
//	if (mUpdatingController)
//		return;
//	mTmpState = SetOperationalMode;
}

void BatteryControllerUpdater::onRequestDelayedSelfMaintenanceChanged()
{
//	if (mBatteryController->RequestDelayedSelfMaintenance() != 0)
//		mTmpState = RequestDelayedMaintenance;
}

void BatteryControllerUpdater::onRequestImmediateSelfMaintenanceChanged()
{
//	if (mBatteryController->RequestImmediateSelfMaintenance())
//		mTmpState = RequestImmediateMaintenance;
}

void BatteryControllerUpdater::startNextAction()
{
	// QLOG_WARN() << __FUNCTION__ << mState << mTmpState;
	if (mTmpState != Wait) {
		State tmp = mTmpState;
		mTmpState = mState;
		mState = tmp;
	}
	switch (mState) {
	case Serial:
		readRegisters(RegSerial, 1);
		break;
	case DeviceState:
		readRegisters(RegStatusSummary, 4);
		break;
	case OperationalMode:
		readRegisters(RegOperationalMode, 1);
		break;
	case Measurements:
		readRegisters(RegStateOfCharge, 6);
		break;
	case Health:
		readRegisters(RegStateOfHealth, 3);
		break;
	case Wait:
	{
		int dt = MeasurementWaitInterval - mStopwatch.elapsed();
		if (dt > 20) {
			mAcquisitionTimer->setInterval(dt);
			mAcquisitionTimer->start();
		} else {
			onWaitFinished();
		}
		break;
	}
	case WaitOnConnectionLost:
		mAcquisitionTimer->setInterval(ConnectionLostWaitInterval);
		mAcquisitionTimer->start();
		break;
	case SetAddress:
		writeRegister(RegDeviceAddress, mDeviceAddress);
		break;
	case ClearStatus:
		// writeRegister(RegClearStatusFlags, mBatteryController->ClearStatusRegisterFlags());
		break;
	case SetOperationalMode:
		// writeRegister(RegEnterRunCommand, mBatteryController->operationalMode());
		break;
	case RequestDelayedMaintenance:
		// writeRegister(RegDelayedSelfMaintenance, mBatteryController->RequestDelayedSelfMaintenance());
		break;
	case RequestImmediateMaintenance:
		// writeRegister(RegImmediateSelfMaintenance, mBatteryController->RequestImmediateSelfMaintenance());
		break;
	default:
		QLOG_ERROR() << "Invalid state while starting new action" << mState;
		mState = Start;
		startNextAction();
		break;
	}
}

void BatteryControllerUpdater::readRegisters(quint16 startReg, quint16 count)
{
	mModbus->readRegisters(ModbusRtu::ReadHoldingRegisters, mDeviceAddress,
						   startReg, count);
}

void BatteryControllerUpdater::writeRegister(quint16 reg, quint16 value)
{
	QLOG_WARN() << __FUNCTION__ << reg << value;
	mModbus->writeRegister(ModbusRtu::WriteSingleRegister, mDeviceAddress,
						   reg, value);
}
