#include <QCoreApplication>

#include "simulator.h"

int main (int argc, char *argv[])
{
	if (argc != 2) {
		qWarning("Usage: SensorSimulator <port>");
		return 1;
	}

	QCoreApplication app(argc, argv);

	Simulator simulator;
	simulator.start(argv[1]);

	return app.exec();
}
