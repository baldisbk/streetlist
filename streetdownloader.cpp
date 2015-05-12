#include "streetdownloader.h"

#include <QQmlEngine>

#include <QDebug>

StreetList::StreetList(QObject *parent) : QObject(parent)
{
//	Street *a = new Street("1-я Какая-нить улица");
//	Street *b = new Street("Проспектный проспект");
//	Street *c = new Street("Бульвар Васи Пупкина");
//	Region *r = new Region("ДАО");
//	District *d = new District("Район");
//	District *e = new District("Раён");
//	r->addDistrict(d);
//	r->addDistrict(e);
//	d->addStreet(a);
//	d->addStreet(b);
//	e->addStreet(b);
//	e->addStreet(c);
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

QVariantMap StreetList::streetV(QString name) const
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

QVariantMap StreetList::districtV(QString name) const
{
	QVariantMap res;
	District* dis = mDistricts.value(name, NULL);
	if (dis) {
		for(int i = 0; i < dis->metaObject()->propertyCount(); ++i) {
			const char* cname = dis->metaObject()->property(i).name();
			res[cname] = dis->property(cname);
		}
		res["streets"] = dis->streets();
		res["region"] = dis->region()->name();
	}
	return res;
}

QVariantMap StreetList::regionV(QString name) const
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
	QQmlEngine::setObjectOwnership(street, QQmlEngine::CppOwnership);
	mStreets[street->wholeName()] = street;
}

void StreetList::addRegion(const QVariantMap &props)
{
	Region* region = new Region(props);
	QQmlEngine::setObjectOwnership(region, QQmlEngine::CppOwnership);
	mRegions[region->name()] = region;
}

void StreetList::addDistrict(const QVariantMap &props)
{
	District* district = new District(props);
	QQmlEngine::setObjectOwnership(district, QQmlEngine::CppOwnership);
	mDistricts[district->name()] = district;
	QString reg = props.value("region").toString();
	if (region(reg))
		region(reg)->addDistrict(district);
}

void StreetList::addStreetToDistrict(const QVariantMap &props)
{
	Street* str = street(props.value("street").toString());
	District* dis = district(props.value("district").toString());
	if (str && dis)
		dis->addStreet(str);
}

Street *StreetList::street(QString name) const
{
	return mStreets.value(name, NULL);
}

District *StreetList::district(QString name) const
{
	return mDistricts.value(name, NULL);
}

Region *StreetList::region(QString name) const
{
	return mRegions.value(name, NULL);
}

void StreetList::onReplyArrived(QNetworkReply *reply)
{
	QString key = reply->url().toString();
	if (!mRequests.contains(key))
		return;
	Request* req = mRequests[key];
	QList<Request*> reqs = parseRequest(reply, req);

	foreach(Request* req, reqs) {
		QNetworkReply* reply = mManager.get(QNetworkRequest(QUrl(req->url)));
		connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
			this, SLOT(onReplyError(QNetworkReply::NetworkError)));
	}

	reply->deleteLater();

	if (mRequests.isEmpty())
		emit finished();
}

void StreetList::onReplyError(QNetworkReply::NetworkError code)
{
	qDebug() << "error" << code;
}

QList<StreetList::Request *> StreetList::parseRequest(QIODevice *input, StreetList::Request *parent)
{
	qDebug() << "parsing" << parent->name << parent->url;
	mRequests.remove(parent->url);
	QList<Request*> newreqs;
	switch (parent->type) {
	case RTCity:
		newreqs = parseCity(input, parent);
		break;
	case RTRegion:
		newreqs = parseRegion(input, parent);
		break;
	case RTDistrict:
		newreqs = parseDistrict(input, parent);
		break;
	case RTStreet:
		newreqs = parseStreet(input, parent);
		break;
	default:;
	}

	foreach(Request* req, newreqs)
		mRequests.insert(req->url, req);

	delete parent;
	return newreqs;
}

QList<StreetList::Request*> StreetList::parseCity(QIODevice *input, Request*)
{
	QList<Request*> requests;
	QRegExp re("<a href=\"(http://mosopen.ru/district/[a-zA-Z]*)\" title=\".*\">(.*)</a>");
	while (!input->atEnd()) {
		QString line = input->readLine().toStdString().c_str();
		if (re.indexIn(line) != -1) {
			Request *newreq = new Request;
			newreq->name = re.cap(2);
			newreq->type = RTRegion;
			newreq->url = re.cap(1);
			requests.append(newreq);

			Region* reg = new Region(newreq->name);
			QQmlEngine::setObjectOwnership(reg, QQmlEngine::CppOwnership);
			mRegions.insert(reg->name(), reg);
			++mRegNum;
		}
	}
	emit progress(1, 1, RTCity);
	return requests;
}

