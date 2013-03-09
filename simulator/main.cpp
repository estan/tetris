#include <QCoreApplication>

#include "simulator.h"

int main (int argc, char *argv[])
{
	if (argc != 2 && argc != 3) {
		qWarning("Usage: simulator <port>");
		qWarning("       simulator <port> <infile>");
		return 1;
	}

	QCoreApplication app(argc, argv);

	Simulator *simulator;
	if (argc == 2) {
		simulator = new Simulator(&app);
	} else if (argc == 3) {
		simulator = new Simulator(argv[2], &app);
	}

	if (!simulator->start(argv[1])) {
		qWarning("Exiting...");
		return 1;
	}

	return app.exec();
}
