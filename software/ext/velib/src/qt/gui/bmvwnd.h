#ifndef _VELIB_QT_GUI_BMVWND_H_
#define _VELIB_QT_GUI_BMVWND_H_

#include <QWidget>

namespace Ui {
class BmvWnd;
}

class BmvWnd : public QWidget
{
	Q_OBJECT

	void timerEvent(QTimerEvent *event);
	void setBusPrefix(QString prefix) {mBusPrefix = prefix; }

public:
	explicit BmvWnd(QWidget *parent = 0);
	~BmvWnd();

private:
	Ui::BmvWnd *ui;
	QString mBusPrefix;
};

#endif
