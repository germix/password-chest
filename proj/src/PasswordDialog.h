#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class PasswordDialog;
}

class Password;

class PasswordDialog : public QDialog
{
	Q_OBJECT
	Ui::PasswordDialog* ui;
public:
	PasswordDialog();
	PasswordDialog(const Password& psw);
	~PasswordDialog();
public:
	QString getService();
	QString getUsername();
	QString getPassword();
private:
	void checkOkEnabled();
private slots:
	void slotTextChanged(const QString& text);
};

#endif // PASSWORDDIALOG_H
