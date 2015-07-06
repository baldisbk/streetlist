#ifndef HOUSEMODEL_H
#define HOUSEMODEL_H

#include <QAbstractItemModel>

#include "streetdownloader.h"

class HouseModel : public QAbstractItemModel
{
	Q_OBJECT
	Q_ENUMS(Roles)
public:
	HouseModel(QObject* parent = NULL);
	HouseModel(StreetList* host, QObject* parent = NULL);
	~HouseModel();

	enum Roles {
		NameRole = Qt::UserRole + 1
	};

	// QAbstractItemModel interface
public:
	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
	virtual QModelIndex parent(const QModelIndex &) const;
	virtual int rowCount(const QModelIndex &) const;
	virtual int columnCount(const QModelIndex &) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QHash<int, QByteArray> roleNames() const;

	Q_PROPERTY(StreetList* host READ host WRITE setHost NOTIFY hostChanged)
	Q_PROPERTY(QString street READ street WRITE setStreet NOTIFY streetChanged)

	StreetList* host() const;
	QString street() const;

public slots:
	QString nameForRow(int row) const;
	int rowForName(QString name) const;

	void setHost(StreetList* host);
	void setStreet(QString arg);

	void reload();

signals:
	void hostChanged(StreetList* host);
	void streetChanged(QString arg);

private:
	QStringList mHouses;
	StreetList* mHost;
	QString mStreet;
};

#endif // HOUSEMODEL_H
