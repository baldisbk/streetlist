#include "streetdownloader.h"

#include <QDebug>

StreetList::StreetList(QObject *parent) : QObject(parent)
{
//	Street *a = new Street("1-я Какая-нить улица");
//	Street *b = new Street("Проспектный проспект");
//	Street *c = new Street("Бульвар Васи Пупкина");
//	Region *r = new Region("ДАО");
//	District *d = new District("Район");
//	District *e = new District("Раён");
//	r->addDistrict(d->name());
//	r->addDistrict(e->name());
//	d->addStreet(a->wholeName());
//	d->addStreet(b->wholeName());
//	e->addStreet(b->wholeName());
//	e->addStreet(c->wholeName());
//	d->setRegion(r->name());
//	e->setRegion(r->name());
//	mStreets[a->wholeName()] = a;
//	mStreets[b->wholeName()] = b;
//	mStreets[c->wholeName()] = c;
//	mDistricts[d->name()] = d;
//	mDistricts[e->name()] = e;
//	mRegions[r->name()] = r;
}

StreetList::~StreetList()
{
	foreach(District* d, mDistricts)
		delete d;
	foreach(Street* s, mStreets)
		delete s;
}

QStringList StreetList::regions() const
{
	return mRegions.keys();
}

QStringList StreetList::districts() const
{
	return mDistricts.keys();
}

QStringList StreetList::streets() const
{
	return mStreets.keys();
}

QVariantMap StreetList::street(QString name) const
{
	QVariantMap res;
	Street* str = mStreets.value(name, NULL);
	if (str) {
		for(int i = 0; i < str->metaObject()->propertyCount(); ++i) {
			const char* cname = str->metaObject()->property(i).name();
			res[cname] = str->property(cname);
		}
		res["houses"] = str->houses().join(",");
	}
	return res;
}

QVariantMap StreetList::district(QString name) const
{
	QVariantMap res;
	District* dis = mDistricts.value(name, NULL);
	if (dis) {
		for(int i = 0; i < dis->metaObject()->propertyCount(); ++i) {
			const char* cname = dis->metaObject()->property(i).name();
			res[cname] = dis->property(cname);
		}
		res["streets"] = dis->streets();
	}
	return res;
}

QVariantMap StreetList::region(QString name) const
{
	QVariantMap res;
	Region* reg = mRegions.value(name, NULL);
	if (reg) {
		for(int i = 0; i < reg->metaObject()->propertyCount(); ++i) {
			const char* cname = reg->metaObject()->property(i).name();
			res[cname] = reg->property(cname);
		}
	}
	return res;
}

void StreetList::addStreet(const QVariantMap &props)
{
	Street* street = new Street(props);
	mStreets[street->wholeName()] = street;
}

void StreetList::addRegion(const QVariantMap &props)
{
	Region* region = new Region(props);
	mRegions[region->name()] = region;
}

void StreetList::addDistrict(const QVariantMap &props)
{
	District* district = new District(props);
	mDistricts[district->name()] = district;
	QString reg = props.value("region").toString();
	if (regionPtr(reg))
		regionPtr(reg)->addDistrict(district->name());
}

void StreetList::addStreetToDistrict(const QVariantMap &props)
{
	Street* str = streetPtr(props.value("street").toString());
	District* dis = districtPtr(props.value("district").toString());
	if (str && dis)
		dis->addStreet(str->wholeName());
}

Street *StreetList::streetPtr(QString name) const
{
	return mStreets.value(name, NULL);
}

District *StreetList::districtPtr(QString name) const
{
	return mDistricts.value(name, NULL);
}

Region *StreetList::regionPtr(QString name) const
{
	return mRegions.value(name, NULL);
}

