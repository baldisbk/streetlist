#ifndef STREETMODEL_H
#define STREETMODEL_H

#include <QAbstractItemModel>

#include "streetdownloader.h"

class StreetModel : public QAbstractItemModel
{
	Q_OBJECT
	Q_ENUMS(Roles)
public:
	StreetModel(QObject* parent = NULL);
	StreetModel(StreetList* host, QObject* parent = NULL);
	~StreetModel();

	enum Roles {
		WholeNameRole = Qt::UserRole + 1,
		NameRole,
		TypeRole,
		NumberRole,
		SecondaryRole,
		HousesRole
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

	StreetList* host() const;

public slots:
	void setHost(StreetList* host);

	void init();
	void filter(QString district, bool flag);
	void refresh();

signals:
	void hostChanged(StreetList* host);

private:
	QList<Street*> mStreets;
	StreetList* mHost;
	QMap<QString, bool> mFilter;
};

#endif // STREETMODEL_H
