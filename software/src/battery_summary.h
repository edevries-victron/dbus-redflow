#ifndef BATTERYSUMMARY_H
#define BATTERYSUMMARY_H

#include <QList>
#include <QObject>

class BatteryController;

/// A statistical roundup of all connected Redflow batteries.
class BatterySummary : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int zbmCount READ zbmCount WRITE setZbmCount NOTIFY zbmCountChanged)
	Q_PROPERTY(QList<int> deviceAddresses READ deviceAddresses NOTIFY deviceAddressesChanged)
	Q_PROPERTY(double averageVoltage READ averageVoltage WRITE setAverageVoltage NOTIFY averageVoltageChanged)
	Q_PROPERTY(double totalCurrent READ totalCurrent WRITE setTotalCurrent NOTIFY totalCurrentChanged)
	Q_PROPERTY(double totalPower READ totalPower WRITE setTotalPower NOTIFY totalPowerChanged)
	Q_PROPERTY(double averageStateOfCharge READ averageStateOfCharge WRITE setAverageStateOfCharge NOTIFY averageStateOfChargeChanged)
	Q_PROPERTY(int operationalMode READ operationalMode WRITE setOperationalMode NOTIFY operationalModeChanged)
	Q_PROPERTY(int requestClearStatusRegister READ requestClearStatusRegister WRITE setRequestClearStatusRegister NOTIFY requestClearStatusRegisterChanged)
	Q_PROPERTY(int requestDelayedSelfMaintenance READ requestDelayedSelfMaintenance WRITE setRequestDelayedSelfMaintenance NOTIFY requestDelayedSelfMaintenanceChanged)
	Q_PROPERTY(int requestImmediateSelfMaintenance READ requestImmediateSelfMaintenance WRITE setRequestImmediateSelfMaintenance NOTIFY requestImmediateSelfMaintenanceChanged)
	Q_PROPERTY(int maintenanceActive READ maintenanceActive WRITE setMaintenanceActive NOTIFY maintenanceActiveChanged)
	Q_PROPERTY(int maintenanceNeeded READ maintenanceNeeded WRITE setMaintenanceNeeded NOTIFY maintenanceNeededChanged)
public:
	BatterySummary(QObject *parent = 0);

	QList<int> deviceAddresses() const;

	void addBattery(BatteryController *c);

	int zbmCount() const;

	double averageVoltage() const;

	double totalCurrent() const;

	double totalPower() const;

	double averageStateOfCharge() const;

	int operationalMode() const;

	void setOperationalMode(int v);

	int requestClearStatusRegister() const;

	void setRequestClearStatusRegister(int v);

	int requestDelayedSelfMaintenance() const;

	void setRequestDelayedSelfMaintenance(int v);

	int requestImmediateSelfMaintenance() const;

	void setRequestImmediateSelfMaintenance(int v);

	int maintenanceActive() const;

	int maintenanceNeeded() const;

signals:
	void zbmCountChanged();

	void deviceAddressesChanged();

	void averageVoltageChanged();

	void totalCurrentChanged();

	void totalPowerChanged();

	void averageStateOfChargeChanged();

	void operationalModeChanged();

	void requestClearStatusRegisterChanged();

	void requestDelayedSelfMaintenanceChanged();

	void requestImmediateSelfMaintenanceChanged();

	void maintenanceActiveChanged();

	void maintenanceNeededChanged();

private slots:
	void onTimeout();

	void onControllerDestroyed();

private:
	void setZbmCount(int v);

	void setAverageVoltage(double v);

	void setTotalCurrent(double v);

	void setTotalPower(double v);

	void setAverageStateOfCharge(double v);

	void setMaintenanceActive(int v);

	void setMaintenanceNeeded(int v);

	void updateValues();

	int mZbmCount;
	double mAverageVoltage;
	double mTotalCurrent;
	double mTotalPower;
	double mAverageStateOfCharge;
	int mOperationalMode;
	int mRequestClearStatusRegister;
	int mRequestDelayedSelfMaintenance;
	int mRequestImmediateSelfMaintenance;
	int mMaintenanceActive;
	int mMaintenanceNeeded;
	QList<BatteryController *> mControllers;
};

#endif // BATTERYSUMMARY_H
