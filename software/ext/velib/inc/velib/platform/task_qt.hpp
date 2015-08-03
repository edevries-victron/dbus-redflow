#ifndef _VELIB_PLATFORM_TASK_QT_HPP_
#define _VELIB_PLATFORM_TASK_QT_HPP_

#include <qobject.h>
#include <qwidget.h>
#include <QMenu>
#include <QPoint>
#include <velib/platform/task.h>
#include <velib/utils/ve_todo.h>

class TaskQt : public QObject
{
	Q_OBJECT

public:
	static bool veTodoFlag;

	TaskQt();

	void start();
	void updateAll();
	static void getCanDrivers(QVector<QString> &vect);
	static void getCanDriversDescription(QVector<QString>& vect);
	static void initCanDrivers();
	virtual void timerEvent(QTimerEvent *event);
	void provideContextMenu(QWidget* widget);
	QMenu& getMenu() { return mMenu; }

	static char const *selectedSerialPort() { return TaskQt::mSerial; }

public slots:
	void showContextMenu(const QPoint& pos);
	void connectCan(bool checked);
	void connectVebus(bool checked);

private:
	void createMenu();
	void createCanMenu();
	void createVebusMenu();
	QWidget* mContextWidget;
	QMenu mMenu;
	static char const *mSerial;
};

#endif
