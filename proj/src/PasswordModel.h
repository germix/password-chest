#ifndef PASSWORDMODEL_H
#define PASSWORDMODEL_H

#include <QAbstractItemModel>

enum
{
	COLUMN_SERVICE,
	COLUMN_PASSWORD,
	MAX_COLUMNS,
};

class Password
{
public:
	QString service;
	QString password;
public:
	Password()
	{
	}
	Password(const Password& o)
	{
		operator = (o);
	}
	Password(const QString& serviceIn, const QString& passwordIn)
		: service(serviceIn)
		, password(passwordIn)
	{
	}
public:
	Password& operator = (const Password& o)
	{
		service = o.service;
		password = o.password;
		return *this;
	}
};

class PasswordModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	QVector<Password> passwords;
	bool dirty;
	bool showPasswords;
	QString currentFileName;
public:
	explicit PasswordModel(QObject *parent = nullptr);

	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// Basic functionality:
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public:
	void clear();
	bool load(const QString& fileName);
	bool save(const QString& fileName);
	void addPassword(const QString& service, const QString& password);
	void editPassword(int index, const QString& service, const QString& password);
	void deletePassword(int index);
	Password getPassword(const QModelIndex& index);
	void setShowPasswords(bool show);
	bool isDirty() const
	{
		return dirty;
	}
	void markDirty()
	{
		dirty = true;
		emit onModified();
	}
signals:
	void onModified();
private:
	bool internalLoad(const QString& fileName);
};

#endif // PASSWORDMODEL_H
