#include <QCoreApplication>
#include <QStringList>
#include <QsLog.h>
#include <QTimer>
#include <velib/qt/v_busitem.h>
#include <velib/qt/v_busitems.h>
#include "abstract_monitor_service.h"
#include "batteryController.h"
#include "version.h"
#include "v_bus_node.h"

AbstractMonitorService::AbstractMonitorService(const QString &serviceName,
											   QObject *parent):
	QObject(parent),
	mRoot(0),
	mConnection(VBusItems::getConnection("serviceName")),
	mServiceName(serviceName),
	mServiceRegistered(false)
{
	QString processName = QCoreApplication::arguments()[0];
	// The D-Bus paths /Mgmt/Connection, /ProductName, and /Connected are used
	// by system-calc to determine whether a service is connected.
	// /Connected must be 1. /Mgmt/Connection and /ProductName must exist and
	// be valid.
	produce("/Mgmt/ProcessName", processName);
	produce("/Mgmt/ProcessVersion", VERSION);
	produce("/Mgmt/Connection", "None");
	produce("/Connected", 1);
	produce("/ProductName", "ZBM summary");
	// produce("/FirmwareVersion", BatteryController->firmwareVersion());
	// produce("/ProductId", VE_PROD_ID_REDFLOW_ZBM2);
	// produce("/DeviceType", BatteryController->deviceType());
	// produce("/DeviceInstance", deviceInstance);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	timer->setInterval(1000);
	timer->start();
}

AbstractMonitorService::~AbstractMonitorService()
{
	if (mServiceRegistered)
		mConnection.unregisterService(mServiceName);
}

void AbstractMonitorService::addBattery(const BatteryController *c)
{
	mControllers.append(c);
	connect(c, SIGNAL(destroyed()), this, SLOT(onDestroyed()));
	updateValues();
}

void AbstractMonitorService::registerService()
{
	if (!mConnection.registerService(mServiceName)) {
		QLOG_ERROR() << "Could not register D-Bus service:" << mServiceName;
		return;
	}
	mServiceRegistered = true;
}

VBusItem *AbstractMonitorService::produce(const QString &path,
										  const QVariant &value,
										  const QString &unit,
										  int precision)
{
	VBusItem *item = new VBusItem(this);
	item->produce(mConnection, path, "", value, unit, precision);
	if (mRoot == 0) {
		mRoot = new VBusNode(mConnection, "/", this);
	}
	mRoot->addChild(path, item);
	return item;
}

const QList<const BatteryController *> &AbstractMonitorService::controllers() const
{
	return mControllers;
}

void AbstractMonitorService::onTimer()
{
	updateValues();
}

void AbstractMonitorService::onDestroyed()
{
	mControllers.removeOne(static_cast<BatteryController *>(sender()));
}
