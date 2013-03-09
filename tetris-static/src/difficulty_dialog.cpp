#include <QSettings>
#include <QComboBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>

#include "difficulty_dialog.h"

DifficultyDialog::DifficultyDialog(QWidget *parent) :
	QDialog(parent, Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
	setWindowTitle("Difficulty Level");

	QLabel* text = new QLabel("Select difficulty level:", this);

	m_difficulties = new QComboBox(this);
	m_difficulties->addItem("Easy", Board::Easy);
	m_difficulties->addItem("Medium", Board::Medium);
	m_difficulties->addItem("Hard", Board::Hard);
	m_difficulties->setCurrentIndex(
				m_difficulties->findData(QVariant::fromValue(QSettings().value("Difficulty"))));

	QDialogButtonBox* buttons = new QDialogButtonBox(
				QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSizeConstraint(QLayout::SetFixedSize);
	layout->addWidget(text);
	layout->addWidget(m_difficulties);
	layout->addWidget(buttons);
}

Board::Difficulty DifficultyDialog::difficulty()
{
	return static_cast<Board::Difficulty>(
				m_difficulties->itemData(m_difficulties->currentIndex()).toInt());
}
