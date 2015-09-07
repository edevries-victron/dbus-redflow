#include <QDBusMessage>
#include <QDBusVariant>
#include <QDebug>
#include <QSet>
#include <velib/qt/ve_qitem.hpp>
#include "dbus_tools.h"
#include "ve_service_dbus.h"

VeServiceDbus::VeServiceDbus(const QDBusConnection &connection, VeQItem *root,
							 QObject *parent):
	QDBusVirtualObject(parent),
	mConnection(connection),
	mRoot(root)
{
	Q_ASSERT(root != 0);
	connectItem(root);
}

VeQItem *VeServiceDbus::root()
{
	return mRoot;
}

QString VeServiceDbus::serviceName() const
{
	return mConnection.name();
}

bool VeServiceDbus::registerService()
{
	if (!mConnection.registerVirtualObject("/", this, QDBusConnection::SubPath)) {
		qDebug() << "Coould not register virtual object";
		return false;
	}
	if (!mConnection.registerService(serviceName())) {
		qDebug() << "Could not register service" << serviceName();
		return false;
	}
	return true;
}

bool VeServiceDbus::unregisterService()
{
	if (!mConnection.unregisterService(serviceName())) {
		qDebug() << "Could not unregister service" << serviceName();
		return false;
	}
	return true;
}

QString VeServiceDbus::introspect(const QString &path) const
{
	static const QString Interface =
		"  <interface name=\"com.victronenergy.BusItem\">\n"
		"    <method name=\"GetDescription\">\n"
		"      <arg direction=\"in\" type=\"s\" name=\"language\"/>\n"
		"      <arg direction=\"in\" type=\"i\" name=\"length\"/>\n"
		"      <arg direction=\"out\" type=\"s\" name=\"descr\"/>\n"
		"    </method>\n"
		"    <method name=\"GetValue\">\n"
		"      <arg direction=\"out\" type=\"v\" name=\"value\"/>\n"
		"    </method>\n"
		"    <method name=\"GetText\">\n"
		"      <arg direction=\"out\" type=\"s\" name=\"value\"/>\n"
		"    </method>\n"
		"    <method name=\"SetValue\">\n"
		"      <arg direction=\"in\" type=\"v\" name=\"value\"/>\n"
		"      <arg direction=\"out\" type=\"i\" name=\"retval\"/>\n"
		"    </method>\n"
		"    <method name=\"GetMin\">\n"
		"      <arg direction=\"out\" type=\"v\" name=\"value\"/>\n"
		"    </method>\n"
		"    <method name=\"GetMax\">\n"
		"      <arg direction=\"out\" type=\"v\" name=\"value\"/>\n"
		"    </method>\n"
		"    <property access=\"readwrite\" type=\"v\" name=\"Value\"/>\n"
		"    <property access=\"read\" type=\"s\" name=\"Text\"/>\n"
		"    <signal name=\"PropertiesChanged\">\n"
		"      <arg direction=\"out\" type=\"a{sv}\" name=\"changes\"/>\n"
		"      <annotation value=\"QVariantMap\" name=\"com.trolltech.QtDBus.QtTypeName.In0\"/>\n"
		"    </signal>\n"
		"  </interface>\n";
	QString result = Interface;
	VeQItem *item = findItem(path);
	if (item == 0)
		return result;
	for (int i=0;;++i) {
		VeQItem *child = item->itemChild(i);
		if (child == 0)
			break;
		result.append(QString("  <node name=\"%1\"/>\n").arg(child->id()));
	}
	return result;
}

bool VeServiceDbus::handleMessage(const QDBusMessage &message,
										const QDBusConnection &connection)
{
	switch (message.type()) {
	case QDBusMessage::MethodCallMessage:
	{
		QString member = message.member();
		VeQItem *item = findItem(message.path());
		if (item == 0) {
			if (member == "GetValue") {
				return handleGetValue(message, connection);
			}
		} else {
			if (member == "GetValue") {
				return handleGetter(message, connection, item->getValue());
			} else if (member == "GetText") {
				return handleGetter(message, connection, item->getText());
			}
		}
	}
		break;
	default:
		qDebug() << __FUNCTION__ << message.type();
		break;
	}
	return false;
}

