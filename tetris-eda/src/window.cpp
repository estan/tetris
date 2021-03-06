/***********************************************************************
 *
 * Copyright (C) 2008, 2010, 2012 Graeme Gott <graeme@gottcode.org>
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

#include "board.h"
#include "locale_dialog.h"
#include "score_board.h"
#include "settings_dialog.h"
#include "sensor.h"

#include <ctime>

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QStatusBar>
#include <QString>
#include <QThread>

#include "analyzer.h"

/*****************************************************************************/

Window::Window(QWidget *parent, Qt::WindowFlags wf)
:	QMainWindow(parent, wf),
  m_sensorThread(0),
  m_analyzerThread(0)
{
	setWindowTitle(tr("Tetris EDA"));
	setWindowIcon(QIcon(":/tetris-eda.png"));

	// Create settings dialog.
	m_settingsDialog = new SettingsDialog(this);

	QWidget* contents = new QWidget(this);
	setCentralWidget(contents);

	// Create preview
	m_preview = new QLabel(contents);
	m_preview->setFixedSize(80, 100);
	m_preview->setAutoFillBackground(true);
	{
		QPalette palette = m_preview->palette();
		palette.setColor(m_preview->backgroundRole(), Qt::black);
		m_preview->setPalette(palette);
	}

	// Create level display
	m_level = new QLabel("0", contents);
	m_level->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_level->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	m_level->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	// Create lines display
	m_lines = new QLabel("0", contents);
	m_lines->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_lines->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	m_lines->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	// Create score display
	m_score = new QLabel("0", contents);
	m_score->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_score->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	m_score->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	// Create sensor status display
	m_sensorStatus = new QLabel("Stopped", contents);
	m_sensorStatus->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_sensorStatus->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	m_sensorStatus->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	// Create scoreboard
	m_score_board = new ScoreBoard(this);

	// Create board
	srand(time(0));
	m_board = new Board(true, contents);
	connect(m_board, SIGNAL(pauseAvailable(bool)), this, SLOT(pauseAvailable(bool)));
	connect(m_board, SIGNAL(nextPieceAvailable(QPixmap)), m_preview, SLOT(setPixmap(QPixmap)));
	connect(m_board, SIGNAL(levelUpdated(int)), m_level, SLOT(setNum(int)));
	connect(m_board, SIGNAL(linesRemovedUpdated(int)), m_lines, SLOT(setNum(int)));
	connect(m_board, SIGNAL(scoreUpdated(int)), this, SLOT(scoreUpdated(int)));
	connect(m_board, SIGNAL(gameOver(int, int, int)), m_score_board, SLOT(addHighScore(int, int, int)));
	connect(m_board, SIGNAL(gameOver(int, int, int)), this, SLOT(gameOver()));
	connect(m_board, SIGNAL(gameStarted()), this, SLOT(newGame()));

	// Create overlay message
	QLabel* message = new QLabel(tr("Click to start a new game."), contents);
	message->setAttribute(Qt::WA_TransparentForMouseEvents);
	message->setAlignment(Qt::AlignCenter);
	message->setStyleSheet(
		"QLabel {"
			"background-color: rgba(255, 255, 255, 200);"
			"color: black;"
			"margin: 0;"
			"padding: 0.5em;"
			"border-radius: 0.5em;"
		"}");
	message->setWordWrap(true);
	connect(m_board, SIGNAL(showMessage(const QString&)), message, SLOT(show()));
	connect(m_board, SIGNAL(showMessage(const QString&)), message, SLOT(setText(const QString&)));
	connect(m_board, SIGNAL(hideMessage()), message, SLOT(hide()));
	connect(m_board, SIGNAL(hideMessage()), message, SLOT(clear()));

	// Create menus
	QMenu* menu = menuBar()->addMenu(tr("&Game"));
	menu->addAction(tr("&New"), m_board, SLOT(newGame()), tr("Ctrl+N"));
	m_pause_action = menu->addAction(tr("&Pause"), m_board, SLOT(pauseGame()), tr("P"));
	m_pause_action->setEnabled(false);
	m_resume_action = menu->addAction(tr("&Resume"), m_board, SLOT(resumeGame()), tr("P"));
	m_resume_action->setVisible(false);
	menu->addSeparator();
	menu->addAction(tr("&Scores"), m_score_board, SLOT(show()));
	menu->addSeparator();
	menu->addAction(tr("&Quit"), this, SLOT(close()), tr("Ctrl+Q"));

	menu = menuBar()->addMenu(tr("&Settings"));
	menu->addAction(tr("Application &Language..."), this, SLOT(setLocale()));
	menu->addAction(tr("Settings..."), this, SLOT(showSettings()));

	menu = menuBar()->addMenu(tr("&Help"));
	menu->addAction(tr("&About"), this, SLOT(about()));
	menu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));

	// Layout window
	QGridLayout* layout = new QGridLayout(contents);
	layout->setMargin(12);
	layout->setSpacing(0);
	layout->setColumnStretch(0, 1);
	layout->setColumnMinimumWidth(1, 12);
	layout->setRowStretch(11, 1);
	layout->setRowMinimumHeight(2, 24);
	layout->setRowMinimumHeight(5, 24);
	layout->setRowMinimumHeight(8, 24);
	layout->addWidget(m_board, 0, 0, 12, 1);
	layout->addWidget(message, 0, 0, 12, 1, Qt::AlignCenter);
	layout->addWidget(new QLabel(tr("Next Piece"), contents), 0, 2, 1, 1, Qt::AlignCenter);
	layout->addWidget(m_preview, 1, 2, Qt::AlignCenter);
	layout->addWidget(new QLabel(tr("Level"), contents), 3, 2, 1, 1, Qt::AlignCenter);
	layout->addWidget(m_level, 4, 2);
	layout->addWidget(new QLabel(tr("Removed Lines"), contents), 6, 2, 1, 1, Qt::AlignCenter);
	layout->addWidget(m_lines, 7, 2);
	layout->addWidget(new QLabel(tr("Score"), contents), 9, 2, 1, 1, Qt::AlignCenter);
	layout->addWidget(m_score, 10, 2);
	layout->addWidget(new QLabel(tr("Sensor Status"), contents), 12, 2, 1, 1, Qt::AlignCenter);
	layout->addWidget(m_sensorStatus, 13, 2);

	// Restore window
	restoreGeometry(QSettings().value("Geometry").toByteArray());

	// Show settings dialog if there are missing settings.
	if (!QSettings().contains("SerialPort") || !QSettings().contains("MonitorPort")) {
		showSettings();
	}

	// Initialize sensor / analyzer.
	initSensor();
}

