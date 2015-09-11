#ifndef BATTERY_CONTROLLER_BRIDGE_H
#define BATTERY_CONTROLLER_BRIDGE_H

#include <QPointer>
#include <QString>
#include "dbus_bridge.h"
#include "dbus_redflow.h"

class BatteryController;
class BatteryControllerSettings;
class Settings;

/*!
 * @brief Connects data from `BatteryController` to the DBus.
 * This class creates and fills the com.victronenergy.xxx.yyy service.
 * Where xxx is grid, pvinverter or shore depending on the `BatteryControllerSettings`,
 * and yyy is the communication port (eg /dev/ttyUSB0).
 */
class BatteryControllerBridge : public DBusBridge
{
	Q_OBJECT
public:
	explicit BatteryControllerBridge(BatteryController *BatteryController,
							QObject *parent = 0);
public slots:
	void produceBatteryInfo(BatteryController *bc, const QString &path);

protected:
	virtual bool toDBus(const QString &path, QVariant &value);

	virtual bool fromDBus(const QString &path, QVariant &value);

private:
	BatteryController *mBatteryController;
};

#endif // BATTERY_CONTROLLER_BRIDGE_H
