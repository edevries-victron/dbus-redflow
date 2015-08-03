#include "vled.h"
#include "ui_vled.h"
#include <qpainter.h>

#include <velib/qt/ve_qitem_c_reproducer.hpp>

VLed::VLed(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::VLed),
	mToggle(0)
{
	ui->setupUi(this);
	this->startTimer(500);
}

VLed::State VLed::getState()
{
	QString id = mBusPrefix + getBusId();
	return (VLed::State) qVeItemGetInt(id.toLatin1());
}

void VLed::paintEvent(QPaintEvent *)
{
	QColor color(mColor);

	switch (getState())
	{
	case VLed::LED_ON:
		break;

	case VLed::LED_OFF:
		color = color.darker(300);
		break;

	case VLed::LED_BLINK:
		if (!mToggle)
			color = color.darker(300);
		break;

	case VLed::LED_BLINK2:
		if (mToggle)
			color = color.darker(300);
		break;

	default:
		color = color.red();
		;
	}
	QLinearGradient gradient(0, 0, 0, 100);
	gradient.setColorAt(0.0, color);
	gradient.setColorAt(1.0, color);

	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(gradient);
	p.drawEllipse(this->rect());
}

void VLed::timerEvent(QTimerEvent *event)
{
	VE_UNUSED(event);

	mToggle = !mToggle;
	repaint();
}

VLed::~VLed()
{
	delete ui;
}
