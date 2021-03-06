include(../common.pri)

TEMPLATE = app

macx {
	CONFIG += x86_64
}

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
	src/window.h \
	src/settings_dialog.h \
    src/analyzer.h

SOURCES = src/board.cpp \
	src/locale_dialog.cpp \
	src/main.cpp \
	src/piece.cpp \
	src/score_board.cpp \
	src/window.cpp \
	src/settings_dialog.cpp \
    src/analyzer.cpp

FORMS += src/settings_dialog.ui

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../sensor/release/ -lsensor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../sensor/debug/ -lsensor
else:unix: LIBS += -L$$OUT_PWD/../sensor/ -lsensor

INCLUDEPATH += $$PWD/../sensor
DEPENDPATH += $$PWD/../sensor

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sensor/release/sensor.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../sensor/debug/sensor.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../sensor/libsensor.a

# MATLAB libraries and includes
MATLAB_PREFIX = /home/estan/Blandat/MATLAB
MATLAB_INCLUDES = $$MATLAB_PREFIX/R2012a/extern/include
MATLAB_LIBS = $$MATLAB_PREFIX/R2012a/bin/glnxa64

LIBS += -L$$MATLAB_LIBS -leng -lmx
INCLUDEPATH += $$MATLAB_INCLUDES
DEPENDPATH += $$MATLAB_LIBS

unix:QMAKE_LFLAGS += -Wl,-rpath,$$MATLAB_LIBS,-rpath-link,$$MATLAB_LIBS
