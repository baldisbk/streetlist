#include "district.h"

#include <QDebug>

#include "street.h"
#include "region.h"

District::District(QObject *parent): QObject(parent), mRegion(NULL)
{
}

District::District(const QString &name, QObject *parent): QObject(parent), mName(name), mRegion(NULL)
{
}

District::District(const QVariantMap &props, QObject *parent): QObject(parent), mRegion(NULL)
{
	mName = props.value("name").toString();
}

District::~District()
{
	setRegion(NULL);
	QList<Street*> streets = mStreets.values();
	foreach(Street* street, streets)
		removeStreet(street);
}

QString District::name() const
{
	return mName;
}

Region *District::region() const
{
	return mRegion;
}

void District::addStreet(Street* street)
{
	if (!street) return;
	if (!mStreets.contains(street->wholeName()))
		mStreets.insert(street->wholeName(), street);
	if (!street->districts().contains(name()))
		street->addDistrict(this);
}

void District::removeStreet(Street *street)
{
	if (!street) return;
	if (mStreets.contains(street->wholeName()))
		mStreets.remove(street->wholeName());
	if (street->districts().contains(name()))
		street->removeDistrict(this);
}

QStringList District::streets() const
{
	return mStreets.keys();
}

void District::setName(QString arg)
{
	if (mName == arg)
		return;

	mName = arg;
	emit nameChanged(arg);
}

void District::setRegion(Region* region)
{
	if (mRegion == region)
		return;

	if (mRegion && mRegion->districts().contains(name()))
		mRegion->removeDistrict(this);

	mRegion = region;

	if (region && !region->districts().contains(name()))
		region->addDistrict(this);

	emit regionChanged(region);
}

void District::check() const
{
	QStringList keys = mStreets.keys();
	foreach(QString key, keys) {
		Street* s = mStreets.value(key);
		if (!s)
			qDebug() << "NULL street key in district" << name() << key;
		else {
			if (s->wholeName() != key)
				qDebug() << "Wrong street key in district" <<
					    name() << key << s->wholeName();
			if (!s->districts().contains(name()))
				qDebug() << "Missing street backref in district" <<
					    name() << key;
		}
	}
	if (!mRegion)
		qDebug() << "NULL region in district" << name();
	else if (!mRegion->districts().contains(name()))
		qDebug() << "Missing region backref in district" << name();
}
