#include <QObject>
#include <QString>
#include <QSerialPort>
#include <QTimer>

class Simulator : public QObject
{
	Q_OBJECT

public:
	explicit Simulator(QObject *parent = 0);
	virtual ~Simulator();
	bool start(const QString &portName);

private slots:
	void writeReading();

private:
	QSerialPort m_serial;
	QTimer m_timer;
	int m_numReadings;
};
