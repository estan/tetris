#ifndef CONSOLE_H
#define CONSOLE_H

#include <QThread>

class Console : public QThread
{
	Q_OBJECT
public:
	explicit Console(QObject *parent = 0);
	
signals:
	void exit();

protected:
	void run();
};

#endif // CONSOLE_H