QList<StreetList::Request*> StreetList::parseRegion(QIODevice *input, Request *parent)
{
	QList<Request*> requests;
	QRegExp re("<dd><a href=\"(http://mosopen.ru/region/.*)\" title=\".*\">(.*)</a></dd>");
	while (!input->atEnd()) {
		QString line = input->readLine().toStdString().c_str();
		if (re.indexIn(line) != -1) {
			Request *newreq = new Request;
			newreq->name = re.cap(2);
			newreq->type = RTDistrict;
			newreq->url = re.cap(1) + "/streets";
			requests.append(newreq);

			District* dist = new District(newreq->name);
			QQmlEngine::setObjectOwnership(dist, QQmlEngine::CppOwnership);
			mDistricts.insert(dist->name(), dist);
			dist->setRegion(region(parent->name));
			++mDistNum;
		}
	}
	--mRegNum;
	emit progress(mRegions.size() - mRegNum, mRegions.size(), RTRegion);
	return requests;
}

QList<StreetList::Request*> StreetList::parseDistrict(QIODevice *input, Request *parent)
{
	QList<Request*> requests;
	QRegExp re("<li><a href=\"(http://mosopen.ru/street/.*)\">(.*)</a></li>");
	while (!input->atEnd()) {
		QString line = input->readLine().toStdString().c_str();
		if (re.indexIn(line) != -1) {
			Request *newreq = new Request;
			newreq->name = Street::normalize(re.cap(2));
			newreq->type = RTStreet;
			newreq->url = re.cap(1);

			Street* street;
			if (!mStreets.contains(newreq->name)) {
				street = new Street(newreq->name);
				QQmlEngine::setObjectOwnership(street, QQmlEngine::CppOwnership);
				mStreets.insert(street->wholeName(), street);
//				requests.append(newreq);
//				++mStreetNum;
			} else
				street = mStreets[newreq->name];
			mDistricts[parent->name]->addStreet(street);
		}
	}
	--mDistNum;
	emit progress(mDistricts.size() - mDistNum, mDistricts.size(), RTDistrict);
	return requests;
}

QList<StreetList::Request*> StreetList::parseStreet(QIODevice *input, Request *parent)
{
	QRegExp re("<a href=\".*\" title=\".*\">(.*)</a>");
	while (!input->atEnd()) {
		QString line = input->readLine().toStdString().c_str();
		if (re.indexIn(line) != -1) {
			mStreets[parent->name]->addHouse(re.cap(1));
		}
	}
	--mStreetNum;
	emit progress(mStreets.size() - mStreetNum, mStreets.size(), RTStreet);
	return QList<Request*>();
}

void StreetList::download()
{
	clear();
	connect(&mManager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(onReplyArrived(QNetworkReply*)));
	QString url("http://mosopen.ru");
	Request *req = new Request;
	req->type = RTCity;
	req->url = url;
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

void StreetList::loadFiles()
{
	clear();
	QString path("/home/baldis/Documents/maps/maps/tmp/");
	QString url("main");
	Request *req = new Request;
	req->type = RTCity;
	req->url = url;
	mRequests.insert(url, req);
	emit progress(0, 1, RTCity);
	emit progress(0, 0, RTRegion);
	emit progress(0, 0, RTDistrict);
	emit progress(0, 0, RTStreet);
	mRegNum = 0;
	mDistNum = 0;
	mStreetNum = 0;
	while(!mRequests.isEmpty()) {
		Request* req = mRequests.first();
		int section = (req->type == RTDistrict)?-2:-1;
		QString name = path + req->url.section("/", section, section) + ".html";
		QFile file(name);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qDebug() << name << "not found";
			mRequests.remove(req->url);
			delete req;
			continue;
		}
		parseRequest(&file, req);
	}
}

void StreetList::check() const
{
	// regions
	QStringList keys = mRegions.keys();
	foreach(QString key, keys) {
		if (!mRegions.value(key))
			qDebug() << "NULL for region key" << key;
		else {
			if (mRegions.value(key)->name() != key)
				qDebug() << "Wrong region value" <<
					    key << mRegions.value(key)->name();
			mRegions.value(key)->check();
		}
	}
	keys = mDistricts.keys();
	foreach(QString key, keys) {
		if (!mDistricts.value(key))
			qDebug() << "NULL for district key" << key;
		else {
			if (mDistricts.value(key)->name() != key)
				qDebug() << "Wrong district value" <<
					    key << mDistricts.value(key)->name();
			mDistricts.value(key)->check();
		}
	}
	keys = mStreets.keys();
	foreach(QString key, keys) {
		if (!mStreets.value(key))
			qDebug() << "NULL for street key" << key;
		else {
			if (mStreets.value(key)->wholeName() != key)
				qDebug() << "Wrong street value" <<
					    key << mStreets.value(key)->name();
			mStreets.value(key)->check();
		}
	}
}

