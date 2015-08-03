#include <qobject.h>
#include <qvector.h>
#include <qstring.h>
#include <QPoint>
#include <QDebug>

#include <velib/canhw/canhw_driver.h>
#include <velib/mk2/hal.h>
#include <velib/platform/plt.h>
#include <velib/platform/task_qt.hpp>
#include <velib/utils/ve_todo.h>
#include <velib/utils/ve_logger.h>

/*
 * TaskQt provides a simple QT wrapper to be compatible with common
 * c example programs. If the c program in questions uses the
 * veItem (see velib/src/types/ve_item.c) then the gui can access
 * values / receive change notification from it with its qt
 * equivalant in velib/src/types/ve_item_qt.cpp.
 */

bool TaskQt::veTodoFlag = veFalse;
char const *TaskQt::mSerial;

TaskQt::TaskQt() {
#if CFG_INIT_CTX
	/*
	 * note: this will "disable" interrupts during init, to behave more
	 * or less how sillicon will get out of reset. Which are stubs on
	 * bare targets and mutexes on an threaded OS. It is left up to the
	 * application when to "enable" interrupts with pltInterruptEnable();
	 * but typically that is add the end of taskInit.
	 */
	pltInitCtx();
#endif
	veLogLevel(3);
}

void TaskQt::start()
{
	taskInit();
	this->startTimer(50);
}

void TaskQt::updateAll() {
	do {
		TaskQt::veTodoFlag = veFalse;
		taskUpdate();
	} while(veTodoFlag);
}

void TaskQt::timerEvent(QTimerEvent *event)
{
	VE_UNUSED(event);
	taskTick();
	updateAll();
}

#if CFG_INIT_CANBUS
void TaskQt::initCanDrivers()
{
	// start CAN bus
	veCanInit();
	veCanDrvDynamicInit();
	veCanDrvEnumerate();
}

static void addGw(struct VeCanGatewayS *gw, void *ctx)
{
	QVector<QString> *vect = (QVector<QString>*) ctx;
	char buf[100];

	if (veCanGwId(gw, buf, sizeof(buf)) >= sizeof(buf))
		return;

	vect->append(buf);
}

/* Print a list of found gateways to stdout */
void TaskQt::getCanDrivers(QVector<QString>& vect)
{
	veCanDrvForeachGw(addGw, &vect);
}

static void addGwDescr(struct VeCanGatewayS *gw, void *ctx)
{
	QVector<QString> *vect = (QVector<QString>*) ctx;
	char buf[100];

	if (veCanGwDesc(gw, buf, sizeof(buf)) >= sizeof(buf))
		return;

	vect->append(buf);
}

/* Print a list of found gateways to stdout */
void TaskQt::getCanDriversDescription(QVector<QString>& vect)
{
	veCanDrvForeachGw(addGwDescr, &vect);
}

void TaskQt::connectCan(bool checked)
{
	QAction *act = (QAction*) sender();

	if (checked) {
		QString str = act->data().toString();
		qDebug() << str;
		VeCanGateway *gw = veCanGwGet(str.toAscii());
		if (gw) {
			if (!veCanOpen())
			{
				//logE("CAN", "canOpen failed");
				return;
			}

			if (veCanSetBitRate(250) == veFalse)
			{
				//logE("CAN", "set bitrate failed");
				return;
			}

			if (!veCanBusOn())
			{
				//logE("CAN", "can bus on failed");
				return;
			}
		}
	}
}

void TaskQt::createCanMenu()
{
	QVector<QString> ids;
	QVector<QString> descr;
	getCanDrivers(ids);
	getCanDriversDescription(descr);

	QMenu *menuConnect = mMenu.addMenu("Connect to CAN");

	int n = 0;
	foreach (QString driver, ids) {
		QAction *act = menuConnect->addAction(ids[n] + " " + descr[n]);
		act->setData(driver);
		act->setCheckable(true);
		connect(act, SIGNAL(triggered(bool)), SLOT(connectCan(bool)));
		n++;
	}
}

#else

void TaskQt::initCanDrivers()
{
}

void TaskQt::connectCan(bool checked)
{
	Q_UNUSED(checked);
}

void TaskQt::createCanMenu()
{
}

#endif

#if CFG_INIT_SERIAL_MK2
void TaskQt::createVebusMenu()
{
	QMenu *menuConnect = mMenu.addMenu("Connect to serial");

	QString port("/dev/ttyUSBO");
	QAction *act = menuConnect->addAction(port);
	act->setData(port);
	act->setCheckable(true);
	connect(act, SIGNAL(triggered(bool)), SLOT(connectVebus(bool)));
}

void TaskQt::connectVebus(bool checked)
{
	Q_UNUSED(checked);

	mSerial = "/dev/ttyUSB0";
	if (!mk2Open(mSerial)) {
		//logE("init", "cannot open '%s'", serialDevice);
		//pltExit(1);
	}
	start();
}

#else

void TaskQt::createVebusMenu()
{
}

void TaskQt::connectVebus(bool checked)
{
	Q_UNUSED(checked);
}

#endif

void TaskQt::provideContextMenu(QWidget* widget)
{
	mContextWidget = widget;
	createMenu();
	connect((QObject*) widget, SIGNAL(customContextMenuRequested(const QPoint&)),
			SLOT(showContextMenu(const QPoint&)));
	widget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void TaskQt::showContextMenu(const QPoint& pos)
{
	mMenu.exec(mContextWidget->mapToGlobal(pos));
}

void TaskQt::createMenu()
{
	createVebusMenu();
	createCanMenu();
}

void veTodo(void)
{
	TaskQt::veTodoFlag = true;
}

char const *pltGetSerialDevice(void)
{
	return TaskQt::selectedSerialPort();
}
