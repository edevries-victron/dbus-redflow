#include "vebuspanel.h"
#include "ui_vebuspanel.h"
#include <vled.h>

#include <velib/mk2/vebus_system.h>
#include <velib/types/ve_values.h>
#include <velib/qt/ve_qitem_c_reproducer.hpp>

static const char ilimit[] = "/Ac/ActiveIn/CurrentLimit";
const char switchpos[] = "/Mode";
const char max_charge[] = "/Dc/MaxChargeCurrent";

VebusPanel::VebusPanel(QWidget *parent) :
	QFrame(parent),
	ui(new Ui::VebusPanel)
{
	ui->setupUi(this);
	this->startTimer(250);
}

VebusPanel::~VebusPanel()
{
	delete ui;
}

void VebusPanel::timerEvent(QTimerEvent *event)
{
	(void) event;

	ui->lcdNumber->display(qVeItemGetInt(mBusPrefix + ilimit));
	ui->lcdNumberChargeCurrent->display(qVeItemGetInt(mBusPrefix + max_charge));
}

void VebusPanel::setBusPrefix(QString prefix)
{
	 mBusPrefix = prefix;

	 foreach (VLed *led, findChildren<VLed *>())
		led->setBusPrefix(prefix);
}

void VebusPanel::on_pushButtonOff_released()
{
	qVeItemSetInt(mBusPrefix + switchpos, SP_SWITCH_OFF);
}

void VebusPanel::on_pushButtonOn_released()
{
	qVeItemSetInt(mBusPrefix + switchpos, SP_SWITCH_ON);
}

void VebusPanel::on_pushButtonChargerOnly_released()
{
	qVeItemSetInt(mBusPrefix + switchpos, SP_CHARGER_ONLY);
}

void VebusPanel::on_pushButtonInverterOnly_pressed()
{
	qVeItemSetInt(mBusPrefix + switchpos, SP_INVERTER_ONLY);
}

void VebusPanel::on_pushButtonAcLower_pressed()
{
	qVeItemSetInt(mBusPrefix + ilimit, qVeItemGetInt(mBusPrefix + ilimit) - 1);
}

void VebusPanel::on_pushButtonAcHigher_pressed()
{
	int value = qVeItemGetInt(mBusPrefix + ilimit);

	if (value)
		qVeItemSetInt(mBusPrefix + ilimit, qVeItemGetInt(mBusPrefix + ilimit) + 1);
	else
		qVeItemSetInt(mBusPrefix + ilimit, 16);
}

void VebusPanel::on_pushButtonMaxCurrentLower_pressed()
{
	qVeItemSetInt(mBusPrefix + max_charge, qVeItemGetInt(mBusPrefix + max_charge) - 6);
}

void VebusPanel::on_pushButtonMaxCurrentHigher_pressed()
{
	qVeItemSetInt(mBusPrefix + max_charge, qVeItemGetInt(mBusPrefix + max_charge) + 6);
}
