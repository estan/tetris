#include <QFileInfo>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QSettings>

#include "settings_dialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	// Load settings.
	serialPortEdit->setText(QSettings().value("SerialPort", "/dev/rfcomm0").toString());
	monitorPortSpinBox->setValue(QSettings().value("MonitorPort", 6464).toInt());

	checkInputs();
}

void SettingsDialog::accept()
{
	// Save settings.
	QSettings().setValue("SerialPort", serialPortEdit->text());
	QSettings().setValue("MonitorPort", monitorPortSpinBox->value());

	QDialog::accept();
}

void SettingsDialog::on_serialPortEdit_textChanged(const QString &fileName)
{
	Q_UNUSED(fileName)
	checkInputs();
}

void SettingsDialog::checkInputs()
{
	/*
	QFileInfo fileInfo(serialPortEdit->text());
	if (fileInfo.exists()) {
		serialPortEdit->setStyleSheet(styleSheet());
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	} else {
		serialPortEdit->setStyleSheet("QLineEdit { border: 1px solid red; }");
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	}*/
}
