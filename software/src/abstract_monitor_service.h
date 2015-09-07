#ifndef MONITORSERVICE_H
#define MONITORSERVICE_H

#include <QDBusConnection>
#include <QObject>
#include <QList>

class BatteryController;
class VBusItem;
class VBusNode;

class AbstractMonitorService : public QObject
{
	Q_OBJECT
public:
	AbstractMonitorService(const QString &serviceName, QObject *parent = 0);

	virtual ~AbstractMonitorService();

	void addBattery(const BatteryController *c);

	void registerService();

	virtual void updateValues() = 0;

protected:
	VBusItem *produce(const QString &path, const QVariant &value,
					  const QString &unit = QString(), int precision = 0);

	const QList<const BatteryController *> &controllers() const;

private slots:
	void onTimer();

	void onDestroyed();

private:
	VBusNode *mRoot;
	QList<const BatteryController *> mControllers;
	QDBusConnection mConnection;
	QString mServiceName;
	bool mServiceRegistered;
};

#endif // MONITORSERVICE_H
