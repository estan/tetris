# Check for QtSerialPort (TODO: Update when Qt 5.1 is out)
exists($$[QT_INSTALL_HEADERS]/QtSerialPort/QSerialPort) {
	greaterThan(QT_MAJOR_VERSION, 4) {
		QT += widgets serialport
	}
	CONFIG += warn_on serialport
}
