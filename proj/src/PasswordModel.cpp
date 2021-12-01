#include "PasswordModel.h"
#include <QFile>
#include <QFileInfo>

#include <pshpack1.h>
#define MAGIC "PSWCHEST"
#define MAGIC_LEN 8
#define VERSION 2

struct HEADER
{
	char	magic[MAGIC_LEN];
	quint32	version;
	quint32	count;
};
#include <poppack.h>

void writeString(QFile& file, const QString& text)
{
	QByteArray utf8 = text.toUtf8();
	int len = utf8.size();

	file.write((char*)&len, sizeof(len));
	file.write(utf8);
}

QString readString(QFile& file)
{
	int len;
	QByteArray utf8;

	file.read((char*)&len, sizeof(len));
	utf8 = file.read(len);

	return QString::fromUtf8(utf8);
}

PasswordModel::PasswordModel(QObject *parent)
	: QAbstractItemModel(parent)
	, dirty(false)
	, showUsernames(false)
	, showPasswords(false)
{
}

QVariant PasswordModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::DisplayRole)
	{
		switch(section)
		{
			case COLUMN_SERVICE:
				return tr("Service");
			case COLUMN_USERNAME:
				return tr("Username");
			case COLUMN_PASSWORD:
				return tr("Password");
		}
	}
	return QVariant();
}

QModelIndex PasswordModel::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row, column);
}

QModelIndex PasswordModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int PasswordModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return passwords.size();
}

int PasswordModel::columnCount(const QModelIndex &parent) const
{
	return MAX_COLUMNS;
}

QVariant PasswordModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if(role == Qt::DisplayRole)
	{
		const Password& psw = passwords.at(index.row());

		switch(index.column())
		{
			case COLUMN_SERVICE:
				return psw.service;
			case COLUMN_USERNAME:
				if(showUsernames)
					return psw.username;
				return QString().fill('*', psw.username.size());
			case COLUMN_PASSWORD:
				if(showPasswords)
					return psw.password;

				return QString().fill('*', psw.password.size());
		}
	}
	return QVariant();
}

void PasswordModel::clear()
{
	beginResetModel();
	passwords.clear();
	dirty = false;
	currentFileName.clear();
	emit onModified();
	endResetModel();
}

bool PasswordModel::load(const QString& fileName)
{
	beginResetModel();
	bool ret = internalLoad(fileName);
	endResetModel();

	if(ret)
	{
		dirty = false;
		currentFileName = QFileInfo(fileName).absoluteFilePath();
		emit onModified();
	}

	return ret;
}

bool PasswordModel::save(const QString& fileName)
{
	QFile file;
	HEADER hdr;

	memcpy(hdr.magic, MAGIC, MAGIC_LEN);
	hdr.version = VERSION;
	hdr.count = passwords.count();

	file.setFileName(fileName);
	if(file.open(QFile::WriteOnly))
	{
		// Write header
		file.write((char*)&hdr, sizeof(hdr));

		// Write passwords
		for(int i = 0; i < passwords.size(); i++)
		{
			Password& psw = passwords[i];

			writeString(file, psw.service);
			writeString(file, psw.username);
			writeString(file, psw.password);
		}

		// ...
		dirty = false;
		currentFileName = QFileInfo(fileName).absoluteFilePath();
		emit onModified();

		return true;
	}
	return false;
}

void PasswordModel::addPassword(const QString& service, const QString& username, const QString& password)
{
	beginInsertRows(QModelIndex(), passwords.size(), passwords.size());
	passwords.append(Password(service, username, password));
	endInsertRows();

	markDirty();
}

void PasswordModel::editPassword(int index, const QString& service, const QString& username, const QString& password)
{
	passwords[index] = Password(service, username, password);
	dataChanged(createIndex(index, 0), createIndex(index, 0));

	markDirty();
}

void PasswordModel::deletePassword(int index)
{
	beginRemoveRows(QModelIndex(), index, index);
	passwords.removeAt(index);
	endRemoveRows();

	markDirty();
}

Password PasswordModel::getPassword(const QModelIndex& index)
{
	return passwords[index.row()];
}

void PasswordModel::setShowUsernames(bool show)
{
	showUsernames = show;
}

void PasswordModel::setShowPasswords(bool show)
{
	showPasswords = show;
}

bool PasswordModel::internalLoad(const QString& fileName)
{
	QFile file;

	passwords.clear();

	file.setFileName(fileName);
	if(file.open(QFile::ReadOnly))
	{
		uint len;
		HEADER hdr;

		// Write header
		len = sizeof(hdr);
		file.read((char*)&hdr, len);

		// Check valid header
		if(len != sizeof(hdr))
		{
			return false;
		}
		if(0 != memcmp(hdr.magic, MAGIC, MAGIC_LEN))
		{
			return false;
		}
		if(hdr.version == 1)
		{
			// Read passwords
			while(!file.atEnd())
			{
				QString service = readString(file);
				QString password = readString(file);

				passwords.append(Password(service, "", password));
			}
		}
		else if(hdr.version == 2)
		{
			// Read passwords
			while(!file.atEnd())
			{
				QString service = readString(file);
				QString username = readString(file);
				QString password = readString(file);

				passwords.append(Password(service, username, password));
			}
		}
		else
		{
			return false;
		}

		if(passwords.size() != hdr.count)
		{
			passwords.clear();
			return false;
		}

		return true;
	}
	return false;
}
