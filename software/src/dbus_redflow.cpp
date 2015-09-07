#include <QsLog.h>
#include "battery_controller_bridge.h"
#include "battery_controller_settings.h"
#include "battery_controller_settings_bridge.h"
#include "battery_controller_updater.h"
#include "batteryController.h"
#include "battery_summary.h"
#include "bms_service.h"
#include "dbus_redflow.h"
#include "dbus_service_monitor.h"
#include "device_scanner.h"
#include "settings.h"
#include "settings_bridge.h"

DBusRedflow::DBusRedflow(const QString &portName, QObject *parent):
	QObject(parent),
	mModbus(new ModbusRtu(portName, 19200, this)),
	mPortName(portName),
	mSummary(0),
	mBmsService(0)
{
	qRegisterMetaType<ConnectionState>();
	qRegisterMetaType<QList<quint16> >();

	mSettings = new Settings(this);
	new SettingsBridge(mSettings, this);

	connect(mModbus, SIGNAL(serialEvent(const char *)),
			this, SLOT(onSerialEvent(const char *)));

	mDeviceScanner = new DeviceScanner(mModbus, mSettings, this);
	connect(mDeviceScanner, SIGNAL(deviceFound(int)), this, SLOT(onDeviceFound(int)));
}

void DBusRedflow::onDeviceFound(int address)
{
	BatteryController *m = new BatteryController(mPortName, address, this);
	new BatteryControllerUpdater(m, mModbus, m);
	mBatteryControllers.append(m);
	connect(m, SIGNAL(connectionStateChanged()),
			this, SLOT(onConnectionStateChanged()));
	mDeviceScanner->setScanInterval(2000);
}

void DBusRedflow::onConnectionStateChanged()
{
	BatteryController *m = static_cast<BatteryController *>(sender());
	switch (m->connectionState()) {
	case Disconnected:
		onConnectionLost();
		break;
	case Detected:
		onDeviceFound();
		break;
	case Connected:
		onDeviceInitialized();
		break;
	}
}

void DBusRedflow::onDeviceFound()
{
	BatteryController *m = static_cast<BatteryController *>(sender());
	BatteryControllerUpdater *mu = m->findChild<BatteryControllerUpdater *>();
	QLOG_INFO() << "Device found:" << m->serial()
				<< '@' << m->portName();
	BatteryControllerSettings *settings = mu->settings();
	settings->setParent(m);
//	BatteryControllerSettingsBridge *b =
//			new BatteryControllerSettingsBridge(settings, settings);
//	connect(b, SIGNAL(initialized()),
//			this, SLOT(onDeviceSettingsInitialized()));
//	mSettings->registerDevice(m->serial());
}

void DBusRedflow::onDeviceSettingsInitialized()
{
}

void DBusRedflow::onDeviceInitialized()
{
	BatteryController *m = static_cast<BatteryController *>(sender());
	BatteryControllerUpdater *mu = m->findChild<BatteryControllerUpdater *>();
	new BatteryControllerBridge(m, mu->settings(), m);
	if (mSummary == 0) {
		mSummary = new BatterySummary(this);
		// Make sure we add the batter before registration. The addBattery
		// function will update the values within the summary, so we avoid
		// registering a service without valid values.
		mSummary->addBattery(m);
		mSummary->registerService();
	} else {
		mSummary->addBattery(m);
	}
	if (mBmsService == 0) {
		mBmsService = new BmsService(this);
		mSummary->addBattery(m);
		mBmsService->registerService();
	} else {
		mBmsService->addBattery(m);
	}
}

void DBusRedflow::onConnectionLost()
{
	foreach (BatteryController *c, mBatteryControllers) {
		if (c->connectionState() != Disconnected)
			return;
	}
	QLOG_ERROR() << "No more batteries connected. Application will shut down.";
	exit(1);
}

void DBusRedflow::onSerialEvent(const char *description)
{
	QLOG_ERROR() << "Serial event:" << description
				 << "Application will shut down.";
	exit(1);
}