void StreetList::clear()
{
	foreach(District* d, mDistricts)
		delete d;
	foreach(Street* s, mStreets)
		delete s;
	mRegions.clear();
	mStreets.clear();
	mDistricts.clear();
}

//================ STREET ==================

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

Street::~Street()
{
	QList<District*> districts = mDistricts.values();
	foreach(District* district, districts)
		removeDistrict(district);
}

QString Street::name() const
{
	return mName;
}

QString Street::type() const
{
	return mType;
}

QString Street::number() const
{
	return mNumber;
}

QString Street::houses() const
{
	return mHouses.join(',');
}

void Street::addHouse(const QString &house)
{
	mHouses.append(house);
}

QStringList Street::houseList() const
{
	return mHouses;
}

QStringList Street::districts() const
{
	return mDistricts.keys();
}

void Street::addDistrict(District *district)
{
	if (!district) return;
	if (!mDistricts.contains(district->name()))
		mDistricts.insert(district->name(), district);
	if (!district->streets().contains(wholeName()))
		district->addStreet(this);
}

void Street::removeDistrict(District *district)
{
	if (!district) return;
	if (mDistricts.contains(district->name()))
		mDistricts.remove(district->name());
	if (district->streets().contains(wholeName()))
		district->removeStreet(this);
}

QString Street::normalize(QString name)
{
	return join(split(name));
}

QStringList Street::split(QString name)
{
	QStringList res;
	for(int i = 0; i < nfNumberOfFields; ++i)
		res.append(QString());

	QRegExp captureType(QString("\\b(%1)\\b").arg(mStreetTypes.join("|")));
	captureType.setCaseSensitivity(Qt::CaseInsensitive);
	QRegExp captureNum1("N\\s(\\d+)");
	QRegExp captureNum2("(\\d+)(а|б|в|г|д|е|-(й|ый|ой|ий|я|ая|яя|е|ое|ье))");

	QString n = name;
	n.remove(",");
	if (captureNum1.indexIn(n) != -1) {
		res[nfNumber] = captureNum1.cap(1);
		n.remove(captureNum1);
	}
	if (captureNum2.indexIn(n) != -1) {
		res[nfNumber] = captureNum2.cap(1);
		n.remove(captureNum2);
	}
	if (captureType.indexIn(n) != -1) {
		res[nfType] = captureType.cap(0).toLower();
		n.remove(captureType);
	}
	res[nfName] = n.simplified();

	return res;
}

QString Street::join(QStringList names)
{
	QStringList out;
	out << names[nfNumber] << names[nfSecondary]
	       << names[nfName] << names[nfType];
	return names.join(' ').simplified();
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

void Street::setNumber(QString arg)
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

void Street::check() const
{
	QStringList list = split(wholeName());
	if (wholeName() != join(list))
		qDebug() << "Unnormalized wholename" << wholeName() << list;
	if (name() != list[nfName])
		qDebug() << "Strange name" << wholeName() << name() << list[nfName];
	if (type() != list[nfType])
		qDebug() << "Strange type" << wholeName() << type() << list[nfType];
	if (number() != list[nfNumber])
		qDebug() << "Strange number" << wholeName() << number() << list[nfNumber];
//	if (secondary() != list[nfSecondary])
//		qDebug() << "Strange secondary" << wholeName() << secondary() << list[nfSecondary];

	QStringList keys = mDistricts.keys();
	foreach(QString key, keys) {
		District* d = mDistricts.value(key);
		if (!d)
			qDebug() << "NULL district key in street" << wholeName() << key;
		else {
			if (d->name() != key)
				qDebug() << "Wrong district key in street" <<
					    wholeName() << key << d->name();
			if (!d->streets().contains(wholeName()))
				qDebug() << "Missing district backref in street" <<
					    wholeName() << key;
		}
	}
}

void Street::joinName()
{
	setWholeName(join(nameList()));
}

void Street::parseName()
{
	QStringList list = split(wholeName());

	setType(list[nfType]);
	setNumber(list[nfNumber]);
	setName(list[nfName]);
	setWholeName(join(list));
}

void Street::processHouses()
{
	QRegExp houseNum("(вл\\s*)?(\\d*)(А|Б|В|Г|Д|Е|Ж|З|И|К)?(\\s*(к|с|стр)\\s*\\d*)*");
}

QStringList Street::nameList() const
{
	QStringList res;
	for(int i = 0; i < nfNumberOfFields; ++i)
		res.append(QString());
	res[nfName] = name();
	res[nfType] = type();
	res[nfSecondary] = QString();
	res[nfNumber] = number();

	return res;
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

//================ DISTRICT ==================

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

//================ REGION ==================

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
