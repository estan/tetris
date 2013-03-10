#include <QCoreApplication>

#include "simulator.h"

int main (int argc, char *argv[])
{
	if (argc != 3 && argc != 4) {
		qWarning("Usage: simulator <port> <frequency>");
		qWarning("       simulator <port> <frequency> <infile>");
		return 1;
	}
	int frequency = 0;
	bool ok = false;
	frequency = QString(argv[2]).toInt(&ok);
	if (!ok) {
		qWarning("Frequency must be an integer");
		return 1;
	}

	QCoreApplication app(argc, argv);

	Simulator *simulator;
	if (argc == 3) {
		simulator = new Simulator(&app);
	} else if (argc == 4) {
		simulator = new Simulator(argv[3], &app);
	}

	if (!simulator->start(argv[1], frequency)) {
		qWarning("Failed to start simulator");
		return 1;
	}

	return app.exec();
}
