/***********************************************************************
 *
 * Copyright (C) 2013 Elvis Stansvik <stansvik@kth.se>
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

#ifndef DIFFICULTY_DIALOG_H
#define DIFFICULTY_DIALOG_H

#include <QDialog>

#include "board.h"

class QComboBox;

/**
 * Dialog to set difficulty level.
 *
 * This class handles setting the game difficulty level.
 */
class DifficultyDialog : public QDialog
{
	Q_OBJECT

public:
	/**
	 * Construct a dialog to choose difficulty level.
	 *
	 * @param parent the parent widget of the dialog
	 */
	DifficultyDialog(QWidget* parent = 0);

	/**
	 * Return the picked difficulty.
	 *
	 * @return picked difficulty
	 */
	Board::Difficulty difficulty();

private:
	QComboBox* m_difficulties; /**< list of difficulties */
};

#endif // DIFFICULTY_DIALOG_H
