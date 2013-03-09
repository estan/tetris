#include <QObject>
#include <QString>
#include <QSerialPort>
#include <QTimer>
#include <QFile>

class Simulator : public QObject
{
	Q_OBJECT

public:
	enum Mode {
		FakeMode,
		FileMode
	};

	explicit Simulator(QObject *parent = 0);
	explicit Simulator(const QString &fileName, QObject *parent = 0);
	virtual ~Simulator();
	bool start(const QString &portName);

private slots:
	void writeReading();
	void stop();

private:
	Mode m_mode;
	QSerialPort m_serial;
	QTimer m_timer;
	QFile m_file;
	int m_numReadings;
};
