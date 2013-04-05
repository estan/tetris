/***********************************************************************
 *
 * Copyright (C) 2007, 2008, 2010 Graeme Gott <graeme@gottcode.org>
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

#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include "analyzer.h"

class QPixmap;
class QTimer;
class Piece;

class Board : public QWidget
{
	Q_OBJECT

public:
	/**
	 * This enum describes a set of difficulty levels.
	 */
	enum DifficultyLevel {
		FirstLevel,
		SecondLevel,
		ThirdLevel
	};

	Board(bool adjustToPerformance, QWidget* parent = 0);

	bool cell(int x, int y) const;
	bool endGame();
	void findFullLines();

	/**
	 * Returns the current difficulty level of the game.
	 *
	 * @return The current difficulty level.
	 */
	DifficultyLevel difficultyLevel() const;

public slots:
	void newGame();
	void pauseGame();
	void resumeGame();

	/**
	 * Increases the difficulty level one step, if possible.
	 */
	void increaseDifficultyLevel();

	/**
	 * Decreases the difficulty level one step, if possible.
	 */
	void decreaseDifficultyLevel();

	/**
	 * Sets the difficulty level of the game.
	 *
	 * @param level The new difficulty level.
	 */
	void setDifficultyLevel(DifficultyLevel level);

	/**
	 * Adjusts the game difficulty level to stress level.
	 *
	 * @param level The stress level to adjust to.
	 */
	void adjustDifficultyToStressLevel(Analyzer::StressLevel level);

signals:
	void pauseAvailable(bool available);
	void nextPieceAvailable(QPixmap piece);
	void levelUpdated(int level);
	void linesRemovedUpdated(int lines);
	void scoreUpdated(int score);
	void gameOver(int level, int lines, int score);
	void gameStarted();
	void hideMessage();
	void showMessage(const QString& message);

	/**
	 * Emitted when the difficulty level of the game changes.
	 *
	 * @param level The new difficulty level.
	 */
	void difficultyLevelChanged(DifficultyLevel level);

protected:
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void paintEvent(QPaintEvent*);
	virtual void focusOutEvent(QFocusEvent* event);
	virtual void resizeEvent(QResizeEvent* event);

private slots:
	void shiftPiece();
	void flashLines();
	void removeLines();

private:
	void gameOver();
	void addCell(int x, int y, int type);
	void removeCell(int x, int y);
	void createPiece();
	void landPiece();
	QPixmap renderPiece(int type) const;
	void updateDifficultyFromPerformance();

private:
	QPixmap m_images[7];
	int m_cells[10][20];
	int m_full_lines[4];
	int m_removed_lines;
	int m_level;
	int m_score;
	Piece* m_piece;
	int m_next_piece;
	QTimer* m_shift_timer;
	QTimer* m_flash_timer;
	int m_flash_frame;
	int m_piece_size;
	QRect m_background;
	bool m_started;
	bool m_done;
	bool m_paused;

	DifficultyLevel m_difficultyLevel; // Difficulty level.
	bool m_adjustToPerformance;        // Adjust difficulty based on performance?
	int m_topCellY;                    // Y-coordinate of top-most landed cell.
	int m_shiftTime;                   // Time to wait before shifting the piece.
	bool m_isFirstPiece;               // True if the player is playing the first piece.

	// Time to wait before shifting the piece for each difficulty level.
	int m_firstLevelShiftTime;
	int m_secondLevelShiftTime;
	int m_thirdLevelShiftTime;
};

inline bool Board::cell(int x, int y) const
{
	Q_ASSERT(x >= 0 && x < 10);
	Q_ASSERT(y >= 0 && y < 20);
	return m_cells[x][y] != 0;
}

#endif // BOARD_H
