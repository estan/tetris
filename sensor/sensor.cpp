#include <QDebug>
#include <QByteArray>
#include <QTextStream>
#include <QtSerialPort/QSerialPort>

#include "sensorreading.h"
#include "sensor.h"

Sensor::Sensor(const QString &portName, QObject *parent) : QObject(parent)
{
	m_serial.setBaudRate(QSerialPort::Baud115200);
	m_serial.setDataBits(QSerialPort::Data8);
	m_serial.setStopBits(QSerialPort::OneStop);
	m_serial.setParity(QSerialPort::NoParity);

	m_serial.setPort(portName);

	connect(&m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

bool Sensor::start()
{
	if (!m_serial.open(QIODevice::ReadWrite)) {
		qWarning("Could not open serial port!");
		return false;
	}
	return true;
}

void Sensor::readData()
{
	SensorReading reading;
	while (m_serial.canReadLine()) {
		QByteArray line = m_serial.readLine().trimmed();
		if (reading.fromCsv(line)) {
			m_backLog.append(reading.conductance());
			if (m_backLog.size() > 10) {
				m_backLog.removeFirst();
				updateStressLevel();
			}
		}
	}
}

void Sensor::updateStressLevel()
{
	// Calculate average conductance over last 10 readings.
	double sum = 0;
	foreach (double conductance, m_backLog) {
		sum += conductance;
	}
	double average = sum / 10;

	// Calculate stress level.
	StressLevel stressLevel;
	if (average > m_highLimit) {
		stressLevel = HighLevel;
	} else if (average > m_lowLimit) {
		stressLevel = MediumLevel;
	} else {
		stressLevel = LowLevel;
	}

	if (stressLevel != m_currentStressLevel) {
		// Actual change in stress level has occurred.
		m_currentStressLevel = stressLevel;
		emit stressLevelChanged(m_currentStressLevel);
	}
}

void Sensor::setLimits(double low, double high)
{
	m_lowLimit = low;
	m_highLimit = high;
}
