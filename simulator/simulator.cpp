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

bool Simulator::start(const QString &portName)
{
	m_serial.setPort(portName);
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

	qDebug() << "Simulator is running, press Enter to stop";

	m_timer.start(125);
	return true;
}

void Simulator::stop()
{
	qDebug() << "Stopping simulator...";
	qApp->exit(0);
}

void Simulator::writeReading()
{
	m_numReadings++;
	if (m_mode == FakeMode) {
		double eda = 3 + qSin(m_numReadings*(M_PI/180));
		QString row = QString("0,0.39,0.12,0.86,3.55,32.9,%1\r\n").arg(eda);
		m_serial.write(row.toLatin1());
	} else if (m_mode == FileMode) {
		m_serial.write(m_file.readLine());
	} else {
		qWarning("Unknown mode");
		return;
	}
	m_serial.flush();
}
