#include "streetmodel.h"

#include "tokenparser.h"
#include "street.h"

BaseStreetModel::BaseStreetModel(QObject *parent):
	QAbstractItemModel(parent), mHost(NULL)
{
}

BaseStreetModel::BaseStreetModel(StreetList *host, QObject *parent):
	QAbstractItemModel(parent), mHost(host)
{
}

QModelIndex BaseStreetModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid())
		return QModelIndex();
	else
		return createIndex(row, column);
}

QModelIndex BaseStreetModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}

int BaseStreetModel::rowCount(const QModelIndex &) const
{
	return mStreets.size();
}

int BaseStreetModel::columnCount(const QModelIndex &) const
{
	return 1;
}

QVariant BaseStreetModel::data(const QModelIndex &index, int role) const
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
	case CanonicalRole:
		return street->canonical();
	default:;
	}
	return QVariant();
}

QHash<int, QByteArray> BaseStreetModel::roleNames() const
{
	QHash<int, QByteArray> res;
	res[WholeNameRole] = "wholeName";
	res[NameRole] = "name";
	res[TypeRole] = "type";
	res[NumberRole] = "number";
	res[HousesRole] = "houses";
	res[SecondaryRole] = "secondary";
	res[CanonicalRole] = "canonical";
	return res;
}

StreetList *BaseStreetModel::host() const
{
	return mHost;
}

Street *BaseStreetModel::street(int row) const
{
	if (row < 0 || row >= mStreets.size())
		return NULL;
	return mStreets.at(row);
}

void BaseStreetModel::setHost(StreetList *host)
{
	if (mHost == host)
		return;

	mHost = host;
	emit hostChanged(host);
}

void BaseStreetModel::init()
{
	if (!mHost) return;

	beginResetModel();

	mStreets.clear();
	QStringList streets = mHost->streets();
	foreach (QString street, streets) {
		Street* str = mHost->street(street);
		mStreets.append(str);
	}

	endResetModel();
}


StreetModel::StreetModel(QObject *parent): QSortFilterProxyModel(parent)
{
	mParent = new BaseStreetModel(this);
	setSourceModel(mParent);
	connect(mParent, SIGNAL(hostChanged(StreetList*)),
		this, SIGNAL(hostChanged(StreetList*)));
}

StreetModel::StreetModel(StreetList *host, QObject *parent):
	QSortFilterProxyModel(parent)
{
	mParent = new BaseStreetModel(host, this);
	setSourceModel(mParent);
	connect(mParent, SIGNAL(hostChanged(StreetList*)),
		this, SIGNAL(hostChanged(StreetList*)));
}

QString StreetModel::nameForRow(int row) const
{
	Street* str = mParent->street(rowToSource(row));
	if (str)
		return str->wholeName();
	else
		return QString();
}

int StreetModel::rowForName(QString name) const
{
	for (int i = 0; i < rowCount(); ++i)
		if (nameForRow(i) == name)
			return i;
	return -1;
}

void StreetModel::filter(QString district, bool flag)
{
	mFilter[district] = flag;
}

void StreetModel::refresh()
{
	invalidateFilter();
}

void StreetModel::bruteforce(const QString &exp, int indexFrom)
{
	MultiParser tp;
	if (!tp.setExpr(exp)) {
		emit error(QString("Expression error: %1").arg(tp.errors()));
		return;
	}
	int stindex = indexFrom;
	if (stindex < 0)
		stindex = 0;
	else {
		++stindex;
		if (stindex == rowCount())
			stindex = 0;
	}
	int index = stindex;
	while (true) {
		Street* street = mParent->street(rowToSource(index));
		QString res = tp.calculate(street);
		if (!tp.errors().isEmpty()) {
			emit error(QString("Evaluate error: %1").arg(tp.errors()));
			return;
		}
		if (!res.isEmpty()) {
			int hIndex = street->houseList().indexOf(res);
			if (hIndex != -1) {
				emit selected(index, hIndex);
				return;
			}
		}

		++index;
		if (index == rowCount())
			index = 0;
		if (index == stindex)
			break;
	}
	emit notFound();
}

void StreetModel::sort(QString role, bool order)
{
	int irole = roleNames().key(role.toStdString().c_str(), -1);
	if (irole == -1)
		return;
	setSortRole(irole);
	QSortFilterProxyModel::sort(
		0, order ? Qt::DescendingOrder : Qt::AscendingOrder);
}

StreetList *StreetModel::host() const
{
	return mParent->host();
}

void StreetModel::setHost(StreetList *arg)
{
	mParent->setHost(arg);
	init();
}

void StreetModel::init()
{
	mParent->init();
	mFilter.clear();
	foreach(QString dist, host()->districts())
		mFilter[dist] = true;

	invalidate();
}

bool StreetModel::filterAcceptsRow(int source_row, const QModelIndex &) const
{
	Street* str = mParent->street(source_row);
	bool accepted = false;
	foreach(QString district, str->districts())
		if (mFilter.value(district, false)) {
			accepted = true;
			break;
		}
	return accepted;
}

int StreetModel::rowToSource(int row) const
{
	QModelIndex pi = index(row, 0);
	QModelIndex si = mapToSource(pi);
	if (si.isValid())
		return si.row();
	else
		return -1;
}

int StreetModel::rowFromSource(int row) const
{
	QModelIndex si = mParent->index(row, 0, QModelIndex());
	QModelIndex pi = mapFromSource(si);
	if (pi.isValid())
		return pi.row();
	else
		return -1;
}
