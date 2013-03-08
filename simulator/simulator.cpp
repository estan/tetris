#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QtCore/qmath.h>

#include <math.h>

#include "simulator.h"

Simulator::Simulator(QObject *parent) : QObject(parent)
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
	m_timer.start(125);
	return true;
}

void Simulator::writeReading()
{
	m_numReadings++;
	double eda = 3 + qSin(m_numReadings*(M_PI/180));
	QString row = QString("0,0.39,0.12,0.86,3.55,32.9,%1\r\n").arg(eda);
	m_serial.write(row.toLatin1());
	m_serial.flush();
}
