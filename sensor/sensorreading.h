#ifndef SENSORREADING_H
#define SENSORREADING_H

#include "sensor.h"

class QString;

class SensorReading
{
public:
	SensorReading();

	bool fromCsv(const QString &csv);
	int number() const;
	Sensor::SamplingRate samplingRate() const;
	double x() const;
	double y() const;
	double z() const;
	double battery() const;
	double temperature() const;
	double conductance() const;

private:
	int m_number;
	Sensor::SamplingRate m_samplingRate;
	double m_x;
	double m_y;
	double m_z;
	double m_battery;
	double m_temperature;
	double m_conductance;
};

QDebug operator<<(QDebug debug, const SensorReading &reading);

#endif // SENSORREADING_H
