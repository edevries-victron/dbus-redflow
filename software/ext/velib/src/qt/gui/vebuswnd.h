#ifndef _VELIB_QT_QUI_VEBUSWND_H_
#define _VELIB_QT_QUI_VEBUSWND_H_

#include <QWidget>

extern "C" int cpp_main(int argc, char *argv[]);

namespace Ui {
class VebusWnd;
}

class VebusWnd : public QWidget
{
	Q_OBJECT

public:
	explicit VebusWnd(QWidget *parent = 0);
	~VebusWnd();

	void timerEvent(QTimerEvent *event);
	void setBusPrefix(QString prefix) { mBusPrefix = prefix; }

private slots:
	void on_pushButton_clicked();

private:
	Ui::VebusWnd *ui;
	QString mBusPrefix;
};

#endif
