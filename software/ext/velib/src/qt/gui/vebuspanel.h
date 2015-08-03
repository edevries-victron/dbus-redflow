#ifndef _VELIB_QT_QUI_VEBUSPANEL_H_
#define _VELIB_QT_QUI_VEBUSPANEL_H_

#include <QFrame>
#include <QTimer>

namespace Ui {
class VebusPanel;
}

class VebusPanel : public QFrame
{
	Q_OBJECT

public:
	explicit VebusPanel(QWidget *parent = 0);
	~VebusPanel();

	void timerEvent(QTimerEvent *event);
	void setBusPrefix(QString prefix);

private slots:
	void on_pushButtonOff_released();

	void on_pushButtonOn_released();

	void on_pushButtonChargerOnly_released();

	void on_pushButtonInverterOnly_pressed();

	void on_pushButtonAcHigher_pressed();

	void on_pushButtonAcLower_pressed();

	void on_pushButtonMaxCurrentLower_pressed();

	void on_pushButtonMaxCurrentHigher_pressed();

private:
	Ui::VebusPanel *ui;
	QString mBusPrefix;
};

#endif
