/***********************************************************************
 *
 * Copyright (C) 2007-2008 Graeme Gott <graeme@gottcode.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "window.h"

#if defined(QTOPIA_PHONE)
#include <QtopiaApplication>
QTOPIA_ADD_APPLICATION(QTOPIA_TARGET,Window)
QTOPIA_MAIN
#else

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("Gottet");
	app.setOrganizationDomain("gottcode.org");
	app.setOrganizationName("GottCode");

	QTranslator qt_translator;
	qt_translator.load("qt_" + QLocale::system().name());
	app.installTranslator(&qt_translator);

	QTranslator gottet_translator;
	gottet_translator.load("gottet_" + QLocale::system().name());
	app.installTranslator(&gottet_translator);

	Window window;
	window.show();

	return app.exec();
}
#endif
