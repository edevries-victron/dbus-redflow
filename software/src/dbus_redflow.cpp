#include <QsLog.h>
#include "battery_controller_bridge.h"
#include "battery_controller_settings.h"
#include "battery_controller_settings_bridge.h"
#include "battery_controller_updater.h"
#include "battery_summary.h"
#include "dbus_redflow.h"
#include "dbus_service_monitor.h"
#include "settings.h"
#include "settings_bridge.h"
#include "batteryController.h"
#include "battery_summary.h"
#include "bms_service.h"

DBusRedflow::DBusRedflow(const QString &portName, QObject *parent):
	QObject(parent),
	mModbus(new ModbusRtu(portName, 19200, this)),
	mSummary(new BatterySummary(this)),
	mBmsService(new BmsService(this))
{
	qRegisterMetaType<ConnectionState>();
	qRegisterMetaType<QList<quint16> >();

	for (int i=1; i<5; ++i) {
		BatteryController *m = new BatteryController(portName, i, this);
		new BatteryControllerUpdater(m, mModbus, m);
		mBatteryControllers.append(m);
		connect(m, SIGNAL(connectionStateChanged()),
				this, SLOT(onConnectionStateChanged()));
	}
	
	mSettings = new Settings(this);
	new SettingsBridge(mSettings, this);

	connect(mModbus, SIGNAL(serialEvent(const char *)),
			this, SLOT(onSerialEvent(const char *)));
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
	mSettings->registerDevice(m->serial());
}

void DBusRedflow::onDeviceSettingsInitialized()
{
}

void DBusRedflow::onDeviceInitialized()
{
	BatteryController *m = static_cast<BatteryController *>(sender());
	BatteryControllerUpdater *mu = m->findChild<BatteryControllerUpdater *>();
	new BatteryControllerBridge(m, mu->settings(), m);
	mSummary->addBattery(m);
	mBmsService->addBattery(m);
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