bool VeServiceDbus::handleGetValue(const QDBusMessage &message,
										 const QDBusConnection &connection)
{
	QVariantMap map;
	buildTree(mRoot, map);
	QDBusMessage reply = message.createReply(QVariant::fromValue(map));
	return connection.send(reply);
}

bool VeServiceDbus::handleGetter(const QDBusMessage &message,
									   const QDBusConnection &connection,
									   const QVariant &value)
{
	QVariant v = value;
	denormalizeVariant(v);
	QDBusMessage reply = message.createReply(
		QVariant::fromValue(QDBusVariant(v)));
	return connection.send(reply);
}

bool VeServiceDbus::handleSetValue(const QDBusMessage &message,
								   const QDBusConnection &connection,
								   VeQItem *producer)
{
	QList<QVariant> args = message.arguments();
	if (args.size() != 1) {
		QDBusMessage reply = message.createErrorReply(QDBusError::InvalidArgs,
													  "Expected 1 argument");
		return connection.send(reply);
	}
	QVariant v = args.first();
	normalizeVariant(v);
	int r = producer->setValue(v);
	QDBusMessage reply = message.createReply(r);
	return connection.send(reply);
}

void VeServiceDbus::buildTree(VeQItem *root, QVariantMap &map)
{
	for (int i=0;;++i) {
		VeQItem *child = root->itemChild(i);
		if (child == 0)
			break;
		if (child->isLeaf()) {
			QString path = child->getRelId(mRoot);
			QVariant v = child->getValue();
			denormalizeVariant(v);
			map[path] = v;
		} else {
			buildTree(child, map);
		}
	}
}

void VeServiceDbus::connectItem(VeQItem *item)
{
	if (item->isLeaf()) {
		connect(item, SIGNAL(valueChanged(VeQItem *, QVariant)),
				this, SLOT(onValueChanged(VeQItem *, QVariant)));
	} else {
		connect(item, SIGNAL(childAdded(VeQItem *)),
				this, SLOT(onChildAdded(VeQItem *)));
		connect(item, SIGNAL(childRemoved(VeQItem *)),
				this, SLOT(onChildRemoved(VeQItem *)));
		for (int i=0;;++i) {
			VeQItem *child = item->itemChild(i);
			if (child == 0)
				break;
			connectItem(child);
		}
	}
}

void VeServiceDbus::disconnectItem(VeQItem *item)
{
	if (item->isLeaf()) {
		disconnect(item, SIGNAL(valueChanged(VeQItem *, QVariant)),
				   this, SLOT(onValueChanged(VeQItem *, QVariant)));
	} else {
		disconnect(item, SIGNAL(childAdded(VeQItem *)),
				   this, SLOT(onChildAdded(VeQItem *)));
		disconnect(item, SIGNAL(childRemoved(VeQItem *)),
				   this, SLOT(onChildRemoved(VeQItem *)));
		for (int i=0;;++i) {
			VeQItem *child = item->itemChild(i);
			if (child == 0)
				break;
			disconnectItem(child);
		}
	}
}

void VeServiceDbus::onChildAdded(VeQItem *item)
{
	connectItem(item);
}

void VeServiceDbus::onChildRemoved(VeQItem *item)
{
	disconnectItem(item);
}

void VeServiceDbus::onValueChanged(VeQItem *item, QVariant var)
{
	Q_UNUSED(var)
	QString dbusPath = item->getRelId(mRoot);
	QDBusMessage message = QDBusMessage::createSignal(dbusPath,
													  "com.victronenergy.BusItem",
													  "PropertiesChanged");
	QVariantMap map;
	QVariant v = item->getValue();
	denormalizeVariant(v);
	map.insert("Value", v);
	map.insert("Text", item->getText());
	message << map;
	if (!mConnection.send(message))
		qDebug() << "Could not send PropertiesChanged signal on path" << dbusPath;
}

VeQItem *VeServiceDbus::findItem(const QString &path) const
{
	if (path == "/")
		return 0;
	return mRoot->itemGet(path);
}
