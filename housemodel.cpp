#include "housemodel.h"

#include "street.h"

HouseModel::HouseModel(QObject *parent):
	QAbstractItemModel(parent), mHost(NULL)
{
}

HouseModel::HouseModel(StreetList *host, QObject *parent):
	QAbstractItemModel(parent), mHost(host)
{
}

HouseModel::~HouseModel()
{
}

QModelIndex HouseModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid())
		return QModelIndex();
	else
		return createIndex(row, column);
}

QModelIndex HouseModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}

int HouseModel::rowCount(const QModelIndex &) const
{
	return mHouses.size();
}

int HouseModel::columnCount(const QModelIndex &) const
{
	return 1;
}

QVariant HouseModel::data(const QModelIndex &index, int role) const
{
	if (	index.parent().isValid() ||
		index.row() < 0 ||
		index.row() >= mHouses.size())
		return QVariant();
	switch(role) {
	case Qt::DisplayRole:
	case NameRole:
		return mHouses.at(index.row());
	default:;
	}
	return QVariant();
}

QHash<int, QByteArray> HouseModel::roleNames() const
{
	QHash<int, QByteArray> res;
	res[NameRole] = "name";
	return res;
}

StreetList *HouseModel::host() const
{
	return mHost;
}

void HouseModel::setHost(StreetList *host)
{
	if (mHost == host)
		return;

	mHost = host;
	reload();

	emit hostChanged(host);
}

void HouseModel::reload()
{
	beginResetModel();
	mHouses.clear();
	if (mHost && !mStreet.isEmpty()) {
		Street* street = mHost->street(mStreet);
		mHouses = street->houseList();
	}
	endResetModel();
}

QString HouseModel::street() const
{
	return mStreet;
}

QString HouseModel::nameForRow(int row) const
{
	if (row >= 0 && row < mHouses.size())
		return mHouses.at(row);
	else
		return QString();
}

int HouseModel::rowForName(QString name) const
{
	return mHouses.indexOf(name);
}

void HouseModel::setStreet(QString arg)
{
	if (mStreet == arg)
		return;

	mStreet = arg;
	reload();
	emit streetChanged(arg);
}

