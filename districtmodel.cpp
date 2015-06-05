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
	case SelectedRole:
		return mSelected.value(district->name(), false);
	default:;
	}
	return QVariant();
}

QHash<int, QByteArray> DistrictModel::roleNames() const
{
	QHash<int, QByteArray> res;
	res[NameRole] = "name";
	res[RegionRole] = "region";
	res[SelectedRole] = "selected";
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
	mSelected.clear();
	if (mHost) {
		QStringList districts = mHost->districts();
		foreach (QString district, districts) {
			District* dist = mHost->district(district);
			if (mFilter.isEmpty() || mFilter.contains(dist->region()->name())) {
				mDistricts.append(dist);
				mSelected[district] = true;
				emit selected(district, true);
			}
		}
	}
	endResetModel();
}

void DistrictModel::select(int index)
{
	if (index < 0 || index >= mDistricts.size())
		return;
	District* district = mDistricts.at(index);
	bool flag = mSelected.value(district->name(), false);
	mSelected[district->name()] = !flag;
	emit selected(district->name(), !flag);
	QModelIndex ind = createIndex(index, 0);
	emit dataChanged(ind, ind);
}

void DistrictModel::filter(QString region, bool flag)
{
	if (flag) {
		if (!mFilter.contains(region))
			mFilter.append(region);
	} else {
		if (mFilter.contains(region))
			mFilter.removeAll(region);
	}
	reload();
}
