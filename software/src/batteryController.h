#ifndef BATTERY_CONTROLLER_H
#define BATTERY_CONTROLLER_H

#include <QMetaType>
#include <QObject>
#include "defines.h"

enum ConnectionState {
	Disconnected,
	Detected,
	Connected
};

Q_DECLARE_METATYPE(ConnectionState)

class BatteryController : public QObject
{
	Q_OBJECT
	Q_PROPERTY(double BattAmps READ BattAmps WRITE setBattAmps NOTIFY battAmpsChanged)
	Q_PROPERTY(double BussAmps READ BussAmps WRITE setBussAmps NOTIFY bussAmpsChanged)
	Q_PROPERTY(double BattVolts READ BattVolts WRITE setBattVolts NOTIFY battVoltsChanged)
	Q_PROPERTY(double BattTemp READ BattTemp WRITE setBattTemp NOTIFY battTempChanged)
	Q_PROPERTY(double BattPower READ BattPower NOTIFY battPowerChanged)
	Q_PROPERTY(double SOC READ SOC WRITE setSOC NOTIFY socChanged);
	Q_PROPERTY(ConnectionState connectionState READ connectionState WRITE setConnectionState NOTIFY connectionStateChanged)
	Q_PROPERTY(int deviceType READ deviceType WRITE setDeviceType NOTIFY deviceTypeChanged)
	Q_PROPERTY(QString productName READ productName)
	Q_PROPERTY(QString serial READ serial WRITE setSerial NOTIFY serialChanged)
	Q_PROPERTY(QString firmwareVersion READ firmwareVersion WRITE setFirmwareVersion NOTIFY firmwareVersionChanged)
	Q_PROPERTY(QString portName READ portName)
	Q_PROPERTY(int operationalMode READ operationalMode WRITE setOperationalMode NOTIFY operationalModeChanged)
//	Q_PROPERTY(int StsRegSummary READ StsRegSummary WRITE setStsRegSummary NOTIFY stsRegSummaryChanged)
//	Q_PROPERTY(int StsRegOperationalFailure READ StsRegOperationalFailure WRITE setStsRegOperationalFailure NOTIFY stsRegOperationalFailureChanged)
//	Q_PROPERTY(int StsRegHardwareFailure READ StsRegHardwareFailure WRITE setStsRegHardwareFailure NOTIFY stsRegHardwareFailureChanged)
//	Q_PROPERTY(int StsRegWarning READ StsRegWarning WRITE setStsRegWarning NOTIFY stsRegWarningChanged)
	Q_PROPERTY(double SOCAmpHrs READ SOCAmpHrs WRITE setSOCAmpHrs NOTIFY socAmpHrsChanged)
	Q_PROPERTY(double AirTemp READ AirTemp WRITE setAirTemp NOTIFY airTempChanged)
	Q_PROPERTY(double HealthIndication READ HealthIndication WRITE setHealthIndication NOTIFY healthIndicationChanged)
	Q_PROPERTY(double BussVolts READ BussVolts WRITE setBussVolts NOTIFY bussVoltsChanged)
	Q_PROPERTY(int State READ State WRITE setState NOTIFY stateChanged)
	Q_PROPERTY(int DeviceAddress READ DeviceAddress WRITE setDeviceAddress NOTIFY deviceAddressChanged)
	Q_PROPERTY(int ClearStatusRegisterFlags READ ClearStatusRegisterFlags WRITE setClearStatusRegisterFlags NOTIFY clearStatusRegisterFlagsChanged)
	Q_PROPERTY(int RequestDelayedSelfMaintenance READ RequestDelayedSelfMaintenance WRITE setRequestDelayedSelfMaintenance NOTIFY requestDelayedSelfMaintenanceChanged)
//	Q_PROPERTY(int SetOperationalMode READ SetOperationalMode WRITE setSetOperationalMode NOTIFY setOperationalModeChanged)
	Q_PROPERTY(int RequestImmediateSelfMaintenance READ RequestImmediateSelfMaintenance WRITE setRequestImmediateSelfMaintenance NOTIFY requestImmediateSelfMaintenanceChanged)

