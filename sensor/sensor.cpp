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

void Sensor::start()
{
	if (m_serial.open(QIODevice::ReadOnly)) {
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
	stop();
	m_serial.setPort(portName);
	start();
}

void Sensor::readData()
{
	SensorReading reading;
	while (m_serial.canReadLine()) {
		QByteArray line = m_serial.readLine().trimmed();
		if (!reading.fromCsv(line)) {
			emit error(QString("Failed to parse \"%1\" as a reading due to: %2").arg(QString(line)).arg(reading.lastError()));
		} else {
			m_readings.append(reading);
			qDebug() << reading;
			if (!m_readings.isEmpty() && QDateTime::currentDateTimeUtc().msecsTo(m_readings.first().time()) >= 5000) {
				qDebug() << "==> Process " << m_readings.size() << " readings in buffer:";
				qDebug() << "==>   From " << m_readings.first().time().toString("hh:mm:ss.zzz") + " to " + m_readings.last().time().toString("hh:mm:ss.zzz");
				emit batchAvailable(m_readings);
				m_readings.clear();
			}
		}
	}
}
