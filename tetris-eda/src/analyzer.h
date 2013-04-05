#ifndef ANALYZER_H
#define ANALYZER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QFile>

#include "engine.h"
#include "sensorreading.h"

class Analyzer : public QObject
{
	Q_OBJECT

public:
	enum StressLevel {
		Low,
		Medium,
		High
	};

	explicit Analyzer(const QString &outDir, QObject *parent = 0);
	virtual ~Analyzer();

signals:
	void stressLevelChanged(Analyzer::StressLevel stressLevel);
	
public slots:
	void start();
	void stop();
	void analyze(const QList<SensorReading> &readings);

private:
	QString m_ledalabPath;
	Engine *m_matlabEngine;
	QString m_outDir;
	bool m_isStarted;
	QFile m_mediansFile;
};

#endif // ANALYZER_H