	Q_PROPERTY(int hasAlarm READ hasAlarm WRITE setHasAlarm NOTIFY hasAlarmChanged)
	Q_PROPERTY(int maintenanceAlarm READ maintenanceAlarm WRITE setMaintenanceAlarm NOTIFY maintenanceAlarmChanged)
	Q_PROPERTY(int maintenanceActiveAlarm READ maintenanceActiveAlarm WRITE setMaintenanceActiveAlarm NOTIFY maintenanceActiveAlarmChanged)
	Q_PROPERTY(int overCurrentAlarm READ overCurrentAlarm WRITE setOverCurrentAlarm NOTIFY overCurrentAlarmChanged)
	Q_PROPERTY(int overVoltageAlarm READ overVoltageAlarm WRITE setOverVoltageAlarm NOTIFY overVoltageAlarmChanged)
	Q_PROPERTY(int batteryTemperatureAlarm READ batteryTemperatureAlarm WRITE setBatteryTemperatureAlarm NOTIFY batteryTemperatureAlarmChanged)
	Q_PROPERTY(int zincPumpAlarm READ zincPumpAlarm WRITE setZincPumpAlarm NOTIFY zincPumpAlarmChanged)
	Q_PROPERTY(int bromidePumpAlarm READ bromidePumpAlarm WRITE setBromidePumpAlarm NOTIFY bromidePumpAlarmChanged)
	Q_PROPERTY(int leakSensorsAlarm READ leakSensorsAlarm WRITE setLeakSensorsAlarm NOTIFY leakSensorsAlarmChanged)
	Q_PROPERTY(int internalFailureAlarm READ internalFailureAlarm WRITE setInternalFailureAlarm NOTIFY internalFailureAlarmChanged)
	Q_PROPERTY(int electricBoardAlarm READ electricBoardAlarm WRITE setElectricBoardAlarm NOTIFY electricBoardAlarmChanged)
	Q_PROPERTY(int batteryTemperatureSensorAlarm READ batteryTemperatureSensorAlarm WRITE setBatteryTemperatureSensorAlarm NOTIFY batteryTemperatureSensorAlarmChanged)
	Q_PROPERTY(int airTemperatureSensorAlarm READ airTemperatureSensorAlarm WRITE setAirTemperatureSensorAlarm NOTIFY airTemperatureSensorAlarmChanged)
	Q_PROPERTY(int stateOfHealthAlarm READ stateOfHealthAlarm WRITE setStateOfHealthAlarm)
	Q_PROPERTY(int leak1TripAlarm READ leak1TripAlarm WRITE setLeak1TripAlarm)
	Q_PROPERTY(int leak2TripAlarm READ leak2TripAlarm WRITE setLeak2TripAlarm)
	Q_PROPERTY(int unknownAlarm READ unknownAlarm WRITE setUnknownAlarm)

signals:
	void battAmpsChanged();
	void battVoltsChanged();
	void battTempChanged();
	void bussAmpsChanged();
	void bussVoltsChanged();
	void airTempChanged();
	void socChanged();
	void battPowerChanged();

	void stsRegSummaryChanged();
	void stsRegHardwareFailureChanged();
	void stsRegOperationalFailureChanged();
	void stsRegWarningChanged();
	void operationalModeChanged();
	void socAmpHrsChanged();
	void healthIndicationChanged();
	void stateChanged();
	void deviceAddressChanged();
	void clearStatusRegisterFlagsChanged();
//	void setOperationalModeChanged();
	void requestDelayedSelfMaintenanceChanged();
	void requestImmediateSelfMaintenanceChanged();

	void hasAlarmChanged();
	void maintenanceAlarmChanged();
	void maintenanceActiveAlarmChanged();
	void overCurrentAlarmChanged();
	void overVoltageAlarmChanged();
	void batteryTemperatureAlarmChanged();
	void zincPumpAlarmChanged();
	void bromidePumpAlarmChanged();
	void leakSensorsAlarmChanged();
	void internalFailureAlarmChanged();
	void electricBoardAlarmChanged();
	void batteryTemperatureSensorAlarmChanged();
	void airTemperatureSensorAlarmChanged();
	void stateOfHealthAlarmChanged();
	void leak1TripAlarmChanged();
	void leak2TripAlarmChanged();
	void unknownAlarmChanged();
public:
	BatteryController(const QString &portName, int deviceAddress, QObject *parent = 0);

	ConnectionState connectionState() const;

	void setConnectionState(ConnectionState state);

	int deviceType() const;

	void setDeviceType(int t);

	QString productName() const;

	QString serial() const;

	void setSerial(const QString &s);

	QString firmwareVersion() const;

	void setFirmwareVersion(const QString &v);

	double BattVolts() const;

	void setBattVolts(double t);

	double BattAmps() const;

	void setBattAmps(double t);

	double BussVolts() const;

	void setBussVolts(double t);

	double BussAmps() const;

