TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 4) {
	QT += serialport
}
CONFIG += staticlib serialport

HEADERS += simulator.h
SOURCES += main.cpp simulator.cpp