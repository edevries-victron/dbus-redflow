#ifndef VEBUSVIRTUALOBJECT_H
#define VEBUSVIRTUALOBJECT_H

#include <QDBusConnection>
#include <QDBusVirtualObject>

class VeQItem;

class VeServiceDbus : public QDBusVirtualObject
{
	Q_OBJECT
public:
	VeServiceDbus(const QDBusConnection &connection, VeQItem *root, QObject *parent = 0);

	QString serviceName() const;

	VeQItem *root();

	bool registerService();

	bool unregisterService();

	virtual QString introspect(const QString &path) const;

	virtual bool handleMessage(const QDBusMessage &message,
							   const QDBusConnection &connection);

private slots:
	void onChildAdded(VeQItem *item);

	void onChildRemoved(VeQItem *item);

	void onValueChanged(VeQItem *item, QVariant var);

private:
	bool handleGetValue(const QDBusMessage &message,
						const QDBusConnection &connection);

	bool handleGetter(const QDBusMessage &message,
					  const QDBusConnection &connection, const QVariant &value);

	bool handleSetValue(const QDBusMessage &message,
						const QDBusConnection &connection,
						VeQItem *producer);

	void buildTree(VeQItem *root, QVariantMap &map);

	void connectItem(VeQItem *item);

	void disconnectItem(VeQItem *item);

	VeQItem *findItem(const QString &path) const;

	QDBusConnection mConnection;
	VeQItem *mRoot;
};

#endif // VEBUSVIRTUALOBJECT_H
