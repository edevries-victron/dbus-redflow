#ifndef _VELIB_QT_QUI_VLED_H_
#define _VELIB_QT_QUI_VLED_H_

#include <QWidget>

namespace Ui {
class VLed;
}

class VLed : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QColor color READ getColor WRITE setColor)
	Q_PROPERTY(State state READ getState)
	Q_PROPERTY(QString busid READ getBusId WRITE setBusId)

	enum State {
		LED_OFF,
		LED_ON,
		LED_BLINK,
		LED_BLINK2
	};

public:
	explicit VLed(QWidget *parent = 0);
	~VLed();

	void paintEvent(QPaintEvent *);
	QColor getColor() { return mColor; }
	void setColor(QColor color) { mColor = color; repaint(); }
	State getState();
	QString getBusId() { return mBusId; }
	void setBusId(QString busId) { mBusId = busId; repaint();}
	void setBusPrefix(QString prefix) { mBusPrefix = prefix; }
	void timerEvent(QTimerEvent *event);

private:
	Ui::VLed *ui;
	QColor mColor;
	State mState;
	int mToggle;
	QString mBusId;
	QString mBusPrefix;
};

#endif
