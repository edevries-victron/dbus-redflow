#include <velib/types/ve_values.h>
#include <velib/qt/ve_qitem_c_reproducer.hpp>

/**
 * Qt class which wraps around a VeItem and translated the c callbacks to
 * qt signals. This class is far from complete and only used for debugging.
 */

/* C stubs to make cpp calls again */
VE_DCL void itemValueChanged(struct VeItem *item)
{
	VeQItemCReproducer *qItem = reinterpret_cast<VeQItemCReproducer *>(veItemCtx(item)->ptr);
	qItem->handleValueChanged();
}

VE_DCL void itemChildAdded(struct VeItem *childItem)
{
	VeItem *parent = veItemParent(childItem);
	VeQItemCReproducer *qItem = reinterpret_cast<VeQItemCReproducer *>(veItemCtx(parent)->ptr);
	qItem->handleChildAdded(childItem);
}

VE_DCL void foreachStub(struct VeItem *child, void *ctx)
{
	VeQItemCReproducer *qItem = reinterpret_cast<VeQItemCReproducer *>(ctx);
	qItem->mForeachCallback(qItem->mItem, child, qItem->mEachCtx);
}

/* Just a wrapper class to make the VeItem available in qt */
bool VeQItemCReproducer::consume(QString service, QString path)
{
	VeItem *item;
	QString url = service + path;

	item = veValueTree();
	if (!item)
		goto not_found;

	item = veItemByUid(item, url.toLatin1());
	if (!item)
		goto not_found;
	mItem = item;

	veItemCtx(mItem)->ptr = this; // FIXME
	veItemSetChanged(mItem, itemValueChanged);
	veItemSetChildAdded(mItem, itemChildAdded);

	return veTrue;

not_found:
	// FIXME: wait for it to be found
	return veFalse;
}

QVariant VeQItemCReproducer::getValue()
{
	return QVariant();
}

/* Call fun on each child / grandchlidren with eachCtx as parameter */
void VeQItemCReproducer::foreachChild(ForeachCallback *fun, void *eachCtx)
{
	mForeachCallback = fun;
	mEachCtx = eachCtx;
	veItemForeach(mItem, foreachStub, this);
}

/* Private: used to find all leafs, container is the QList from getChildIds */
void VeQItemCReproducer::addChildId(struct VeItem *item, struct VeItem *child, void *container)
{
	char buf[VE_MAX_UID_SIZE];

	/* Ignore the parent only having child nodes */
	if (veItemFirstChild(child) != NULL)
		return;

	QList<QString> *list = reinterpret_cast<QList<QString> *>(container);

	/* Fixme in the c Item to return relative path directly */
	veItemUid(item, buf, sizeof(buf));
	QString parentName = QString(buf);

	veItemUid(child, buf, sizeof(buf));
	list->append(QString(buf).mid(parentName.length() + 1));
}

/* Return all ids of objects not having childs themselves */
QList<QString> VeQItemCReproducer::getChildIds()
{
	QList<QString> ret;

	// add all child ids to the container (if any)
	if (mItem != 0)
		foreachChild(addChildId, &ret);

	return ret;
}

QString VeQItemCReproducer::getText()
{
	char buf[100];

	if (mItem == NULL)
		return "";

	size_t n = veItemValueFmtString(mItem, buf, sizeof(buf));
	if (n >= sizeof(buf))
		return "";

	return QString(buf);
}

/* Replace these with proper cpp functions */
int qVeItemGetInt(QString uid)
{
	QByteArray arr;
	arr = uid.toLatin1();
	return veValueGetInt(arr.data());
}

int qVeItemSetInt(QString uid, int value)
{
	QByteArray arr;
	arr = uid.toLatin1();
	return veValueSetInt(arr.data(), value);
}
