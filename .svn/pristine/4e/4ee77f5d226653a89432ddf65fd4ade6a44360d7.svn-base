#include "vebuswnd.h"
#include "ui_vebuswnd.h"

#include <velib/platform/task.h>
#include <velib/qt/ve_qitem_c_reproducer.hpp>
#include <velib/types/ve_values.h>

static const char ilimit[] = "/Control/Ac/ActiveIn/CurrentLimit";

VebusWnd::VebusWnd(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::VebusWnd)
{
	ui->setupUi(this);
	this->startTimer(500);
}

VebusWnd::~VebusWnd()
{
	delete ui;
}

void VebusWnd::timerEvent(QTimerEvent *event)
{
	char buf[100];
	VE_UNUSED(event);

	foreach (VLabel *label, findChildren<VLabel *>()) {
		QString id = mBusPrefix + label->getBusId();
		veValueGetText(id.toLatin1(), buf, sizeof(buf));
		label->setText(buf);
	}
}

void VebusWnd::on_pushButton_clicked()
{
	qVeItemSetInt(mBusPrefix + ilimit, 0);
}
