#ifndef DEVICE_SCANNER_H
#define DEVICE_SCANNER_H

#include <QList>
#include <QObject>

class ModbusRtu;
class QTimer;
class Settings;

/// Finds Redflow batteries via modbus RTU
class DeviceScanner : public QObject
{
	Q_OBJECT
public:
	DeviceScanner(ModbusRtu *modbus, Settings *settings, QObject *parent);

	int scanInterval() const;

	void setScanInterval(int i);

signals:
	void deviceFound(int address);

private slots:
	void onReadCompleted(int function, quint8 slaveAddress, const QList<quint16> &values);

	void onWriteCompleted(int function, quint8 slaveAddress, quint16 address, quint16 value);

	void onErrorReceived(int errorType, quint8 slaveAddress, int exception);

	void onTimer();

private:
	int getNextCandidateAddress() const;

	int getNextScanAddress(int address) const;

	void addNewDevice(int address);

	void scanAddress(int address);

	ModbusRtu *mModbus;
	Settings *mSettings;
	int mScanInterval;
	int mProbedAddress;
	int mNewDeviceAddress;
	int mAutoScanAddress;
	int mMaxAddress;
	QList<int> mDiscoveredDevices;
};

#endif // DEVICE_SCANNER_H
