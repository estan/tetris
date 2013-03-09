#include <QTextStream>
#include <QFile>

#include "console.h"

Console::Console(QObject *parent) :
    QThread(parent)
{
}

void Console::run()
{
	QTextStream qin(stdin, QFile::ReadOnly);
	forever {
		QString line = qin.readLine();
		if (!line.isNull()) {
			emit exit();
		}
	}
}
