#ifndef BMSSERVICE_H
#define BMSSERVICE_H

#include <QDBusConnection>
#include <QObject>
#include <QList>

class BatteryController;
class VBusItem;

class BmsService : public QObject
{
	Q_OBJECT
public:
	BmsService(QObject *parent = 0);

	virtual ~BmsService();

	void addBattery(const BatteryController *c);

private slots:
	void onTimer();

	void onDestroyed();

private:
	void updateValues();

	VBusItem *produce(const QString &path, const QVariant &value,
					  const QString &unit = QString(), int precision = 0);

	QList<const BatteryController *> mControllers;
	QDBusConnection mConnection;
	VBusItem *mAllowedToCharge;
	VBusItem *mAllowedToDischarge;
};

#endif // BMSSERVICE_H
