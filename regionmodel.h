#ifndef REGIONMODEL_H
#define REGIONMODEL_H

#include <QAbstractItemModel>

#include "streetdownloader.h"

class RegionModel : public QAbstractItemModel
{
	Q_OBJECT
	Q_ENUMS(Roles)
public:
	RegionModel(QObject* parent = NULL);
	RegionModel(StreetList* host, QObject* parent = NULL);
	~RegionModel();

	enum Roles {
		NameRole = Qt::UserRole + 1,
		SelectedRole
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

	void select(int index);

signals:
	void hostChanged(StreetList* host);
	void selected(QString name, bool flag);

private:
	QList<Region*> mRegions;
	QMap<QString, bool> mSelected;
	StreetList* mHost;
};

#endif // REGIONMODEL_H