	void setBussAmps(double t);

	double BattTemp() const;

	void setBattTemp(double t);

	double AirTemp() const;

	void setAirTemp(double t);

	double SOC() const;

	void setSOC(double t);

	double BattPower() const;

	int StsRegSummary() const;
	void setStsRegSummary(int t);
	int StsRegHardwareFailure() const;
	void setStsRegHardwareFailure(int t);
	int StsRegOperationalFailure() const;
	void setStsRegOperationalFailure(int t);
	int StsRegWarning() const;
	void setStsRegWarning(int t);
	int operationalMode() const;
	void setOperationalMode(int t);
	double SOCAmpHrs() const;
	void setSOCAmpHrs(double t);
	double HealthIndication() const;
	void setHealthIndication(double t);
	int State() const;
	void setState(int t);
	int DeviceAddress() const;
	void setDeviceAddress(int t);
	int ClearStatusRegisterFlags() const;
	void setClearStatusRegisterFlags(int t);
	int RequestDelayedSelfMaintenance() const;
	void setRequestDelayedSelfMaintenance(int t);
//	int SetOperationalMode() const;
//	void setSetOperationalMode(int t);
	int RequestImmediateSelfMaintenance() const;
	void setRequestImmediateSelfMaintenance(int t);

	int hasAlarm() const;
	void setHasAlarm(int a);
	int maintenanceAlarm() const;
	void setMaintenanceAlarm(int v);
	int maintenanceActiveAlarm() const;
	void setMaintenanceActiveAlarm(int v);
	int overCurrentAlarm() const;
	void setOverCurrentAlarm(int v);
	int overVoltageAlarm() const;
	void setOverVoltageAlarm(int v);
	int batteryTemperatureAlarm() const;
	void setBatteryTemperatureAlarm(int v);
	int zincPumpAlarm() const;
	void setZincPumpAlarm(int v);
	int bromidePumpAlarm() const;
	void setBromidePumpAlarm(int v);
	int leakSensorsAlarm() const;
	void setLeakSensorsAlarm(int v);
	int internalFailureAlarm() const;
	void setInternalFailureAlarm(int v);
	int electricBoardAlarm() const;
	void setElectricBoardAlarm(int v);
	int batteryTemperatureSensorAlarm() const;
	void setBatteryTemperatureSensorAlarm(int v);
	int airTemperatureSensorAlarm() const;
	void setAirTemperatureSensorAlarm(int v);
	int stateOfHealthAlarm() const;
	void setStateOfHealthAlarm(int v);
	int leak1TripAlarm() const;
	void setLeak1TripAlarm(int v);
	int leak2TripAlarm() const;
	void setLeak2TripAlarm(int v);
	int unknownAlarm() const;
	void setUnknownAlarm(int v);

	/*!
	 * Returns the logical name of the communication port. (eg. /dev/ttyUSB1).
	 */
	QString portName() const;

signals:
	void connectionStateChanged();

	void deviceTypeChanged();

	void serialChanged();

	void firmwareVersionChanged();

	void errorCodeChanged();

private:
	ConnectionState mConnectionState;
	int mDeviceType;
	QString mFirmwareVersion;
	QString mPortName;
	QString mSerial;
	double mBattVolts;
	double mBussVolts;
	double mBattAmps;
	double mBussAmps;
	double mBattTemp;
	double mAirTemp;
	double mSOC;
	int mStsRegSummary;
	int mStsRegHardwareFailure;
	int mStsRegOperationalFailure;
	int mStsRegWarning;
	int mOperationalMode;
	double mSOCAmpHrs;
	double mHealthIndication;
	int mState;
	int mDeviceAddress;
	int mClearStatusRegisterFlags;
	int mRequestDelayedSelfMaintenance;
	int mRequestImmediateSelfMaintenance;

	int mHasAlarm;
	int mMaintenanceAlarm;
	int mMaintenanceActiveAlarm;
	int mOverCurrentAlarm;
	int mOverVoltageAlarm;
	int mBatteryTemperatureAlarm;
	int mZincPumpAlarm;
	int mBromidePumpAlarm;
	int mLeakSensorsAlarm;
	int mInternalFailureAlarm;
	int mElectricBoardAlarm;
	int mBatteryTemperatureSensorAlarm;
	int mAirTemperatureSensorAlarm;
	int mStateOfHealthAlarm;
	int mLeak1TripAlarm;
	int mLeak2TripAlarm;
	int mUnknownAlarm;
};

#endif // BATTERY_CONTROLLER_H
