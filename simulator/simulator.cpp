#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QCoreApplication>
#include <QtCore/qmath.h>

#include <math.h>

#include "console.h"
#include "simulator.h"

Simulator::Simulator(QObject *parent) : QObject(parent), m_mode(FakeMode)
{
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(writeReading()));
}

Simulator::Simulator(const QString &fileName, QObject *parent) : QObject(parent),
	m_mode(FileMode),
	m_file(fileName)
{
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(writeReading()));
}

Simulator::~Simulator()
{
	if (m_serial.isOpen())
		m_serial.close();
}

bool Simulator::start(const QString &portName, int frequency)
{
	if (frequency < 1 || frequency > 100) {
		qWarning("Unsupported frequency");
		return false;
	}
	m_serial.setPortName(portName);
	if (!m_serial.open(QIODevice::WriteOnly)) {
		qWarning("Failed to open serial port");
		return false;
	}
	if (m_mode == FileMode) {
		if (!m_file.open(QIODevice::ReadOnly)) {
			qWarning("Failed to open input file");
			return false;
		}
	}

	// Start console thread.
	Console *console = new Console();
	connect(console, SIGNAL(exit()), this, SLOT(stop()));
	connect(console, SIGNAL(finished()), console, SLOT(deleteLater()));
	console->start();

	qDebug() << "Simulator is running, press [Enter] to stop";

	m_timer.start(qFloor(1000/frequency));
	return true;
}

void Simulator::stop()
{
	qDebug() << "Stopping simulator...";
	if (m_serial.isOpen()) {
		m_serial.close();
	}
	qApp->exit(0);
}

void Simulator::writeReading()
{
	m_numReadings++;
	QByteArray reading;

	if (m_mode == FakeMode) {
		// Write a sine wave, elevated by 3.
		double eda = 3 + qSin(m_numReadings*(M_PI/180));
		reading = QString("0,0.39,0.12,0.86,3.55,32.9,%1\r\n").arg(eda).toLatin1();
	} else if (m_mode == FileMode) {
		// Write a line from the input file.
		reading = m_file.readLine();
		if (reading.isEmpty()) {
			qDebug() << "End of input file reached";
			stop();
		}
	} else {
		qWarning("Unknown mode");
		return;
	}

	if (m_serial.write(reading) == -1) {
		qWarning("Failed to write to serial port");
		return;
	}
	m_serial.flush();
}
