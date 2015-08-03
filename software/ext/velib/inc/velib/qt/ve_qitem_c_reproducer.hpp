#ifndef _VELIB_QT_VE_QITEM_C_REPRODUCER_HPP_
#define _VELIB_QT_VE_QITEM_C_REPRODUCER_HPP_

#include <QAbstractItemModel>
#include <QMap>
#include <QObject>
#include <QVariant>

#include <velib/types/ve_item.h>
#include <velib/types/ve_item_def.h>

VE_DCL void itemValueChanged(struct VeItem *item);
VE_DCL void itemChildAdded(struct VeItem *childItem);
VE_DCL void foreachStub(struct VeItem *, void *ctx);

class VeQItemCReproducer : public QObject //QAbstractItemModel
{
	Q_OBJECT

	typedef void ForeachCallback(VeItem *item, VeItem *child, void *eachCtx);

public:
	VeQItemCReproducer(QObject *parent = 0) : QObject(parent),
	  mItem(0)
		{}
	VeQItemCReproducer(VeItem *item, QObject *parent = 0) : QObject(parent),
	  mItem(item)
		{}

	bool consume(QString service, QString path);

	QVariant getValue();
	int setValue(QVariant const& value);
	QString getText();
	QVariant getMin();
	QVariant getMax();

	QList<QString> getChildIds();

	static void addChildId(struct VeItem *item, struct VeItem *child, void *container);

protected:
	void handleValueChanged() { emit valueChanged(this); }
	void handleMinChanged() { emit minChanged(this); }
	void handleMaxChanged() {  emit maxChanged(this); }
	void handleTextChanged() { emit textChanged(this); }
	void handleChildAdded(struct VeItem *child) { emit childAdded(this, QString(veItemId(child))); }

signals:
	void valueChanged(VeQItemCReproducer *veValue);
	void minChanged(VeQItemCReproducer *veValue);
	void maxChanged(VeQItemCReproducer *veValue);
	void textChanged(VeQItemCReproducer *veValue);

	void childAdded(VeQItemCReproducer *veValue, QString id);

private:
	void foreachChild(ForeachCallback *fun, void *eachCtx);

	struct VeItem *mItem;
	ForeachCallback *mForeachCallback;
	void *mEachCtx;

friend void itemValueChanged(struct VeItem *item);
friend void itemChildAdded(struct VeItem *childItem);
friend void foreachStub(struct VeItem *, void *ctx);

};

int qVeItemGetInt(QString uid);
int qVeItemSetInt(QString uid, int value);

#endif
