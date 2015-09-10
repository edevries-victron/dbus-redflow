#ifndef BATTERY_CONTROLLER_UPDATER_H
#define BATTERY_CONTROLLER_UPDATER_H

#include <QElapsedTimer>
#include <QObject>
#include "defines.h"
#include "modbus_rtu.h"

class BatteryController;
class BatteryControllerSettings;

/*!
 * Retrieves data from a Carlo Gavazzi energy meter.
 * This class will setup a connection (modbus RTU) to an energy meter and
 * retrieve data from the device. The value will be stored in an `AcSensor`
 * object.
 *
 * This class is implemented as a state engine. The diagram below shows the
 * progress through the states.
 * @dotfile battery_controller_updater_states.dot
 */
class BatteryControllerUpdater : public QObject
{
	Q_OBJECT
public:
	/*!
	 * Creates an instance of `BatteryControllerUpdater`, and starts the setup
	 * process.
	 * If the setup succeeds, the `connectionState` of the `acSensor` will be
	 * set to `Detected`, otherwise to `disconnected` signal.
	 * Once the state is `Detected`, the object will become idle until
	 * `startMeasurement` is called.
	 * @param acSensor. The storage object. All retrieved values will be
	 * stored here. The `portName` property of this object should be set before
	 * calling this contructor.
	 * @param modbus. The modbus connection object. This object may be shared
	 * between multiple `AcSensorUpdater` objects. The `modbus` object will not
	 * be deleted in the destructor.
	 */
	BatteryControllerUpdater(BatteryController *mBatteryController,
							 ModbusRtu *modbus, QObject *parent = 0);

	/*!
	 * Returns the settings object.
	 * This object is created by the `AcSensorUpdater` when a device has been
	 * detected. It will be null while the `connectionState` of the `AcSensor`
	 * is `Disconnected` or `Searched`.
	 * The information in the settings will be used for data retrieval and can
	 * be changed while retrieval is active. This may lead to reinitialization
	 * of the updater (and the energy meter itself).
	 */
	BatteryControllerSettings *settings();

private slots:
	void onErrorReceived(int errorType, quint8 addr, int exception);

	void onReadCompleted(int function, quint8 addr, const QList<quint16> &registers);

	void onWriteCompleted(int function, quint8 addr, quint16 address, quint16 value);

	void onWaitFinished();

	void onClearStatusRegisterFlagsChanged();

	void onOperationalModeChanged();

	void onRequestDelayedSelfMaintenanceChanged();

	void onRequestImmediateSelfMaintenanceChanged();

	void onDeviceAddressChanged();

private:
	enum State {
		Serial,
		FirmwareVersion,
		DeviceState,
		Measurements,
		OperationalMode,
		Health,
		Wait,
		WaitOnConnectionLost,

		SetAddress,
		SetOperationalMode,
		ClearStatus,
		RequestDelayedMaintenance,
		RequestImmediateMaintenance,

		Init = Serial,
		Start = DeviceState
	};

	void startNextAction();

	void queueWriteAction(State writeState);

	void readRegisters(quint16 startReg, quint16 count);

	void writeRegister(quint16 reg, quint16 value);

	BatteryController *mBatteryController;
	int mDeviceAddress;
	bool mUpdatingController;
	BatteryControllerSettings *mSettings;
	ModbusRtu *mModbus;
	QTimer *mAcquisitionTimer;
	int mTimeoutCount;
	QElapsedTimer mStopwatch;
	State mState;
	State mTmpState;
};

#endif // BATTERY_CONTROLLER_UPDATER_H
