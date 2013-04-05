#include <QList>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <QDir>

#include "engine.h"
#include "sensorreading.h"
#include "analyzer.h"

#define BUFSIZE 1024

Analyzer::Analyzer(const QString &outDir, QObject *parent) : QObject(parent),
	m_ledalabPath("/home/estan/Projekt/tetris/3rdparty/Ledalab"),
	m_matlabEngine(0),
	m_outDir(outDir),
	m_isStarted(false),
	m_mediansFile("/home/estan/scr_medians.csv")
{
	if (!m_mediansFile.open(QIODevice::WriteOnly)) {
		qDebug() << "Failed to open medians file";
		return;
	}
}

Analyzer::~Analyzer()
{
	m_mediansFile.close();
}

void Analyzer::start()
{
	if (m_isStarted)
		return;

	m_matlabEngine = engOpen("");
	if (!m_matlabEngine) {
		qDebug() << "Can't start MATLAB engine";
		return;
	}

	// Create output directory if it doesn't exist.
	QDir dir(m_outDir);
	if (!dir.exists()) {
		dir.mkdir(m_outDir);
	}

	// Enter the Ledalab directory.
	if (engEvalString(m_matlabEngine, QString("cd '%1'").arg(m_ledalabPath).toUtf8()) == 1) {
		qDebug() << "Failed to enter Ledalab directory";
		return;
	}

	m_isStarted = true;
}

void Analyzer::stop()
{
	if (!m_isStarted)
		return;

	if (m_matlabEngine != 0)
		engClose(m_matlabEngine);

	m_isStarted = false;
}

void Analyzer::analyze(const QList<SensorReading> &readings)
{
	if (!m_isStarted)
		return;

	// Create output directory if it doesn't exist.
	QString startTime = readings.first().time().toLocalTime().toString();
	QString endTime = readings.last().time().toLocalTime().toString();
	QString dirName = m_outDir + "/" + startTime + "-" + endTime;
	QDir dir(dirName);
	if (!dir.exists()) {
		dir.mkdir(dirName);
	}

	// Write SC data to input file.
	QFile file(dirName + "/scdata.csv");
	if (!file.open(QIODevice::WriteOnly)) {
		qWarning("Failed to open scdata.csv file");
		return;
	}
	foreach (const SensorReading &reading, readings) {
		QString conductance = QString::number(reading.conductance());
		file.write(QString("%1\n").arg(conductance).toUtf8());
	}
	file.close();

	// Set up output buffer for MATLAB.
	char result[BUFSIZE + 1];
	result[BUFSIZE] = '\0';
	engOutputBuffer(m_matlabEngine, result, BUFSIZE - 1);

	// Run Ledalab on input directory.
	QString ledalabCmd = QString("Ledalab('%1', 'open', 'text2', 'analyze', 'CDA', 'optimize', 1, 'export_scrlist', [0.05 5])").arg(dirName);
	qDebug() << "Running: " << ledalabCmd;
	if (engEvalString(m_matlabEngine, ledalabCmd.toUtf8()) == 1) {
		qDebug() << "Failed to run Ledalab";
		return;
	}

	// Open output file.
	QFile outFile(dirName + "/scdata_scrlist.csv");
	if (!outFile.open(QIODevice::ReadOnly)) {
		qWarning("Failed to open scdata_scrlist.csv");
		return;
	}

	// Read list of SCRs.
	QList<double> amplitudes;
	double amplitude;
	QTextStream stream(&outFile);
	while (!stream.atEnd()) {
		bool ok = true;
		amplitude = stream.readLine().trimmed().toDouble(&ok);
		if (ok)
			amplitudes.append(amplitude);
	}

	// Calculate median amplitude.
	qSort(amplitudes);
	int nAmplitudes = amplitudes.length();
	double medianAmplitude = 0.0;
	if (!amplitudes.isEmpty()) {
		if ((nAmplitudes % 2) == 0) {
			medianAmplitude = (amplitudes.at(nAmplitudes/2) + amplitudes.at(nAmplitudes/2 - 1))/2;
		} else {
			medianAmplitude = amplitudes.at(amplitudes.length()/2);
		}
	}

	// Emit appropriate signal.
	if (medianAmplitude > 0.6445) {
		qDebug() << "Stress level changed to " << High;
		emit stressLevelChanged(High);
	} else if (medianAmplitude > 0.6099) {
		qDebug() << "Stress level changed to " << Medium;
		emit stressLevelChanged(Medium);
	} else {
		qDebug() << "Stress level changed to " << Low;
		emit stressLevelChanged(Low);
	}

	// Print output.
	qDebug() << "Median amplitude:\n" << medianAmplitude;
	QTextStream mediansStream(&m_mediansFile);
	mediansStream << QString::number(medianAmplitude) + "\n";
}
