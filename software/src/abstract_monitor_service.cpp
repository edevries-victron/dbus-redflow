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
	mServiceName(serviceName),
	mServiceRegistered(false)
{
	QString processName = QCoreApplication::arguments()[0];
	produce("/Mgmt/ProcessName", processName);
	produce("/Mgmt/ProcessVersion", VERSION);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	timer->setInterval(1000);
	timer->start();
}

AbstractMonitorService::~AbstractMonitorService()
{
	if (mServiceRegistered) {
		QLOG_INFO() << "Unregistering service" << mServiceName;
		QDBusConnection connection = VBusItems::getConnection(mServiceName);
		connection.unregisterService(mServiceName);
	}
}

void AbstractMonitorService::addBattery(BatteryController *c)
{
	mControllers.append(c);
	connect(c, SIGNAL(destroyed()), this, SLOT(onDestroyed()));
	updateValues();
}

void AbstractMonitorService::registerService()
{
	if (mServiceRegistered)
		return;
	QLOG_INFO() << "Registering service" << mServiceName;
	QDBusConnection connection = VBusItems::getConnection(mServiceName);
	if (!connection.registerService(mServiceName)) {
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
	QDBusConnection connection = VBusItems::getConnection(mServiceName);
	item->produce(connection, path, "", value, unit, precision);
	if (mRoot == 0) {
		mRoot = new VBusNode(connection, "/", this);
	}
	mRoot->addChild(path, item);
	return item;
}

const QList<BatteryController *> &AbstractMonitorService::controllers() const
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
