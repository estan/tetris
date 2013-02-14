#include <QString>
#include <QStringList>
#include <QDebug>

#include "sensorreading.h"

SensorReading::SensorReading() :
	m_number(0),
	m_samplingRate(Sensor::UnknownRate),
	m_x(0.0),
	m_y(0.0),
	m_z(0.0),
	m_battery(0.0),
	m_temperature(0.0),
	m_conductance(0.0)
{
}

bool SensorReading::fromCsv(const QString &csv)
{
	QStringList tokens = csv.split(",");
	if (tokens.length() != 7) {
		qWarning(QString("Got %1 tokens, expected 7").arg(tokens.length()).toLatin1());
		return false;
	}

	m_samplingRate = Sensor::UnknownRate;
	bool ok = false;
	m_number = tokens[0].toInt(&ok);
	if (!ok) {
		if (tokens[0] == "A") {
			m_samplingRate = Sensor::TwoHertz;
		} else if (tokens[0] == "B") {
			m_samplingRate = Sensor::FourHertz;
		} else if (tokens[0] == "C") {
			m_samplingRate = Sensor::EightHertz;
		} else if (tokens[0] == "D") {
			m_samplingRate = Sensor::SixteenHertz;
		} else if (tokens[0] == "E") {
			m_samplingRate = Sensor::ThirtyTwoHertz;
		} else {
			qWarning(QString("Failed to parse %1 as a number or sampling rate").arg(tokens[1]).toLatin1());
			return false;
		}
		m_number = 10;
	}

	m_x = tokens[1].toDouble(&ok);
	if (!ok) {
		qWarning(QString("Failed to parse %1 as an x value").arg(tokens[1]).toLatin1());
		return false;
	}

	m_y = tokens[2].toDouble(&ok);
	if (!ok) {
		qWarning(QString("Failed to parse %1 as an y value").arg(tokens[2]).toLatin1());
		return false;
	}

	m_z = tokens[3].toDouble(&ok);
	if (!ok) {
		qWarning(QString("Failed to parse %1 as a z value").arg(tokens[3]).toLatin1());
		return false;
	}

	m_battery = tokens[4].toDouble(&ok);
	if (!ok) {
		qWarning(QString("Failed to parse %1 as a battery level value").arg(tokens[4]).toLatin1());
		return false;
	}

	m_temperature = tokens[5].toDouble(&ok);
	if (!ok) {
		qWarning(QString("Failed to parse %1 as a temperature value").arg(tokens[5]).toLatin1());
		return false;
	}

	m_conductance = tokens[6].toDouble(&ok);
	if (!ok) {
		qWarning(QString("Failed to parse %1 as a conductance value").arg(tokens[6]).toLatin1());
		return false;
	}

	return true;
}

int SensorReading::number() const
{
	return m_number;
}

Sensor::SamplingRate SensorReading::samplingRate() const
{
	return m_samplingRate;
}

double SensorReading::x() const
{
	return m_x;
}

double SensorReading::y() const
{
	return m_y;
}

double SensorReading::z() const
{
	return m_z;
}

double SensorReading::battery() const
{
	return m_battery;
}

double SensorReading::temperature() const
{
	return m_temperature;
}

double SensorReading::conductance() const
{
	return m_conductance;
}

QDebug operator<<(QDebug debug, const SensorReading &reading)
{
	debug.nospace() << "number: " << reading.number() << ", ";
	debug.nospace() << "samplingRate: ";
	if (reading.samplingRate() == Sensor::UnknownRate) {
		debug.nospace() << "Unknown, ";
	} else {
		debug.nospace() << reading.samplingRate() << " Hz, ";
	}
	debug.nospace() << "x: " << reading.x() << " G, ";
	debug.nospace() << "y: " << reading.y() << " G, ";
	debug.nospace() << "z: " << reading.z() << " G, ";
	debug.nospace() << "battery: " << reading.battery() << " V, ";
	debug.nospace() << "temperature: " << reading.temperature() << " C, ";
	debug.nospace() << "conductance: " << reading.conductance() << " µS";
	return debug.space();
}
