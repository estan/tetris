include(../common.pri)

TEMPLATE = lib
CONFIG += staticlib

SOURCES = sensorreading.cpp
HEADERS = sensorreading.h

serialport {
	SOURCES += sensor.cpp
	HEADERS += sensor.h
}
