TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets serialport
}
CONFIG += warn_on serialport
macx {
	CONFIG += x86_64
}

INCLUDEPATH += ../sensor
LIBS += -L../sensor -lsensor

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

VERSION = $$system(git rev-parse --short HEAD)
isEmpty(VERSION) {
	VERSION = 0
}
DEFINES += VERSIONSTR=\\\"git.$${VERSION}\\\"

unix: !macx {
	TARGET = tetris-eda
} else {
	TARGET = TetrisEDA
}

HEADERS = src/board.h \
	src/locale_dialog.h \
	src/piece.h \
	src/score_board.h \
	src/window.h

SOURCES = src/board.cpp \
	src/locale_dialog.cpp \
	src/main.cpp \
	src/piece.cpp \
	src/score_board.cpp \
	src/window.cpp

TRANSLATIONS = translations/tetris-eda_en.ts \
	translations/tetris-eda_fr.ts

macx {
	ICON = icons/tetris-eda.icns
} else:unix {
	RESOURCES = icons/icon.qrc
} else:win32 {
	RC_FILE = icons/icon.rc
}

unix: !macx {
	isEmpty(PREFIX) {
		PREFIX = /usr/local
	}
	isEmpty(BINDIR) {
		BINDIR = bin
	}

	target.path = $$PREFIX/$$BINDIR/

	icon.files = icons/tetris-eda.png
	icon.path = $$PREFIX/share/icons/hicolor/48x48/apps

	desktop.files = icons/tetris-eda.desktop
	desktop.path = $$PREFIX/share/applications/

	qm.files = translations/*.qm
	qm.path = $$PREFIX/share/tetris-eda/translations

	INSTALLS += target icon desktop qm
}
