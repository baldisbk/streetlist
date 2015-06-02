#include "districtmodel.h"

#include "district.h"
#include "region.h"

DistrictModel::DistrictModel(QObject *parent):
	QAbstractItemModel(parent), mHost(NULL)
{
}

DistrictModel::DistrictModel(StreetList *host, QObject *parent):
	QAbstractItemModel(parent), mHost(host)
{
}

DistrictModel::~DistrictModel()
{
}

QModelIndex DistrictModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid())
		return QModelIndex();
	else
		return createIndex(row, column);
}

QModelIndex DistrictModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}

int DistrictModel::rowCount(const QModelIndex &) const
{
	return mDistricts.size();
}

int DistrictModel::columnCount(const QModelIndex &) const
{
	return 1;
}

QVariant DistrictModel::data(const QModelIndex &index, int role) const
{
	if (	index.parent().isValid() ||
		index.row() < 0 ||
		index.row() >= mDistricts.size())
		return QVariant();
	District* district = mDistricts.at(index.row());
	switch(role) {
	case Qt::DisplayRole:
	case NameRole:
		return district->name();
	case RegionRole:
		return district->region()->name();
	default:;
	}
	return QVariant();
}

QHash<int, QByteArray> DistrictModel::roleNames() const
{
	QHash<int, QByteArray> res;
	res[NameRole] = "name";
	res[RegionRole] = "region";
	return res;
}

StreetList *DistrictModel::host() const
{
	return mHost;
}

void DistrictModel::setHost(StreetList *host)
{
	if (mHost == host)
		return;

	mHost = host;
	reload();
	emit hostChanged(host);
}

void DistrictModel::reload()
{
	beginResetModel();
	mDistricts.clear();
	if (mHost) {
		QStringList districts = mHost->districts();
		foreach (QString district, districts)
			mDistricts.append(mHost->district(district));
	}
	endResetModel();
}
