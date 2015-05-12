#include "streetmodel.h"
#include <QJSValue>
#include <QJSValueIterator>

StreetModel::StreetModel(QObject *parent):
	QAbstractItemModel(parent)
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
	return res;
}

void dump(const QJSValue& v)
{
	QJSValueIterator it(v);
	qDebug() << "===DUMP";
	while (it.hasNext()) {
		it.next();
		qDebug() << it.name() << ": " << it.value().toString();
	}
}

void StreetModel::filt(const QVariant &streets)
{
	QJSValue value = streets.value<QJSValue>();
	int len = value.property("length").toInt();
	dump(value);
	for (int i = 0; i < len; ++i) {
		QJSValueList list;
		list.append(QJSValue(i));
		QJSValue line = value.property("item").call(list);
		dump(line);
		//QJSValue line = value.property("item").call(QJSValueList() << QJSValue(i));
		//qDebug() << line.property("name").toString();
	}
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
	emit hostChanged(host);
}
