#ifndef _VELIB_QT_QUI_VLABEL_H_
#define _VELIB_QT_QUI_VLABEL_H_

#include <QLabel>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class VLabel : public QLabel
{
	Q_OBJECT
	Q_PROPERTY(QString busid READ getBusId WRITE setBusId DESIGNABLE true)

public:
	explicit VLabel(QWidget *parent = 0);

	QString getBusId() { return mBusId; }
	void setBusId(QString busId) { mBusId = busId; }
signals:

public slots:

private:
	QString mBusId;
};

#endif
