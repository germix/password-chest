#include "PasswordDialog.h"
#include "ui_PasswordDialog.h"
#include "PasswordModel.h"

PasswordDialog::PasswordDialog()
	: QDialog()
	, ui(new Ui::PasswordDialog)
{
	ui->setupUi(this);

	setWindowTitle(tr("Add password"));
	connect(ui->txtService, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
	connect(ui->txtUsername, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
	connect(ui->txtPassword, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));

	checkOkEnabled();
}

PasswordDialog::PasswordDialog(const Password& psw)
	: QDialog()
	, ui(new Ui::PasswordDialog)
{
	ui->setupUi(this);

	setWindowTitle(tr("Edit password"));
	connect(ui->txtService, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
	connect(ui->txtUsername, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
	connect(ui->txtPassword, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));

	ui->txtService->setText(psw.service);
	ui->txtUsername->setText(psw.username);
	ui->txtPassword->setText(psw.password);

	checkOkEnabled();
}

PasswordDialog::~PasswordDialog()
{
	delete ui;
}

QString PasswordDialog::getService()
{
	return ui->txtService->text();
}

QString PasswordDialog::getUsername()
{
	return ui->txtUsername->text();
}

QString PasswordDialog::getPassword()
{
	return ui->txtPassword->text();
}

void PasswordDialog::checkOkEnabled()
{
	ui->btnOk->setEnabled(
		   !ui->txtService->text().isEmpty()
		&& !ui->txtUsername->text().isEmpty()
		&& !ui->txtPassword->text().isEmpty());
}

void PasswordDialog::slotTextChanged(const QString& text)
{
	checkOkEnabled();
}
