#ifndef BATTERY_CONTROLLER_H
#define BATTERY_CONTROLLER_H

#include <QMetaType>
#include <QObject>
#include "defines.h"

enum ConnectionState {
	Disconnected,
	Searched,
	Detected,
	Connected
};

Q_DECLARE_METATYPE(ConnectionState)


class BatteryController : public QObject
{
	Q_OBJECT
	Q_PROPERTY(double BattAmps READ BattAmps WRITE setBattAmps NOTIFY battAmpsChanged)
	Q_PROPERTY(int BattVolts READ BattVolts WRITE setBattVolts NOTIFY battVoltsChanged)
	Q_PROPERTY(double BattTemp READ BattTemp WRITE setBattTemp NOTIFY battTempChanged)
	Q_PROPERTY(int BattPower READ BattPower WRITE setBattPower NOTIFY battPowerChanged)
	Q_PROPERTY(int SOC READ SOC WRITE setSOC NOTIFY socChanged);
	Q_PROPERTY(ConnectionState connectionState READ connectionState WRITE setConnectionState NOTIFY connectionStateChanged)
	Q_PROPERTY(int deviceType READ deviceType WRITE setDeviceType NOTIFY deviceTypeChanged)
	Q_PROPERTY(int deviceSubType READ deviceSubType WRITE setDeviceSubType NOTIFY deviceSubTypeChanged)
	Q_PROPERTY(QString productName READ productName)
	Q_PROPERTY(QString serial READ serial WRITE setSerial NOTIFY serialChanged)
	Q_PROPERTY(int firmwareVersion READ firmwareVersion WRITE setFirmwareVersion NOTIFY firmwareVersionChanged)
	Q_PROPERTY(int errorCode READ errorCode WRITE setErrorCode NOTIFY errorCodeChanged)
	Q_PROPERTY(QString portName READ portName)

signals:
	void battAmpsChanged();

	void battVoltsChanged();

	void battTempChanged();

	void bussAmpsChanged();

	void bussVoltsChanged();

	void airTempChanged();

	void socChanged();

	void battPowerChanged();
	
public:
	BatteryController(const QString &portName, int slaveAddress, QObject *parent = 0);

	ConnectionState connectionState() const;

	void setConnectionState(ConnectionState state);

	/*!
	 * Returns the device type as reported by the energy meter.
	 */
	int deviceType() const;

	void setDeviceType(int t);

	/*!
	 * Returns the device sub type as reported by the energy meter. This value
	 * is non-zero for EM24 energy meters only.
	 */
	int deviceSubType() const;

	void setDeviceSubType(int t);

	/*!
	 * Returns the full official producet name of the energy meter.
	 * For example: EM24=DIN.AV9.3.X.IS.X.
	 * The return value is based on the value of `deviceType` and
	 * `subDeviceType`.
	 */
	QString productName() const;

	/*!
	 * Returned the serial number of the energy meter.
	 */
	QString serial() const;

	void setSerial(const QString &s);

	int firmwareVersion() const;

	void setFirmwareVersion(int v);

	int BattVolts() const;

	void setBattVolts(int t);

	double BattAmps() const;

	void setBattAmps(int t);

	int BussVolts() const;

	void setBussVolts(int t);

	int BussAmps() const;

	void setBussAmps(int t);

	double BattTemp() const;

	void setBattTemp(int t);

	int AirTemp() const;

	void setAirTemp(int t);

	int SOC() const;

	void setSOC(int t);

	int BattPower() const;

	void setBattPower(int t);


	/*!
	 * Returns the error code.
	 * Possible values:
	 * - 0: no error
	 * - 1: front selector is locked (EM24 only). When the selector is locked,
	 * in is not possible to change settings in the energy meter. Relevant
	 * settings are: power returned to grid should be negative (application H)
	 * and (optionnally) change between single phase and multi phase
	 * measurement.
	 */
	int errorCode() const;

	void setErrorCode(int code);

	/*!
	 * Returns the logical name of the communication port. (eg. /dev/ttyUSB1).
	 */
	QString portName() const;

	int slaveAddress() const;

	/*!
	 * Reset all measured values to NaN
	 */
	void resetValues();

signals:
	void connectionStateChanged();

	void deviceTypeChanged();

	void deviceSubTypeChanged();

	void serialChanged();

	void firmwareVersionChanged();

	void errorCodeChanged();

private:
	ConnectionState mConnectionState;
	int mDeviceType;
	int mDeviceSubType;
	int mErrorCode;
	int mFirmwareVersion;
	QString mPortName;
	int mSlaveAddress;
	QString mSerial;
	int mBattVolts;
	int mBussVolts;
	int mBattAmps;
	int mBussAmps;
	int mBattTemp;
	int mAirTemp;
	int mBattPower;
	int mSOC;
};

#endif // BATTERY_CONTROLLER_H
