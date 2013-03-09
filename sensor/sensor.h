#ifndef SENSOR_H
#define SENSOR_H

#include <QObject>
#include <QList>
#include <QtSerialPort/QSerialPort>

#include "sensorreading.h"

class Sensor : public QObject
{
	Q_OBJECT

public:
	explicit Sensor(const QString &portName, QObject *parent = 0);

	void start();
	void stop();

signals:
	void started();
	void stopped();
	void batchAvailable(QList<SensorReading> readings);
	void error(const QString &error);

public slots:
	void setPort(const QString &portName);

private slots:
	void readData();

private:
	QSerialPort m_serial;
	QList<SensorReading> m_readings;
};

#endif // SENSOR_H
