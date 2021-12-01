#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "PasswordModel.h"
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QMimeData>

#include "AboutDialog.h"
#include "PasswordDialog.h"
#include "RecentFilesMenu.h"

#define TITLE "PasswordChest"

#define SETTINGS_APPLICATION "PasswordChest"
#define SETTINGS_ORGANIZATION "Germix"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	passwordModel = new PasswordModel();
	ui->treeView->setModel(passwordModel);

	ui->menu_View->addAction(ui->mainToolBar->toggleViewAction());

	connect(passwordModel, SIGNAL(onModified()), this, SLOT(slotPasswordModel_modified()));
	connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotTreeView_doubleClicked(QModelIndex)));
	connect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(slotTreeViewSelectionModel_currentChanged(QModelIndex,QModelIndex)));

	//
	// Load settings
	//
	QSettings s(SETTINGS_ORGANIZATION, SETTINGS_APPLICATION);

	initRecentFilesMenu(s.value("RecentFiles").toByteArray());
	languages.init(ui->menu_Languages, "translations", "password_chest", s.value("Language").toString());

	restoreGeometry(s.value("WindowGeometry").toByteArray());
	restoreState(s.value("WindowState").toByteArray());
	ui->treeView->header()->restoreState(s.value("TreeViewHeader").toByteArray());

	openFile(s.value("LastestFile", "").toString());
}

MainWindow::~MainWindow()
{
	//
	// Save settings
	//
	QSettings s(SETTINGS_ORGANIZATION, SETTINGS_APPLICATION);
	s.setValue("WindowState", saveState());
	s.setValue("WindowGeometry", saveGeometry());
	s.setValue("LastestFile", lastFileName);
	s.setValue("Language", languages.language());
	s.setValue("RecentFiles", recentFiles->saveState());
	s.setValue("TreeViewHeader", ui->treeView->header()->saveState());

	// ...
	delete ui;
}

void MainWindow::initRecentFilesMenu(const QByteArray& state)
{
	recentFiles = new RecentFilesMenu(20, 10, tr("Recent files"), QIcon(":/icons/clear.png"));

	ui->menu_File->insertMenu(ui->actionExit, recentFiles);
	ui->menu_File->insertSeparator(ui->actionExit);

	connect(recentFiles, SIGNAL(onFileTriggered(QString)), this, SLOT(slotRecentFiles_fileTriggered(QString)));

	recentFiles->restoreState(state);
}

void MainWindow::newFile()
{
	closeFile();
}

void MainWindow::openFile(const QString& fileName)
{
	if(closeFile())
	{
		if(passwordModel->load(fileName))
		{
			recentFiles->removeFile(passwordModel->currentFileName);
			lastDirectory = QFileInfo(passwordModel->currentFileName).absolutePath();
		}
	}
}

bool MainWindow::saveFile(const QString& fileName)
{
	QString fname;

	fname = fileName;
	if(fname.isEmpty())
	{
		fname = QFileDialog::getSaveFileName(
					NULL,
					tr("Save file"),
					lastDirectory,
					tr("Password Chest files") + " (*.pswc);;");

		if(!fname.isNull())
		{
			recentFiles->addFile(passwordModel->currentFileName);
		}
	}
	if(!fname.isNull())
	{
		if(passwordModel->save(fname))
		{
			lastDirectory = QFileInfo(passwordModel->currentFileName).absolutePath();

			return true;
		}
	}
	return false;
}

