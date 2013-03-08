TEMPLATE = lib

greaterThan(QT_MAJOR_VERSION, 4) {
	QT += serialport
}
CONFIG += staticlib serialport

SOURCES = sensor.cpp sensorreading.cpp
HEADERS = sensor.h sensorreading.h
