#ifndef SENSORREADING_H
#define SENSORREADING_H

#include <QDateTime>

class QString;

class SensorReading
{
public:
	SensorReading();

	bool fromCsv(const QString &csv);
	double x() const;
	double y() const;
	double z() const;
	double battery() const;
	double temperature() const;
	double conductance() const;
	QDateTime time() const;
	QString lastError() const;

private:
	double m_x;
	double m_y;
	double m_z;
	double m_battery;
	double m_temperature;
	double m_conductance;
	QDateTime m_time;
	QString m_error;
};

QDebug operator<<(QDebug debug, const SensorReading &reading);

#endif // SENSORREADING_H
