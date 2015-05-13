#include "region.h"

#include <QDebug>

#include "district.h"

Region::Region(QObject *parent): QObject(parent)
{
}

Region::Region(const QString &name, QObject *parent): QObject(parent), mName(name)
{
}

Region::Region(const QVariantMap &props, QObject *parent): QObject(parent)
{
	mName = props.value("name").toString();
}

Region::~Region()
{
	QList<District*> districts;
	foreach(District* district, districts)
		removeDistrict(district);
}

QString Region::name() const
{
	return mName;
}

void Region::addDistrict(District* district)
{
	if (!district) return;
	if (!mDistricts.contains(district->name()))
		mDistricts.insert(district->name(), district);
	if (district->region() != this)
		district->setRegion(this);
}

QStringList Region::districts() const
{
	return mDistricts.keys();
}

void Region::removeDistrict(District *district)
{
	if (!district) return;
	if (mDistricts.contains(district->name()))
		mDistricts.remove(district->name());
	if (district->region() == this)
		district->setRegion(NULL);
}

void Region::setName(QString arg)
{
	if (mName == arg)
		return;

	mName = arg;
	emit nameChanged(arg);
}

void Region::check() const
{
	QStringList keys = mDistricts.keys();
	foreach(QString key, keys) {
		if (!mDistricts.value(key))
			qDebug() << "NULL for district key in region" <<
				    name() << key;
		else {
			District* d = mDistricts.value(key);
			if (d->name() != key)
				qDebug() << "Wrong district key in region" <<
					    name() << key << d->name();
			if (d->region() != this)
				qDebug() << "Wrong district backref in region" <<
					    name() << key <<
					    (d->region()?d->region()->name():"NULL");
		}
	}
}