bool MainWindow::closeFile()
{
	if(passwordModel->isDirty())
	{
		QString s;
		if(passwordModel->currentFileName.isEmpty())
			s = tr("Untitled");
		else
			s = QFileInfo(passwordModel->currentFileName).fileName();

		switch(QMessageBox::question(this,
									 tr("Save"),
									 tr("Save file \"%1\"?").arg(s),
									 QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
		{
			case QMessageBox::Yes:
				if(!saveFile(passwordModel->currentFileName))
				{
					return false;
				}
				break;
			case QMessageBox::Cancel:
				return false;
			default:
			case QMessageBox::No:
				break;
		}
	}
	if(!passwordModel->currentFileName.isEmpty())
	{
		recentFiles->addFile(passwordModel->currentFileName);
	}
	lastFileName = passwordModel->currentFileName;
	passwordModel->clear();

	return true;
}

void MainWindow::editPassword(const QModelIndex& index)
{
	Password psw = passwordModel->getPassword(index);
	PasswordDialog dlg(psw);

	if(dlg.exec() == PasswordDialog::Accepted)
	{
		passwordModel->editPassword(index.row(), dlg.getService(), dlg.getUsername(), dlg.getPassword());
	}
}

void MainWindow::updateTitle()
{
	QString s = TITLE;

	s += " - ";
	if(passwordModel->currentFileName.isEmpty())
		s += tr("Untitled");
	else
		s += QFileInfo(passwordModel->currentFileName).fileName();

	if(passwordModel->isDirty())
	{
		s += " [*]";
	}
	setWindowTitle(s);
	setWindowModified(passwordModel->isDirty());
}

void MainWindow::updateActions()
{
	QModelIndex index = ui->treeView->currentIndex();

	if(index.isValid())
	{
		ui->actionEditPassword->setEnabled(true);
		ui->actionDeletePassword->setEnabled(true);
	}
	else
	{
		ui->actionEditPassword->setEnabled(false);
		ui->actionDeletePassword->setEnabled(false);
	}
}

void MainWindow::changeEvent(QEvent* e)
{
	if(e != NULL)
	{
		switch(e->type())
		{
#if 1
			case QEvent::LocaleChange:
				{
					QString locale = QLocale::system().name();
					locale.truncate(locale.lastIndexOf('_'));
					languages.load(locale);
				}
				break;
#endif
			case QEvent::LanguageChange:
				ui->retranslateUi(this);
				recentFiles->rebuild(tr("Recent files"));
				updateTitle();
				break;
			default:
				break;
		}
	}
	QMainWindow::changeEvent(e);
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	if(closeFile())
		e->accept();
	else
		e->ignore();
}

void MainWindow::dropEvent(QDropEvent* e)
{
	QString s;
	const QList<QUrl> urls = e->mimeData()->urls();

	// ...
	e->acceptProposedAction();
	for(int i = 0; i < urls.size(); i++)
	{
		s = urls.at(i).toLocalFile();
		if(!s.isEmpty())
		{
			QFileInfo fi(s);
			if(fi.suffix().toLower() == "pswc")
			{
				openFile(s);
			}
		}
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent* e)
{
	if(e->mimeData()->hasFormat("text/uri-list"))
	{
		e->acceptProposedAction();
	}
}

void MainWindow::slotAction()
{
	QAction* action = qobject_cast<QAction*>(sender());

	if(action == ui->actionNewFile)
	{
		newFile();
	}
	else if(action == ui->actionOpenFile)
	{
		QString filters;

		filters += tr("Password Chest files") + " (*.pswc);;";

		QString fileName = QFileDialog::getOpenFileName(
					this,
					tr("Open file"),
					lastDirectory,
					filters);
		if(!fileName.isEmpty())
		{
			openFile(fileName);
		}
	}
	else if(action == ui->actionSaveFile)
	{
		saveFile(passwordModel->currentFileName);
	}
	else if(action == ui->actionSaveFileAs)
	{
		saveFile();
	}
	else if(action == ui->actionExit)
	{
		close();
	}
	else if(action == ui->actionAddPassword)
	{
		PasswordDialog dlg;

		if(dlg.exec() == PasswordDialog::Accepted)
		{
			passwordModel->addPassword(dlg.getService(), dlg.getUsername(), dlg.getPassword());
		}
	}
	else if(action == ui->actionEditPassword)
	{
		editPassword(ui->treeView->currentIndex());
	}
	else if(action == ui->actionDeletePassword)
	{
		passwordModel->deletePassword(ui->treeView->currentIndex().row());
	}
	else if(action == ui->actionViewUsernames)
	{
		passwordModel->setShowUsernames(action->isChecked());
		ui->treeView->viewport()->update();
	}
	else if(action == ui->actionViewPasswords)
	{
		passwordModel->setShowPasswords(action->isChecked());
		ui->treeView->viewport()->update();
	}
	else if(action == ui->actionAbout)
	{
		AboutDialog().exec();
	}
}

void MainWindow::slotRecentFiles_fileTriggered(const QString& fileName)
{
	openFile(fileName);
}

void MainWindow::slotPasswordModel_modified()
{
	updateTitle();

	updateActions();
}

void MainWindow::slotTreeView_doubleClicked(const QModelIndex& index)
{
	editPassword(index);
}

void MainWindow::slotTreeViewSelectionModel_currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	updateActions();
}
