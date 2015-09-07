#ifndef VEDBUSITEMPROVIDER_H
#define VEDBUSITEMPROVIDER_H

#include <QDBusConnection>

class VeQItem;
class VeServiceDbus;

class VeItemProviderDbus : public QObject
{
	Q_OBJECT
public:
	VeItemProviderDbus(VeQItem *root,
					   QObject *parent = 0);

private slots:
	void onChildAdded(VeQItem *item);

	void onChildRemoved(VeQItem *item);

private:
	void addServices();

	QList<VeServiceDbus *> mServices;
	VeQItem *mRoot;
};

#endif // VEDBUSITEMPROVIDER_H
