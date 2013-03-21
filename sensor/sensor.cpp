#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QByteArray>
#include <QTextStream>
#include <QtSerialPort/QSerialPort>

#include "sensorreading.h"
#include "sensor.h"

Sensor::Sensor(const QString &portName, QObject *parent) : QObject(parent),
	m_serialPort(this),
	m_state(Stopped),
	m_connectionAttempt(1)
{
	m_serialPort.setBaudRate(QSerialPort::Baud115200);
	m_serialPort.setDataBits(QSerialPort::Data8);
	m_serialPort.setStopBits(QSerialPort::OneStop);
	m_serialPort.setParity(QSerialPort::NoParity);
	m_serialPort.setPortName(portName);

	connect(&m_serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
}

Sensor::~Sensor()
{
	if (m_serialPort.isOpen()) {
		m_serialPort.close();
	}
}

void Sensor::start()
{
	if (state() == Started) {
		stop();
	}
	if (m_connectionAttempt > 10) {
		emit error("Connection failed after 10 attempts");
		stop();
		return;
	}
	setState(Connecting);
	if (m_serialPort.open(QIODevice::ReadOnly)) {
		m_connectionAttempt = 1;
		m_serialPort.clear(QSerialPort::Input);
		setState(Started);
	} else {
		// Retry in 3 seconds.
		m_connectionAttempt++;
		emit error(m_serialPort.errorString());
		QTimer::singleShot(3000, this, SLOT(start()));
	}
}

void Sensor::stop()
{
	if (m_serialPort.isOpen()) {
		m_serialPort.close();
	}
	setState(Stopped);
}

void Sensor::setPort(const QString &portName) {
	m_serialPort.setPortName(portName);
}

void Sensor::readData()
{
	SensorReading reading;
	while (m_serialPort.canReadLine()) {
		QByteArray line = m_serialPort.readLine().trimmed();
		if (!reading.fromCsv(line)) {
			emit error(reading.lastError());
			break;
		}
		m_readings.append(reading);
		qDebug() << reading;
		QDateTime firstTime = m_readings.first().time();
		if (firstTime.msecsTo(QDateTime::currentDateTimeUtc()) >= 5000) {
			// We have 5 seconds of readings, so emit the batch.
			emit batchAvailable(m_readings);
			m_readings.clear();
		}
	}
}

Sensor::State Sensor::state() const
{
	return m_state;
}

void Sensor::setState(State state)
{
	m_state = state;
	emit stateChanged(state);
}
