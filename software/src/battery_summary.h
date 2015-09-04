#ifndef BATTERYSUMMARY_H
#define BATTERYSUMMARY_H

#include <QDBusConnection>
#include <QObject>
#include <QList>

class BatteryController;
class VBusItem;

class BatterySummary : public QObject
{
	Q_OBJECT
public:
	BatterySummary(QObject *parent = 0);

	virtual ~BatterySummary();

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
	VBusItem *mVoltage;
	VBusItem *mCurrent;
	VBusItem *mPower;
	VBusItem *mTemperature;
	VBusItem *mConsumedAmphours;
	// VBusItem *mAlarms;
	VBusItem *mSoc;
};

#endif // BATTERYSUMMARY_H
