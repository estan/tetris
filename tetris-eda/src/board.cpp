/***********************************************************************
 *
 * Copyright (C) 2007, 2008, 2010, 2012 Graeme Gott <graeme@gottcode.org>
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

#include "board.h"

#include "piece.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QDebug>

/*****************************************************************************/

static QColor colors[] = {
	QColor("#0057ae"),
	QColor("#006e29"),
	QColor("#9c0f0f"),
	QColor("#539ae3"),
	QColor("#8f6b32"),
	QColor("#ec7331"),
	QColor("#644a9b")
};

/*****************************************************************************/

Board::Board(bool adjustToPerformance, QWidget* parent)
:	QWidget(parent),
	m_removed_lines(0),
	m_level(1),
	m_score(0),
	m_piece(0),
	m_next_piece((rand() % 7) + 1),
	m_flash_frame(-1),
	m_piece_size(0),
	m_started(false),
	m_done(false),
	m_paused(false),
	m_difficultyLevel(FirstLevel),
	m_adjustToPerformance(adjustToPerformance),
	m_topCellY(19),
	m_shiftTime(1000),
	m_firstLevelShiftTime(1000),
	m_secondLevelShiftTime(500),
	m_thirdLevelShiftTime(250)
{
	setMinimumSize(201, 401);
	setFocusPolicy(Qt::StrongFocus);
	setFocus();

	m_shift_timer = new QTimer(this);
	m_shift_timer->setInterval(m_shiftTime);
	m_shift_timer->setSingleShot(true);
	connect(m_shift_timer, SIGNAL(timeout()), this, SLOT(shiftPiece()));

	m_flash_timer = new QTimer(this);
	m_flash_timer->setInterval(80);
	connect(m_flash_timer, SIGNAL(timeout()), this, SLOT(flashLines()));

	for (int i = 0; i < 4; ++i) {
		m_full_lines[i] = -1;
	}

	for (int col = 0; col < 10; ++col) {
		for (int row = 0; row < 20; ++row) {
			m_cells[col][row] = 0;
		}
	}
}

/*****************************************************************************/

