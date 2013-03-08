#ifndef SENSOR_H
#define SENSOR_H

#include <QObject>
#include <QList>
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
	enum StressLevel {
		LowLevel,
		MediumLevel,
		HighLevel
	};

	explicit Sensor(const QString &portName, QObject *parent = 0);

	bool start();

	void updateStressLevel();
	void setLimits(double low, double high);

signals:
	void stressLevelChanged(StressLevel stressLevel);

private slots:
	void readData();

private:
	QSerialPort m_serial;
	QList<double> m_backLog;

	double m_lowLimit;
	double m_highLimit;

	StressLevel m_currentStressLevel;
};

#endif // SENSOR_H