void StreetList::onReplyArrived(QNetworkReply *reply)
{
	QString key = reply->url().toString();
	if (!mRequests.contains(key))
		return;
	Request req = mRequests[key];
	mRequests.remove(key);
	switch (req.type) {
	case RTCity: {
		QRegExp re("<a href=\"(http://mosopen.ru/district/[a-zA-Z]*)\" title=\".*\">(.*)</a>");
		while (!reply->atEnd()) {
			QString line = reply->readLine().toStdString().c_str();
			if (re.indexIn(line) != -1) {
				Request newreq;
				newreq.name = re.cap(2);
				newreq.type = RTRegion;
				QString url = re.cap(1);
				mRequests.insert(url, newreq);
				QNetworkReply* reply = mManager.get(QNetworkRequest(QUrl(url)));
				connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
					this, SLOT(onReplyError(QNetworkReply::NetworkError)));
				Region* reg = new Region(newreq.name);
				mRegions.insert(reg->name(), reg);
				++mRegNum;
			}
		}
		emit progress(1, 1, RTCity);
		break;
	}
	case RTRegion: {
		QRegExp re("<dd><a href=\"(http://mosopen.ru/region/.*)\" title=\".*\">(.*)</a></dd>");
		while (!reply->atEnd()) {
			QString line = reply->readLine().toStdString().c_str();
			if (re.indexIn(line) != -1) {
				Request newreq;
				newreq.name = re.cap(2);
				newreq.type = RTDistrict;
				QString url = re.cap(1) + "/streets";
				mRequests.insert(url, newreq);
				QNetworkReply* reply = mManager.get(QNetworkRequest(QUrl(url)));
				connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
					this, SLOT(onReplyError(QNetworkReply::NetworkError)));
				District* dist = new District(newreq.name);
				mDistricts.insert(dist->name(), dist);
				dist->setRegion(req.name);
				++mDistNum;
			}
		}
		--mRegNum;
		emit progress(mRegions.size() - mRegNum, mRegions.size(), RTRegion);
		break;
	}
	case RTDistrict: {
		QRegExp re("<li><a href=\"(http://mosopen.ru/street/.*)\">(.*)</a></li>");
		while (!reply->atEnd()) {
			QString line = reply->readLine().toStdString().c_str();
			if (re.indexIn(line) != -1) {
				Request newreq;
				newreq.name = re.cap(2);
				newreq.type = RTStreet;
				QString url = re.cap(1);
				Street* street;
				if (!mStreets.contains(newreq.name)) {
					street = new Street(newreq.name);
					mStreets.insert(street->name(), street);
//					mRequests.insert(url, newreq);
//					QNetworkReply* reply = mManager.get(QNetworkRequest(QUrl(url)));
//					connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
//						this, SLOT(onReplyError(QNetworkReply::NetworkError)));
//					++mStreetNum;
				}
				mDistricts[req.name]->addStreet(street->wholeName());
			}
		}
		--mDistNum;
		emit progress(mDistricts.size() - mDistNum, mDistricts.size(), RTDistrict);
		break;
	}
	case RTStreet: {
		QRegExp re("<a href=\".*\" title=\".*\">(.*)</a>");
		while (!reply->atEnd()) {
			QString line = reply->readLine().toStdString().c_str();
			if (re.indexIn(line) != -1) {
				mStreets[req.name]->addHouse(re.cap(1));
			}
		}
		--mStreetNum;
		emit progress(mStreets.size() - mStreetNum, mStreets.size(), RTStreet);
		break;
	}
	default:;
	}

	if (mRequests.isEmpty())
		emit finished();
}

void StreetList::onReplyError(QNetworkReply::NetworkError code)
{
	qDebug() << "error" << code;
}

void StreetList::download()
{
	connect(&mManager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(onReplyArrived(QNetworkReply*)));
	QString url("http://mosopen.ru");
	Request req;
	req.type = RTCity;
	mRequests.insert(url, req);
	emit progress(0, 1, RTCity);
	emit progress(0, 0, RTRegion);
	emit progress(0, 0, RTDistrict);
	emit progress(0, 0, RTStreet);
	mRegNum = 0;
	mDistNum = 0;
	mStreetNum = 0;
	QNetworkReply* reply = mManager.get(QNetworkRequest(QUrl(url)));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
		this, SLOT(onReplyError(QNetworkReply::NetworkError)));
}

QStringList Street::mStreetTypes;

Street::Street(QObject *parent): QObject(parent)
{
	initTypes();
}

Street::Street(const QString &name, QObject *parent): QObject(parent), mWholeName(name)
{
	initTypes();
	parseName();
}

