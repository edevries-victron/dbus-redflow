#ifndef BATTERYSUMMARYBRIDGE_H
#define BATTERYSUMMARYBRIDGE_H

#include "dbus_bridge.h"

class BatterySummary;

/// Publishes the properties from the BatterySummary class on the D-Bus.
class BatterySummaryBridge : public DBusBridge
{
	Q_OBJECT
public:
	BatterySummaryBridge(BatterySummary *summary, QObject *parent = 0);

protected:
	virtual bool toDBus(const QString &path, QVariant &v);
};

#endif // BATTERYSUMMARYBRIDGE_H
