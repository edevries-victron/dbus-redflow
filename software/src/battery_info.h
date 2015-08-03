#ifndef BATTERY_INFO_H
#define BATTERY_INFO_H

#include <QObject>
#include <QTime>

/*!
 * Contains measurement data from an ZBM Battery.
 */
class BatteryInfo : public QObject
{
	Q_OBJECT
	Q_PROPERTY(double current READ current WRITE setCurrent NOTIFY currentChanged)
	Q_PROPERTY(double voltage READ voltage WRITE setVoltage NOTIFY voltageChanged)
	Q_PROPERTY(double power READ power WRITE setPower NOTIFY powerChanged)
	Q_PROPERTY(double energyForward READ energyForward WRITE setEnergyForward NOTIFY energyForwardChanged)
	Q_PROPERTY(double energyReverse READ energyReverse WRITE setEnergyReverse NOTIFY energyReverseChanged)
public:
	explicit BatteryInfo(QObject *parent = 0);

	int BattVolts() const;

	void setBattVolts(int v);

	int BattAmps() const;

	void setBattAmps(int v);

	int SOC() const;

	void setSOC(int v);

	int BattTemp() const;

	void setBattTemp(int v);

	int BussVolts() const;

	void setBussVolts(int v);

	int BussAmps() const;

	void setBussAmps(int v);

	/*!
	 * @brief Reset all measured values to NaN
	 */
	void resetValues();

signals:
	void BattVoltsChanged();

	void BattAmpsChanged();

	void SOCChanged();

	void BussVoltsChanged();

	void BussAmpsChanged();

	void BattTempChanged();

	void AirTempChanged();

private:
	static bool valuesEqual(double v1, double v2);

	int mBattVolts;
	int mBattAmps;
	int mSOC;
	int mBussVolts;
	int mBussAmps;
	int mBattTemp;
	int mAirTemp;
};

#endif // BATTERY_INFO_H
