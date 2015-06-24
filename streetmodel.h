#ifndef STREETMODEL_H
#define STREETMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

#include "streetdownloader.h"

class BaseStreetModel : public QAbstractItemModel
{
	Q_OBJECT
	Q_ENUMS(Roles)
public:
	BaseStreetModel(QObject* parent = NULL);
	BaseStreetModel(StreetList* host, QObject* parent = NULL);

	enum Roles {
		WholeNameRole = Qt::UserRole + 1,
		NameRole,
		TypeRole,
		NumberRole,
		SecondaryRole,
		HousesRole,
		CanonicalRole
	};

	// QAbstractItemModel interface
public:
	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
	virtual QModelIndex parent(const QModelIndex &) const;
	virtual int rowCount(const QModelIndex &) const;
	virtual int columnCount(const QModelIndex &) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QHash<int, QByteArray> roleNames() const;

	StreetList* host() const;

	Street* street(int row) const;

public slots:
	void setHost(StreetList* host);

	void init();

signals:
	void hostChanged(StreetList* host);

private:
	QList<Street*> mStreets;
	StreetList* mHost;
};

class StreetModel: public QSortFilterProxyModel {
	Q_OBJECT
	Q_ENUMS(Roles)

public:
	StreetModel(QObject* parent = NULL);
	StreetModel(StreetList* host, QObject* parent = NULL);

	enum Roles {
		WholeNameRole = Qt::UserRole + 1,
		NameRole,
		TypeRole,
		NumberRole,
		SecondaryRole,
		HousesRole,
		CanonicalRole
	};

	Q_PROPERTY(StreetList* host READ host WRITE setHost NOTIFY hostChanged)
	Q_PROPERTY(QString searchString READ searchString WRITE setSearchString NOTIFY searchStringChanged)
	Q_PROPERTY(int streetIndex READ streetIndex WRITE setStreetIndex NOTIFY streetIndexChanged)
	Q_PROPERTY(int houseIndex READ houseIndex WRITE setHouseIndex NOTIFY houseIndexChanged)
	Q_PROPERTY(QString houseFound READ houseFound WRITE setHouseFound NOTIFY houseFoundChanged)

	StreetList* host() const;
	QString searchString() const;
	int streetIndex() const;
	int houseIndex() const;
	QString houseFound() const;

public slots:
	QString nameForRow(int row) const;
	int rowForName(QString name) const;

	void setHost(StreetList* arg);
	void setSearchString(QString arg);
	void setStreetIndex(int arg);
	void setHouseIndex(int arg);

	void filter(QString district, bool flag);
	void refresh();
	void init();

	void bruteforce();
	void sort(QString role, bool order);

	void setHouseFound(QString arg);

protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

signals:
	void error(QString desc);
	void notFound();

	void hostChanged(StreetList* arg);
	void houseFoundChanged(QString arg);
	void searchStringChanged(QString arg);
	void streetIndexChanged(int arg);
	void houseIndexChanged(int arg);

private:
	int rowToSource(int row) const;
	int rowFromSource(int row) const;
	void updateHouse();

	BaseStreetModel* mParent;
	QMap<QString, bool> mFilter;
	QString mSearchString;
	int mStreetIndex;
	QString mHouseFound;
	int mHouseIndex;
};

#endif // STREETMODEL_H
