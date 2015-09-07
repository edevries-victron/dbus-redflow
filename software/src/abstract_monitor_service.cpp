#include <QCoreApplication>
#include <QStringList>
#include <QsLog.h>
#include <QTimer>
#include <velib/qt/ve_qitem.hpp>
#include "abstract_monitor_service.h"
#include "batteryController.h"
#include "version.h"

AbstractMonitorService::AbstractMonitorService(VeQItem *root, QObject *parent):
	QObject(parent),
	mRoot(root),
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
//	if (mServiceRegistered) {
//		QLOG_INFO() << "Unregistering service" << mServiceName;
//		QDBusConnection connection = VBusItems::getConnection(mServiceName);
//		connection.unregisterService(mServiceName);
//	}
}

void AbstractMonitorService::addBattery(BatteryController *c)
{
	mControllers.append(c);
	connect(c, SIGNAL(destroyed()), this, SLOT(onDestroyed()));
	updateValues();
}

void AbstractMonitorService::registerService()
{
//	if (!mConnection.registerService(mServiceName)) {
//		QLOG_ERROR() << "Could not register D-Bus service:" << mServiceName;
//		return;
//	}
//	mServiceRegistered = true;
}

VeQItem *AbstractMonitorService::produce(const QString &path,
										  const QVariant &value,
										  const QString &unit,
										  int precision)
{
	VeQItem *item = mRoot->itemGetOrCreateAndProduce(path, value);
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
