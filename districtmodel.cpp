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
	mFilter.clear();
	mSelected.clear();
	foreach(QString reg, mHost->regions())
		mFilter.append(reg);
	init();
	emit hostChanged(host);
}

void DistrictModel::init()
{
	if (!mHost) return;

	beginResetModel();
	mDistricts.clear();
	mSelected.clear();
	mFilter.clear();

	QStringList districts = mHost->districts();
	foreach (QString district, districts) {
		District* dist = mHost->district(district);
		mDistricts.append(dist);
		mSelected[district] = true;
	}
	foreach (QString region, mHost->regions())
		mFilter.append(region);

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
	emit updated();
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
}

void DistrictModel::refresh()
{
	if (!mHost) return;

	beginResetModel();

	QStringList districts = mHost->districts();
	foreach (QString district, districts) {
		District* dist = mHost->district(district);
		bool old = mDistricts.contains(dist);
		if (mFilter.contains(dist->region()->name())) {
			if (!old) {
				mDistricts.append(dist);
				mSelected[district] = true;
			}
		} else {
			mSelected[district] = false;
			if (old)
				mDistricts.removeAll(dist);
		}
		emit selected(district, mSelected[district]);
	}

	endResetModel();
	emit updated();
}
