#ifndef DISTRICTMODEL_H
#define DISTRICTMODEL_H

#include <QAbstractItemModel>

#include "streetdownloader.h"

class DistrictModel : public QAbstractItemModel
{
	Q_OBJECT
	Q_ENUMS(Roles)
public:
	DistrictModel(QObject* parent = NULL);
	DistrictModel(StreetList* host, QObject* parent = NULL);
	~DistrictModel();

	enum Roles {
		NameRole = Qt::UserRole + 1,
		RegionRole
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

	void reload();

signals:
	void hostChanged(StreetList* host);

private:
	QList<District*> mDistricts;
	StreetList* mHost;
};

#endif // DISTRICTMODEL_H
