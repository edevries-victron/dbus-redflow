#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QStringList>
#include "defines.h"

/*!
 * Contains the global (battery independent) settings.
 */
class Settings : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QStringList deviceIds READ deviceIds WRITE setDeviceIds NOTIFY deviceIdsChanged)
public:
	explicit Settings(QObject *parent = 0);

	const QStringList &deviceIds() const;

	void setDeviceIds(const QStringList &deviceIds);

	void registerDevice(const QString &serial);

signals:
	void deviceIdsChanged();

private:
	QStringList mDeviceIds;

};

#endif // SETTINGS_H
