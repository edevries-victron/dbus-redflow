#include <QDBusMessage>
#include <QDBusVariant>
#include <velib/qt/ve_qitem.hpp>
#include <velib/qt/v_busitems.h>
#include "ve_item_provider_dbus.h"
#include "ve_service_dbus.h"

static const QString DefaultName = "consumer";

VeItemProviderDbus::VeItemProviderDbus(VeQItem *root, QObject *parent):
	QObject(parent),
	mRoot(root)
{
	addServices();
	connect(mRoot, SIGNAL(childAdded(VeQItem *)),
			this, SLOT(onChildAdded(VeQItem *)));
	connect(mRoot, SIGNAL(childRemoved(VeQItem *)),
			this, SLOT(onChildRemoved(VeQItem *)));
}

void VeItemProviderDbus::onChildAdded(VeQItem *item)
{
	VeServiceDbus *service = new VeServiceDbus(VBusItems::getConnection(item->id()), item, this);
	mServices.append(service);
	service->registerService();
}

void VeItemProviderDbus::onChildRemoved(VeQItem *item)
{
	foreach (VeServiceDbus *service, mServices) {
		if (service->root() == item) {
			mServices.removeOne(service);
			service->unregisterService();
			delete service;
			break;
		}
	}
}

void VeItemProviderDbus::addServices()
{
	for (int i=0; ; ++i) {
		VeQItem *item = mRoot->itemChild(i);
		if (item == 0)
			break;
		onChildAdded(item);
	}
}
