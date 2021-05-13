#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class PasswordModel;
class RecentFilesMenu;

#include "Languages.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
	Ui::MainWindow* ui;
	PasswordModel* passwordModel;
	QString lastDirectory;
	QString lastFileName;

	Languages languages;
	RecentFilesMenu* recentFiles;

public:
	explicit MainWindow(QWidget* parent = 0);
	~MainWindow();
private:
	void initRecentFilesMenu(const QByteArray& state);
	void newFile();
	void openFile(const QString& fileName);
	bool saveFile(const QString& fileName = QString());
	bool closeFile();
	void editPassword(const QModelIndex& index);
	void updateTitle();
	void updateActions();
private:
	void changeEvent(QEvent* e) override;
	void closeEvent(QCloseEvent* e) override;
	void dropEvent(QDropEvent* e) override;
	void dragEnterEvent(QDragEnterEvent* e) override;
private slots:
	void slotAction();
	void slotRecentFiles_fileTriggered(const QString& fileName);
	void slotPasswordModel_modified();
	void slotTreeView_doubleClicked(const QModelIndex& index);
	void slotTreeViewSelectionModel_currentChanged(const QModelIndex& current, const QModelIndex& previous);
};

#endif // MAINWINDOW_H
