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

	StreetList* host() const;

public slots:
	QString nameForRow(int row) const;
	int rowForName(QString name) const;

	void filter(QString district, bool flag);
	void refresh();
	void setHost(StreetList* arg);
	void init();

	void bruteforce(const QString& exp, int indexFrom);

protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

signals:
	void hostChanged(StreetList* arg);
	void selected(int index, int hindex);
	void error(QString desc);
	void notFound();

private:
	int rowToSource(int row) const;
	int rowFromSource(int row) const;

	BaseStreetModel* mParent;
	QMap<QString, bool> mFilter;
};

#endif // STREETMODEL_H
