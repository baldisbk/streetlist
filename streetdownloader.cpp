#include "streetdownloader.h"

#include <QQmlEngine>
#include <QCoreApplication>

#include <QDebug>

#include "street.h"
#include "district.h"
#include "region.h"

//#define NO_HOUSES

StreetList::StreetList(QObject *parent) : QObject(parent)
{
	connect(&mManager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(onReplyArrived(QNetworkReply*)));
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
		res["houses"] = str->houses();
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
	const QNetworkReply* reply = dynamic_cast<const QNetworkReply*>(sender());
	if (reply)
		emit loadError(reply->errorString());
	else
		emit loadError("error " + QString::number(code));
}

QList<StreetList::Request *> StreetList::parseRequest(QIODevice *input, StreetList::Request *parent)
{
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
			newreq->name = StreetParser::normalize(re.cap(2));
			newreq->type = RTStreet;
			newreq->url = re.cap(1);

			Street* street;
			if (!mStreets.contains(newreq->name)) {
				street = new Street(newreq->name);
				QQmlEngine::setObjectOwnership(street, QQmlEngine::CppOwnership);
				mStreets.insert(street->wholeName(), street);
#ifndef NO_HOUSES
				requests.append(newreq);
				++mStreetNum;
#endif
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
	QRegExp re("<a href=\"http://address.mosopen.ru/.*\" title=\".*\">(.*)</a>");
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

void StreetList::loadFiles(QString dirName)
{
	clear();
	Request *req = new Request;
	req->type = RTCity;
	req->url = "main";
	mRequests.insert(req->url, req);
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
		QString name = dirName + '/' + req->url.section("/", section, section) + ".html";
		QFile file(name);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			emit loadError(QString("Error opening %1: %2").
				arg(name).
				arg(file.errorString()));
			mRequests.remove(req->url);
			delete req;
			continue;
		}
		parseRequest(&file, req);
		QCoreApplication::processEvents();
	}
	emit finished();
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