Window::~Window()
{
	if (m_sensorThread && m_sensorThread->isRunning()) {
		m_sensorThread->quit();
	}
}

/*****************************************************************************/

void Window::closeEvent(QCloseEvent* event)
{
	QSettings().setValue("Geometry", saveGeometry());
	if (!m_board->endGame()) {
		event->ignore();
	}
}

/*****************************************************************************/

void Window::pauseAvailable(bool available)
{
	static QPixmap preview;

	if (available) {
		m_pause_action->setVisible(true);
		m_resume_action->setVisible(false);
		m_preview->setPixmap(preview);
	} else {
		m_resume_action->setVisible(true);
		m_pause_action->setVisible(false);
		preview = *m_preview->pixmap();
		m_preview->clear();
	}
}

/*****************************************************************************/

void Window::scoreUpdated(int score)
{
	m_score->setText(QString("%L1").arg(score));

	int position = m_score_board->highScorePosition(score);
	QPalette palette = m_score->palette();
	if (position == 0) {
		palette.setColor(m_preview->foregroundRole(), Qt::red);
	} else if (position < 10) {
		palette.setColor(m_preview->foregroundRole(), Qt::blue);
	}
	m_score->setPalette(palette);
}

/*****************************************************************************/

void Window::newGame()
{
	m_pause_action->setEnabled(true);

	QPalette palette = m_score->palette();
	palette.setColor(m_preview->foregroundRole(), Qt::black);
	m_score->setPalette(palette);
}