bool Board::endGame()
{
	if (m_done || !m_started) {
		return true;
	}

	if (QMessageBox::question(this, tr("Question"), tr("End the current game?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
		gameOver();
		return true;
	} else {
		return false;
	}
}

/*****************************************************************************/

void Board::findFullLines()
{
	// Empty list of full lines
	for (int i = 0; i < 4; ++i)
		m_full_lines[i] = -1;
	int pos = 0;

	// Find full lines
	bool full = false;
	for (int row = 0; row < 20; ++row) {
		full = true;
		for (int col = 0; col < 10; ++col) {
			if (m_cells[col][row] == 0)
				full = false;
		}
		if (full) {
			m_full_lines[pos] = row;
			++pos;
		}
	}
}

/*****************************************************************************/

Board::DifficultyLevel Board::difficultyLevel() const
{
	return m_difficultyLevel;
}

/*****************************************************************************/

void Board::increaseDifficultyLevel()
{
	if (m_difficultyLevel == ThirdLevel)
		return; // Already on highest level.

	DifficultyLevel newLevel = m_difficultyLevel;

	if (m_difficultyLevel == FirstLevel)
		newLevel = SecondLevel;
	else if (m_difficultyLevel == SecondLevel)
		newLevel = ThirdLevel;
	else {
		qWarning("Unknown difficulty level");
		return;
	}

	setDifficultyLevel(newLevel);
}

/*****************************************************************************/

void Board::decreaseDifficultyLevel()
{
	if (m_difficultyLevel == FirstLevel)
		return; // Already on lowest level.

	DifficultyLevel newLevel = m_difficultyLevel;

	if (m_difficultyLevel == ThirdLevel)
		newLevel = SecondLevel;
	else if (m_difficultyLevel == SecondLevel)
		newLevel = FirstLevel;
	else {
		qWarning("Unknown difficulty level");
		return;
	}

	setDifficultyLevel(newLevel);
}

/*****************************************************************************/

void Board::setDifficultyLevel(DifficultyLevel level)
{
	if (level == m_difficultyLevel)
		return;

	switch (level) {
	case FirstLevel:
		m_shiftTime = m_firstLevelShiftTime;
		break;
	case SecondLevel:
		m_shiftTime = m_secondLevelShiftTime;
		break;
	case ThirdLevel:
		m_shiftTime = m_thirdLevelShiftTime;
		break;
	default:
		qWarning("Unknown difficulty level");
		return;
	}
	qDebug() << "New difficulty: " << level;
	m_difficultyLevel = level;
	m_shift_timer->setInterval(m_shiftTime);
	m_shift_timer->setSingleShot(true);

	emit difficultyLevelChanged(m_difficultyLevel);
}

/*****************************************************************************/

void Board::newGame()
{
	if (!endGame()) {
		return;
	}

	m_flash_timer->stop();
	m_shift_timer->stop();
	delete m_piece;
	m_piece = 0;

	emit hideMessage();
	m_started = true;
	m_done = false;
	m_paused = false;
	m_removed_lines = 0;
	m_level = 1;
	m_score = 0;
	m_shift_timer->setInterval(m_shiftTime);
	m_next_piece = (rand() % 7) + 1;
	m_topCellY = 19;
	m_difficultyLevel = FirstLevel;
	m_shiftTime = m_firstLevelShiftTime;

	for (int i = 0; i < 4; ++i)
		m_full_lines[i] = -1;

	for (int col = 0; col < 10; ++col) {
		for (int row = 0; row < 20; ++row) {
			m_cells[col][row] = 0;
		}
	}

	emit pauseAvailable(true);
	emit levelUpdated(m_level);
	emit linesRemovedUpdated(m_removed_lines);
	emit scoreUpdated(m_score);
	emit gameStarted();

	setCursor(Qt::BlankCursor);
	createPiece();
}

/*****************************************************************************/

void Board::pauseGame()
{
	m_paused = true;
	if (m_flash_frame > -1) {
		m_flash_timer->stop();
	} else {
		m_shift_timer->stop();
	}

	update();
	emit showMessage(tr("<big><b>Paused</b></big><br>Click to resume playing."));

	unsetCursor();
	emit pauseAvailable(false);
}

/*****************************************************************************/

void Board::resumeGame()
{
	m_paused = false;
	if (m_flash_frame > -1) {
		m_flash_timer->start();
	} else {
		m_shift_timer->start();
	}

	emit hideMessage();
	update();

	setCursor(Qt::BlankCursor);
	emit pauseAvailable(true);
}

/*****************************************************************************/

void Board::keyPressEvent(QKeyEvent* event)
{
	if (!m_piece || m_paused)
		return;

	switch (event->key()) {
	case Qt::Key_Left:
		m_piece->moveLeft();
		break;
	case Qt::Key_Right:
		m_piece->moveRight();
		break;
	case Qt::Key_Up:
		m_piece->rotate();
		break;
	case Qt::Key_Down:
		m_piece->drop();
		landPiece();
		break;
	default:
		break;
	}

	update();
}

/*****************************************************************************/

void Board::mousePressEvent(QMouseEvent* event)
{
	if (m_paused) {
		resumeGame();
	} else if (m_done || !m_started) {
		newGame();
	}
	QWidget::mousePressEvent(event);
}

/*****************************************************************************/

void Board::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.fillRect(m_background, Qt::black);

	if (m_paused) {
		return;
	}

	// Draw board
	for (int col = 0; col < 10; ++col) {
		for (int row = 0; row < 20; ++row) {
			int cell = m_cells[col][row] - 1;
			if (cell >= 0) {
				painter.drawPixmap(col * m_piece_size + m_background.x(), row * m_piece_size + m_background.y(), m_images[cell]);
			}
		}
	}

	// Draw piece
	if (m_piece) {
		int type = m_piece->type() - 1;
		const Cell* cells = m_piece->cells();
		for (int i = 0; i < 4; ++i) {
			painter.drawPixmap(cells[i].x * m_piece_size + m_background.x(), cells[i].y * m_piece_size + m_background.y(), m_images[type]);
		}
	}
}

/*****************************************************************************/

void Board::focusOutEvent(QFocusEvent*)
{
	if (m_piece && !m_done && !m_paused)
		pauseGame();
}

/*****************************************************************************/

void Board::resizeEvent(QResizeEvent* event)
{
	m_piece_size = qMin(event->size().width() / 10, event->size().height() / 20);
	int w = m_piece_size * 10 + 1;
	int h = m_piece_size * 20 + 1;
	m_background = QRect((width() - w) / 2, (height() - h) / 2, w, h);

	QPainter painter;

	for (int i = 0; i < 7; ++i) {
		QPixmap pixmap(m_piece_size + 1, m_piece_size + 1);
		pixmap.fill(QColor(0, 0, 0, 0));

		painter.begin(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setBrush(colors[i]);
		painter.setPen(QPen(colors[i].lighter(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		painter.drawRoundRect(1, 1, m_piece_size - 1, m_piece_size - 1);
		painter.end();

		m_images[i] = pixmap;
	}
}

/*****************************************************************************/

void Board::shiftPiece()
{
	Q_ASSERT(m_piece != 0);

	if (m_piece->moveDown()) {
		update();
		m_shift_timer->start();
	} else {
		landPiece();
	}
}

/*****************************************************************************/

void Board::flashLines()
{
	m_flash_frame++;
	if (m_flash_frame < 6) {
		int amount = (m_flash_frame % 2) ? 7 : -7;

		for (int i = 0; i < 4; ++i) {
			int row = m_full_lines[i];
			if (row == -1) {
				break;
			}

			for (int col = 0; col < 10; ++col) {
				m_cells[col][row] += amount;
			}
		}

		update();
	} else {
		m_flash_timer->stop();
		m_flash_frame = -1;
		removeLines();
	}
}

/*****************************************************************************/

void Board::removeLines()
{
	int score = 14 * m_level;

	// Loop through full lines
	for (int i = 0; i < 4; ++i) {
		int row = m_full_lines[i];
		if (row == -1)
			break;

		// Remove line
		for (int col = 0; col < 10; ++col) {
			removeCell(col, row);
		}
		++m_removed_lines;
		++m_topCellY;
		score *= 3;

		// Shift board down
		for (; row > 0; --row) {
			for (int col = 0; col < 10; ++col) {
				m_cells[col][row] = m_cells[col][row - 1];
			}
		}
	}

	// Remove top line
	if (m_full_lines[0] != -1) {
		for (int col = 0; col < 10; ++col) {
			removeCell(col, 0);
		}
	}

	m_level = (m_removed_lines / 10) + 1;
	//m_shift_timer->setInterval(10000 / (m_removed_lines + 20));
	m_score += score;
	emit levelUpdated(m_level);
	emit linesRemovedUpdated(m_removed_lines);
	emit scoreUpdated(m_score);

	// Empty list of full lines
	for (int i = 0; i < 4; ++i)
		m_full_lines[i] = -1;

	// Add new piece
	createPiece();
}

/*****************************************************************************/

void Board::gameOver()
{
	delete m_piece;
	m_piece = 0;
	m_done = true;
	unsetCursor();
	emit showMessage(tr("<big><b>Game Over!</b></big><br>Click to start a new game."));
	emit gameOver(m_level, m_removed_lines, m_score);
}

/*****************************************************************************/

void Board::addCell(int x, int y, int type)
{
	Q_ASSERT(x >= 0 && x < 10);
	Q_ASSERT(y >= 0 && y < 20);
	Q_ASSERT(type > 0 && type < 8);
	Q_ASSERT(m_cells[x][y] == 0);

	m_cells[x][y] = type;
}

/*****************************************************************************/

void Board::removeCell(int x, int y)
{
	Q_ASSERT(x >= 0 && x < 10);
	Q_ASSERT(y >= 0 && y < 20);

	m_cells[x][y] = 0;
}

/*****************************************************************************/

void Board::createPiece()
{
	Q_ASSERT(m_piece == 0);

	updateDifficultyFromPerformance();

	m_piece = new Piece(m_next_piece, this);
	if (m_piece->isValid()) {
		m_next_piece = (rand() % 7) + 1;
		emit nextPieceAvailable(renderPiece(m_next_piece));
		m_shift_timer->start();
	} else {
		gameOver();
	}
	update();
}

/*****************************************************************************/

void Board::landPiece()
{
	m_shift_timer->stop();

	int type = m_piece->type();
	int top_cell_y = 19; // Y-coordinate of top-most cell.
	const Cell* cells = m_piece->cells();
	for (int i = 0; i < 4; ++i) {
		addCell(cells[i].x, cells[i].y, type);
		top_cell_y = qMin(top_cell_y, cells[i].y);
	}
	delete m_piece;
	m_piece = 0;

	findFullLines();
	if (m_full_lines[0] != -1) {
		m_flash_timer->start();
	} else {
		if (top_cell_y < m_topCellY) {
			// Height of stack increased.
			m_topCellY = top_cell_y;
		}
		createPiece();
	}
}

/*****************************************************************************/

QPixmap Board::renderPiece(int type) const
{
	Q_ASSERT(type > 0 && type < 8);

	Cell piece[4];
	Piece::cells(piece, type);

	QPixmap result(80, 100);
	result.fill(Qt::black);
	{
		QPainter painter(&result);
		painter.setRenderHint(QPainter::Antialiasing, true);
		if (type == 1) {
			painter.translate(30, 10);
		} else if (type < 7) {
			painter.translate(20, 20);
		} else {
			painter.translate(20, 30);
		}

		for (int i = 0; i < 4; ++i) {
			painter.setBrush(colors[type - 1]);
			painter.setPen(QPen(colors[type - 1].lighter(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
			painter.drawRoundRect(piece[i].x * 20, piece[i].y * 20, 19, 19);
		}
	}

	return result;
}

/*****************************************************************************/

void Board::updateDifficultyFromPerformance()
{
	if (!m_adjustToPerformance)
		return; // Not enabled.

	if (m_topCellY < 8) {
		setDifficultyLevel(FirstLevel);
		// increaseDifficultyLevel();
	} else if (m_topCellY > 13) {
		setDifficultyLevel(ThirdLevel);
		// decreaseDifficultyLevel();
	} else {
		setDifficultyLevel(SecondLevel);
	}
}

/*****************************************************************************/
