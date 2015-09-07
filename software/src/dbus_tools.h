#ifndef DBUS_TOOLS_HPP
#define DBUS_TOOLS_HPP

#include <QString>
#include <QVariant>

QString formatValue(double value, const QString &unit, int precision);

QString formatValue(const QVariant &value, const QString &unit, int precision);

/*!
 * qtDbus creates a variant of QDBusArgument for all none basic types
 * encountered in case of a QDBusVariant. These can be be casted/demarshalled to
 * a explicit type. However qml does will not perform such casts and complains
 * it can't deal with a QDBusArgument. In order to make this work, the casts are
 * performed here. This is in no way complete. QVariantMap and arrays of basic
 * types are supported.
 *
 * NOTE: The remote c side has no onChange support at the moment (june 2014)
 */
void normalizeVariant(QVariant &v, QString *signature = 0);

void denormalizeVariant(QVariant &v);

#endif // DBUS_TOOLS_HPP

