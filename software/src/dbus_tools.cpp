#include <QDBusArgument>
#include <QDBusMetaType>
#include <QDBusVariant>
#include <QDebug>
#include "dbus_tools.h"

Q_DECLARE_METATYPE(QList<int>)

QString formatValue(double value, const QString &unit,
						   int precision) {
	QString s;
	if (precision == -1)
		s.setNum(value, 'f');
	else
		s.setNum(value, 'f', precision);
	if (!s.isEmpty())
		s += unit;
	return s;
}

QString formatValue(const QVariant &value, const QString &unit,
						   int precision) {
	if (!value.isValid()) {
		return "--";
	}
	if (value.type() == QVariant::Double) {
		return formatValue(value.toDouble(), unit, precision);
	} else {
		return value.toString() + unit;
	}
}

static void normalizeMap(QVariantMap &map)
{
	for (QVariantMap::iterator it = map.begin(); it != map.end(); ++it) {
		normalizeVariant(it.value());
	}
}

static void normalizeList(QVariantList &list)
{
	for (QVariantList::iterator it = list.begin(); it != list.end(); ++it) {
		normalizeVariant(*it);
	}
}

void normalizeVariant(QVariant &v, QString *signature)
{
	if (v.userType() == QVariant::Map) {
		QVariantMap m = v.toMap();
		normalizeMap(m);
	} else if (v.userType() == qMetaTypeId<QDBusVariant>()) {
		v = qvariant_cast<QDBusVariant>(v).variant();
		normalizeVariant(v, signature);
	} else if (v.userType() == qMetaTypeId<QDBusArgument>()) {
		QDBusArgument arg = qvariant_cast<QDBusArgument>(v);
		if (signature != 0)
			*signature = arg.currentSignature();
		if (arg.currentSignature() == "a{sv}") {
			QVariantMap m = qdbus_cast<QVariantMap>(arg);
			normalizeMap(m);
			v = m;
		} else  if (arg.currentSignature() == "av") {
			QVariantList vl = qdbus_cast<QVariantList>(arg);
			normalizeList(vl);
			v = vl;
		} else  if (arg.currentSignature() == "ai") {
			// An empty list of integers is used by victron to encode an invalid
			// value, because D-Bus itself does not define an invalid (or null)
			// value.
			QList<int> vl = qdbus_cast<QList<int> >(arg);
			if (vl.size() == 0)
				v = QVariant();
		} else {
			qDebug() <<	"Cannot handle signature:" << arg.currentSignature();
		}
	}
	if (signature == 0)
		return;
	/// @todo EV Code like this must be present somewhere in QT.
	if (!v.isValid()) {
		*signature = "";
		return;
	}
	switch (v.type()) {
	case QVariant::Bool:
		*signature = "b";
		break;
	case QVariant::Int:
		*signature = "i";
		break;
	case QVariant::Double:
		*signature = "f";
		break;
	case QVariant::String:
		*signature = "s";
		break;
	default:
		break;
	}
}

void denormalizeVariant(QVariant &v)
{
	if (!v.isValid())
		v = QVariant::fromValue(QList<int>());
}
