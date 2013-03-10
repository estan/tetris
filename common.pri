CONFIG += warn_on

# For Qt 5.x we need the widgets module.
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}

# Check for QtSerialPort (TODO: Update when Qt 5.1 is out)
exists($$[QT_INSTALL_HEADERS]/QtSerialPort/QSerialPort) {
	greaterThan(QT_MAJOR_VERSION, 4) {
		QT += serialport
	}
	CONFIG += serialport
}
