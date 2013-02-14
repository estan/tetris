#ifndef SENSOR_H
#define SENSOR_H

#include <QObject>
#include <QByteArray>
#include <QtSerialPort/QSerialPort>

class Sensor : public QObject
{
	Q_OBJECT

public:
	enum SamplingRate
	{
		TwoHertz = 2,
		FourHertz = 4,
		EightHertz = 8,
		SixteenHertz = 16,
		ThirtyTwoHertz = 32,
		UnknownRate = -1
	};

	explicit Sensor(const QString &portName, QObject *parent = 0);

	bool start();

private slots:
	void readData();

private:
	QSerialPort m_serial;
	QByteArray m_buffer;
};

#endif // SENSOR_H
