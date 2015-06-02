#include "streetmodel.h"
#include <QJSValue>
#include <QJSValueIterator>

StreetModel::StreetModel(QObject *parent):
	QAbstractItemModel(parent), mHost(NULL)
{
}

StreetModel::StreetModel(StreetList *host, QObject *parent):
	QAbstractItemModel(parent), mHost(host)
{
}

StreetModel::~StreetModel()
{
}

QModelIndex StreetModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid())
		return QModelIndex();
	else
		return createIndex(row, column);
}

QModelIndex StreetModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}

int StreetModel::rowCount(const QModelIndex &) const
{
	return mStreets.size();
}

int StreetModel::columnCount(const QModelIndex &) const
{
	return 1;
}

QVariant StreetModel::data(const QModelIndex &index, int role) const
{
	if (	index.parent().isValid() ||
		index.row() < 0 ||
		index.row() >= mStreets.size())
		return QVariant();
	Street* street = mStreets.at(index.row());
	switch(role) {
	case Qt::DisplayRole:
	case WholeNameRole:
		return street->wholeName();
	case NameRole:
		return street->name();
	case TypeRole:
		return street->type();
	case NumberRole:
		return street->number();
	case HousesRole:
		return street->houses();
	case SecondaryRole:
		return street->secondary();
	default:;
	}
	return QVariant();
}

QHash<int, QByteArray> StreetModel::roleNames() const
{
	QHash<int, QByteArray> res;
	res[WholeNameRole] = "wholeName";
	res[NameRole] = "name";
	res[TypeRole] = "type";
	res[NumberRole] = "number";
	res[HousesRole] = "houses";
	res[SecondaryRole] = "secondary";
	return res;
}

StreetList *StreetModel::host() const
{
	return mHost;
}

void StreetModel::setHost(StreetList *host)
{
	if (mHost == host)
		return;

	mHost = host;

	if (host) {
		beginResetModel();
		QStringList streets = host->streets();
		mStreets.clear();
		foreach (QString street, streets)
			mStreets.append(host->street(street));
		endResetModel();
	} else
		mStreets.clear();

	emit hostChanged(host);
}

void StreetModel::reload()
{
	// TODO dirty
	StreetList* list = host();
	setHost(NULL);
	setHost(list);
}
