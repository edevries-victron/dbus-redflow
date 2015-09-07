#include <QsLog.h>
#include <QTimer>
#include "device_scanner.h"
#include "modbus_rtu.h"

DeviceScanner::DeviceScanner(ModbusRtu *modbus, Settings *settings, QObject *parent):
	QObject(parent),
	mModbus(modbus),
	mSettings(settings),
	mScanInterval(0),
	mProbedAddress(0),
	mNewDeviceAddress(0),
	mAutoScanAddress(1),
	mMaxAddress(1)
{
	Q_ASSERT(modbus != 0);
	Q_ASSERT(settings != 0);
	scanAddress(mAutoScanAddress);
	connect(mModbus, SIGNAL(readCompleted(int, quint8, const QList<quint16> &)),
			this, SLOT(onReadCompleted(int, quint8, QList<quint16>)));
	connect(mModbus, SIGNAL(writeCompleted(int, quint8, quint16, quint16)),
			this, SLOT(onWriteCompleted(int, quint8, quint16, quint16)));
	connect(mModbus, SIGNAL(errorReceived(int, quint8, int)),
			this, SLOT(onErrorReceived(int, quint8, int)));
}

int DeviceScanner::scanInterval() const
{
	return mScanInterval;
}

void DeviceScanner::setScanInterval(int i)
{
	mScanInterval = i;
}

void DeviceScanner::onReadCompleted(int function, quint8 slaveAddress,
									const QList<quint16> &values)
{
	Q_UNUSED(function);
	Q_UNUSED(values);
	if (slaveAddress != mProbedAddress)
		return;
	// We have a successful read. There are several options here:
	// * We found a new device mProbedAddress with default address (1 or 99)
	//   * Create a new candidate address (max address + 1) and try to read from
	//     it. On timeout, we know the address is not in use, so we can set
	//     it as new address for the device we just found.
	// * We found an existing device while checking a candidate
	//   * Create a new candidate and try again
	// * We found an existing device during a random device scan
	//   * Use straight away.
	QLOG_WARN() << __FUNCTION__ << "Found device:" << mProbedAddress;
	if (mProbedAddress == 1 || mProbedAddress == 99) {
		mNewDeviceAddress = mProbedAddress;
		scanAddress(getNextCandidateAddress());
	} else if (mNewDeviceAddress > 0) {
		addNewDevice(mProbedAddress);
		scanAddress(getNextCandidateAddress());
	} else {
		addNewDevice(mProbedAddress);
		mAutoScanAddress = getNextScanAddress(mAutoScanAddress);
		scanAddress(mAutoScanAddress);
	}
}

void DeviceScanner::onWriteCompleted(int function, quint8 slaveAddress,
									 quint16 address, quint16 value)
{
	Q_UNUSED(function)
	Q_UNUSED(value)
	QLOG_WARN() << __FUNCTION__ << slaveAddress << address << value;
	if (slaveAddress != mNewDeviceAddress || address != 0x9030)
		return;
	// We get here if mCandidate address has been written to the device.
	// Let's find out if the operation was successful.
	mMaxAddress = qMax(mMaxAddress, mProbedAddress);
	mNewDeviceAddress = 0;
	scanAddress(mProbedAddress);
}

void DeviceScanner::onErrorReceived(int errorType, quint8 slaveAddress,
									int exception)
{
	Q_UNUSED(exception);
	if (slaveAddress != mProbedAddress)
		return;
	/// @todo EV This may also be a write error.
	if (errorType == ModbusRtu::Timeout) {
		// No device found. Options:
		// * No device found at candidate address. We can use this address for
		//   new device.
		// * Random device scan failed to find a device, try next address
		if (mNewDeviceAddress > 0) {
			QLOG_WARN() << __FUNCTION__ << "Change modbus address from"
						<< mNewDeviceAddress << "to" << mProbedAddress;
			mModbus->writeRegister(ModbusRtu::WriteSingleRegister,
								   mNewDeviceAddress, 0x9030, mProbedAddress);
		} else {
			scanAddress(getNextScanAddress(mProbedAddress));
		}
	} else {
		scanAddress(mProbedAddress);
	}
}

void DeviceScanner::onTimer()
{
	mModbus->readRegisters(ModbusRtu::ReadHoldingRegisters, mProbedAddress, 0x9010, 1);
}

int DeviceScanner::getNextCandidateAddress() const
{
	return getNextScanAddress(mMaxAddress);
}

int DeviceScanner::getNextScanAddress(int address) const
{
	for (int a = address + 1;; ++a) {
		if (a > 254)
			a = 2;
		if (a != 1 && a != 90 && !mDiscoveredDevices.contains(a))
			return a;
	}
}

void DeviceScanner::addNewDevice(int address)
{
	QLOG_WARN() << __FUNCTION__ << "New device:" << address;
	mDiscoveredDevices.append(address);
	mMaxAddress = qMax(mMaxAddress, address);
	emit deviceFound(address);
}

void DeviceScanner::scanAddress(int address)
{
	QLOG_WARN() << __FUNCTION__ << "Scanning address:" << address;
	mProbedAddress = address;
	if (mScanInterval < 250) {
		mModbus->readRegisters(ModbusRtu::ReadHoldingRegisters, address, 0x9010, 1);
	} else {
		QTimer::singleShot(mScanInterval - 250, this, SLOT(onTimer()));
	}
}
