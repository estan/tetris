#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include "ui_settings_dialog.h"

class SettingsDialog : public QDialog, private Ui::SettingsDialog
{
	Q_OBJECT
	
public:
	explicit SettingsDialog(QWidget *parent = 0);

public slots:
	void accept();

private slots:
	void on_serialPortEdit_textChanged(const QString &fileName);

private:
	void checkInputs();
};

#endif // SETTINGS_DIALOG_H