Street::Street(const QVariantMap &props, QObject *parent): QObject(parent)
{
	initTypes();
	if (props.value("name").isNull()) {
		setWholeName(props.value("wholeName").toString());
	} else {
		mName = props.value("name").toString();
		mType = props.value("type").toString();
		mNumber = props.value("number").toInt();
		mHouses = props.value("houses").toString().split(",");
		joinName();
	}
}

QString Street::name() const
{
	return mName;
}

QString Street::type() const
{
	return mType;
}

int Street::number() const
{
	return mNumber;
}

QStringList Street::houses() const
{
	return mHouses;
}

void Street::addHouse(const QString &house)
{
	mHouses.append(house);
}

QString Street::wholeName() const
{
	return mWholeName;
}

void Street::setName(QString arg)
{
	if (mName == arg)
		return;

	mName = arg;
	emit nameChanged(arg);
	joinName();
}

void Street::setType(QString arg)
{
	if (mType == arg)
		return;

	mType = arg;
	emit typeChanged(arg);
	joinName();
}

void Street::setNumber(int arg)
{
	if (mNumber == arg)
		return;

	mNumber = arg;
	emit numberChanged(arg);
	joinName();
}

void Street::setWholeName(QString arg)
{
	if (mWholeName == arg)
		return;

	mWholeName = arg;
	emit wholeNameChanged(arg);
	parseName();
}

void Street::joinName()
{
	if (mNumber > 0)
		setWholeName(QString("%1 %2 %3").arg(mNumber).arg(mName).arg(mType));
	else
		setWholeName(QString("%1 %2").arg(mName).arg(mType));
}

void Street::parseName()
{
	QRegExp captureType(QString("\\b(%1)\\b").arg(mStreetTypes.join("|")));
	captureType.setCaseSensitivity(Qt::CaseInsensitive);
	QRegExp captureNum("(N\\s)?(\\d+)(а|б|в|г|д|е|-(й|ый|ой|ий|я|ая|яя|е|ое|ье))?");

	QString partName = mWholeName;
	if (captureType.indexIn(partName) != -1) {
		setType(captureType.cap(0).toLower());
		partName.remove(captureType);
	}
	if (captureNum.indexIn(partName) != -1) {
		setNumber(captureNum.cap(2).toInt());
		partName.remove(captureNum);
	} else setNumber(0);
	setName(partName.simplified());
	joinName();
}

void Street::processHouses()
{
	QRegExp houseNum("(вл\\s*)?(\\d*)(А|Б|В|Г|Д|Е|Ж|З|И|К)?(\\s*(к|с|стр)\\s*\\d*)*");
}

void Street::initTypes()
{
	if (mStreetTypes.isEmpty()) {
		mStreetTypes
			<< "улица"
			<< "проспект"
			<< "шоссе"
			<< "проезд"
			<< "переулок"
			<< "бульвар"
			<< "аллея"
			<< "площадь"
			<< "посёлок"
			<< "мост"
			<< "набережная"
			<< "тупик"
			<< "просека"
			<< "квартал"
			<< "микрорайон"
			<< "деревня"
			<< "парк"
			<< "линия";
	}
}


District::District(QObject *parent): QObject(parent)
{
}

District::District(const QString &name, QObject *parent): QObject(parent), mName(name)
{
}

District::District(const QVariantMap &props, QObject *parent): QObject(parent)
{
	mName = props.value("name").toString();
	mRegion = props.value("region").toString();
}

QString District::name() const
{
	return mName;
}

QString District::region() const
{
	return mRegion;
}

void District::addStreet(QString street)
{
	if (!mStreets.contains(street))
		mStreets.append(street);
}

QStringList District::streets() const
{
	return mStreets;
}

void District::setName(QString arg)
{
	if (mName == arg)
		return;

	mName = arg;
	emit nameChanged(arg);
}

void District::setRegion(QString arg)
{
	if (mRegion == arg)
		return;

	mRegion = arg;

	emit regionChanged(arg);
}


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

QString Region::name() const
{
	return mName;
}

void Region::addDistrict(QString district)
{
	if (!mDistricts.contains(district))
		mDistricts.append(district);
}

QStringList Region::districts() const
{
	return mDistricts;
}

void Region::setName(QString arg)
{
	if (mName == arg)
		return;

	mName = arg;
	emit nameChanged(arg);
}
