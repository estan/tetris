/***********************************************************************
 *
 * Copyright (C) 2008, 2012 Graeme Gott <graeme@gottcode.org>
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

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

#include "sensor.h"

class QAction;
class QLabel;
class QString;
class QThread;
class Board;
class ScoreBoard;
class SettingsDialog;

class Window : public QMainWindow
{
	Q_OBJECT
public:
	Window(QWidget *parent = 0, Qt::WindowFlags wf = 0);
	~Window();

protected:
	virtual void closeEvent(QCloseEvent* event);

private slots:
	void pauseAvailable(bool available);
	void scoreUpdated(int score);
	void newGame();
	void gameOver();
	void about();
	void setLocale();

	void showSettings();
	void sensorStateChanged(Sensor::State state);
	void sensorError(const QString &error);

private:
	void initSensor();

private:
	Board* m_board;
	QAction* m_pause_action;
	QAction* m_resume_action;
	QLabel* m_preview;
	QLabel* m_level;
	QLabel* m_lines;
	QLabel* m_score;
	QLabel* m_sensorStatus;
	ScoreBoard* m_score_board;
	SettingsDialog* m_settingsDialog;
	QThread* m_sensorThread;
};

#endif // WINDOW_H
