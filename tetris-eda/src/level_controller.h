#ifndef LEVEL_CONTROLLER_H
#define LEVEL_CONTROLLER_H

#include <QObject>

#include "board.h"

/**
 * Base class for difficulty level controllers.
 *
 * Level controllers inherit this class and emit the changeDifficultyLevel
 * signal when they wish the change the difficulty level of the game. The
 * Board class is configured with a specific level controller, and will
 * react to the changeDifficultyLevel signal.
 *
 * At the moment there are two subclasses, LevelControllerEDA and
 * LevelControllerPerformance, each implementing their own algorithm for
 * changing the difficulty level.
 */
class LevelController : public QObject
{
	Q_OBJECT

public:
	explicit LevelController(QObject *parent = 0);
	
signals:
	/**
	 * Request a change in the level of difficulty.
	 *
	 * Subclasses should emit this signal when they want to effect
	 * a change in the level of difficulty of the game.
	 *
	 * @param level The requested difficulty level.
	 */
	void changeDifficultyLevel(Board::DifficultyLevel level);
};

#endif // LEVEL_CONTROLLER_H
