#include "bmvwnd.h"
#include "ui_bmvwnd.h"
#include "vlabel.h"

#include <velib/types/ve_values.h>

BmvWnd::BmvWnd(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::BmvWnd),
	mBusPrefix("com.victronenergy.bmv/")
{
	ui->setupUi(this);
	this->startTimer(500);
}

void BmvWnd::timerEvent(QTimerEvent *event)
{
	char buf[100];
	VE_UNUSED(event);

	foreach (VLabel *label, findChildren<VLabel *>()) {
		QString id = mBusPrefix + label->getBusId();
		veValueGetText(id.toLatin1(), buf, sizeof(buf));
		label->setText(buf);
	}
}

BmvWnd::~BmvWnd()
{
	delete ui;
}