/*****************************************************************************/

void Window::gameOver()
{
	m_pause_action->setEnabled(false);
}

/*****************************************************************************/

void Window::about()
{
	QMessageBox::about(this, tr("About"), QString("<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>")
		.arg(tr("Tetris"), QApplication::applicationVersion(),
			tr("A tetris-eda clone I made for my wife"),
			tr("Copyright &copy; 2008-%1 Graeme Gott.").arg("2012"),
			tr("Released under the <a href=%1>GPL 3</a> license").arg("\"http://www.gnu.org/licenses/gpl.html\""))
	);
}

/*****************************************************************************/

void Window::setLocale()
{
	LocaleDialog dialog;
	dialog.exec();
}

/*****************************************************************************/

void Window::showSettings()
{
	m_settingsDialog->exec();
}

/*****************************************************************************/

void Window::sensorStateChanged(Sensor::State state)
{
	switch (state) {
	case Sensor::Connecting:
		m_sensorStatus->setText("Connecting...");
		break;
	case Sensor::Started:
		m_sensorStatus->setText("Started");
		break;
	case Sensor::Stopped:
		m_sensorStatus->setText("Stopped");
		break;
	default:
		m_sensorStatus->setText("Unknown");
		break;

	}
}

/*****************************************************************************/

void Window::sensorError(const QString &error)
{
	statusBar()->showMessage(error);
}

/*****************************************************************************/

void Window::initSensor()
{
	// Create the sensor and move it to a different thread.
	m_sensorThread = new QThread();
	Sensor *sensor = new Sensor(QSettings().value("SerialPort").toString());
	sensor->moveToThread(m_sensorThread);

	// Create the analyzer and move it to a different thread.
	m_analyzerThread = new QThread();
	Analyzer *analyzer = new Analyzer("/home/estan/testout");
	analyzer->moveToThread(m_analyzerThread);

	// Connect sensor's analyze signal to analyzer's analyze slot.
	connect(sensor, SIGNAL(analyze(QList<SensorReading>)), analyzer, SLOT(analyze(QList<SensorReading>)));

	// Connect analyzer's stressLevelChanged to the board's adjustDifficultyToStressLevel slot.
	qRegisterMetaType<Sensor::State>("Analyzer::StressLevel");
	//connect(analyzer, SIGNAL(stressLevelChanged(Analyzer::StressLevel)), m_board, SLOT(adjustDifficultyToStressLevel(Analyzer::StressLevel)));

	// Connect window to sensor signals.
	qRegisterMetaType<Sensor::State>("Sensor::State");
	qRegisterMetaType<QList<SensorReading> >("QList<SensorReading>");
	connect(sensor, SIGNAL(stateChanged(Sensor::State)), this, SLOT(sensorStateChanged(Sensor::State)));
	connect(sensor, SIGNAL(error(const QString &)), this, SLOT(sensorError(const QString &)));

	// Connect to thread signals to start the sensor and clean up.
	connect(m_sensorThread, SIGNAL(started()), sensor, SLOT(start()));
	connect(m_sensorThread, SIGNAL(finished()), sensor, SLOT(deleteLater()));
	connect(m_sensorThread, SIGNAL(finished()), m_sensorThread, SLOT(deleteLater()));

	// Connect to analyzer signals to start the analyzer and clean up.
	connect(m_analyzerThread, SIGNAL(started()), analyzer, SLOT(start()));
	connect(m_analyzerThread, SIGNAL(finished()), analyzer, SLOT(deleteLater()));
	connect(m_analyzerThread, SIGNAL(finished()), m_analyzerThread, SLOT(deleteLater()));

	// Start the sensor and analyzer threads.
	m_sensorThread->start();
	m_analyzerThread->start();
}

/*****************************************************************************/
