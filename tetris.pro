include(common.pri)

TEMPLATE = subdirs
SUBDIRS = sensor

serialport {
	SUBDIRS += tetris-eda simulator
	tetris-eda.depends = sensor
} else {
	message("No QtSerialPort found: tetris-eda and simulator will be excluded.")
}

