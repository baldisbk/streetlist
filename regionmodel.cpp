#include "regionmodel.h"

#include "region.h"

RegionModel::RegionModel(QObject *parent):
	QAbstractItemModel(parent), mHost(NULL)
{
}

RegionModel::RegionModel(StreetList *host, QObject *parent):
	QAbstractItemModel(parent), mHost(host)
{
}

RegionModel::~RegionModel()
{
}

QModelIndex RegionModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid())
		return QModelIndex();
	else
		return createIndex(row, column);
}

QModelIndex RegionModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}

int RegionModel::rowCount(const QModelIndex &) const
{
	return mRegions.size();
}

int RegionModel::columnCount(const QModelIndex &) const
{
	return 1;
}

QVariant RegionModel::data(const QModelIndex &index, int role) const
{
	if (	index.parent().isValid() ||
		index.row() < 0 ||
		index.row() >= mRegions.size())
		return QVariant();
	Region* region = mRegions.at(index.row());
	switch(role) {
	case Qt::DisplayRole:
	case NameRole:
		return region->name();	
	case SelectedRole:
		return mSelected.value(region->name(), false);
	default:;
	}
	return QVariant();
}

QHash<int, QByteArray> RegionModel::roleNames() const
{
	QHash<int, QByteArray> res;
	res[NameRole] = "name";
	res[SelectedRole] = "selected";
	return res;
}

StreetList *RegionModel::host() const
{
	return mHost;
}

void RegionModel::setHost(StreetList *host)
{
	if (mHost == host)
		return;

	mHost = host;
	init();
	emit hostChanged(host);
}

void RegionModel::init()
{
	if (!mHost) return;

	beginResetModel();
	mRegions.clear();
	mSelected.clear();
	QStringList regions = mHost->regions();
	foreach (QString region, regions) {
		mRegions.append(mHost->region(region));
		mSelected[region] = true;
	}

	endResetModel();
}

void RegionModel::refresh()
{
	emit updated();
}

void RegionModel::select(int index)
{
	if (index < 0 || index >= mRegions.size())
		return;
	Region* region = mRegions.at(index);
	bool flag = mSelected.value(region->name(), false);
	mSelected[region->name()] = !flag;
	emit selected(region->name(), !flag);
	QModelIndex ind = createIndex(index, 0);
	emit dataChanged(ind, ind);
	emit updated();
}
