#include <QDebug>
#include <QThread>
#include <QByteArray>
#include <QTextStream>
#include <QtSerialPort/QSerialPort>

#include "sensorreading.h"
#include "sensor.h"

Sensor::Sensor(const QString &portName, QObject *parent) : QObject(parent),
	m_serial(this)
{
	m_serial.setBaudRate(QSerialPort::Baud115200);
	m_serial.setDataBits(QSerialPort::Data8);
	m_serial.setStopBits(QSerialPort::OneStop);
	m_serial.setParity(QSerialPort::NoParity);
	m_serial.setPortName(portName);

	connect(&m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

Sensor::~Sensor()
{
	if (m_serial.isOpen()) {
		m_serial.close();
	}
}

void Sensor::start()
{
	if (m_serial.open(QIODevice::ReadOnly)) {
		m_serial.clear(QSerialPort::Input);
		emit started();
	} else {
		emit error("Could not open serial port!");
	}
}

void Sensor::stop()
{
	if (m_serial.isOpen()) {
		m_serial.close();
		emit stopped();
	}
}

void Sensor::setPort(const QString &portName) {
	m_serial.setPortName(portName);
}

void Sensor::readData()
{
	SensorReading reading;
	while (m_serial.canReadLine()) {
		QByteArray line = m_serial.readLine().trimmed();
		if (!reading.fromCsv(line)) {
			emit error(QString("Error parsing \"%1\": %2").arg(QString(line)).arg(reading.lastError()));
		} else {
			m_readings.append(reading);
			qDebug() << reading;
			qint64 sinceOldest = m_readings.first().time().msecsTo(QDateTime::currentDateTimeUtc());
			if (!m_readings.isEmpty() && sinceOldest >= 5000) {
				emit batchAvailable(m_readings);
				m_readings.clear();
			}
		}
	}
}
