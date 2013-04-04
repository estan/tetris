#ifndef SENSOR_H
#define SENSOR_H

#include <QObject>
#include <QList>
#include <QtSerialPort/QSerialPort>

#include "sensorreading.h"

class Sensor : public QObject
{
	Q_OBJECT
	Q_ENUMS(State)

public:
	enum State {
		Connecting,
		Started,
		Stopped
	};

	explicit Sensor(const QString &portName, QObject *parent = 0);
	virtual ~Sensor();

	State state() const;

signals:
	void stateChanged(Sensor::State state);
	void analyze(QList<SensorReading> readings);
	void error(const QString &error);

public slots:
	void start();
	void stop();
	void setPort(const QString &portName);

private slots:
	void readData();
	void setState(Sensor::State state);

private:
	QSerialPort m_serialPort;
	QList<SensorReading> m_readings;
	State m_state;
	int m_connectionAttempt;
};

Q_DECLARE_METATYPE(Sensor::State)

#endif // SENSOR_H
