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
			qDebug() << reading;
		}
	}
}
