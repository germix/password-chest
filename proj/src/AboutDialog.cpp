#include "AboutDialog.h"
#include "ui_AboutDialog.h"
#include <QFileInfo>
#include <QDateTime>

AboutDialog::AboutDialog(QWidget* parent)
: QDialog(parent)
, ui(new Ui::AboutDialog)
{
	ui->setupUi(this);

	{
		QString s;
		QFileInfo info(qApp->applicationFilePath());

		s += tr("Time") + ": ";
		s += info.lastModified().time().toString();
		s += "\n";
		s += tr("Date") + ": ";
		s += info.lastModified().date().toString();
		s += "\n";
		s += tr("Version") + ": ";
		s += QString::number(MAJOR_VERSION) + "." + QString::number(MINOR_VERSION);
		s += "\n";
		s += tr("Developer") + ": Germán Martínez";
		ui->label->setText(s);
	}
	{
		QSize s = sizeHint();
		setMinimumSize(s);
		setMaximumSize(s);
		setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
	}
}

AboutDialog::~AboutDialog()
{
	delete ui;
}
