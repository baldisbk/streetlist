#include "streetdownloader.h"

#include <QDebug>

StreetDownloader::StreetDownloader(QObject *parent) : QObject(parent)
{
	mStreets.insert("1-я Какая-нить улица", new Street("1-я Какая-нить улица"));
}

StreetDownloader::~StreetDownloader()
{
	foreach(District* d, mDistricts)
		delete d;
	foreach(Street* s, mStreets)
		delete s;
}

QStringList StreetDownloader::regions() const
{
	return mRegions.keys();
}

QStringList StreetDownloader::districts() const
{
	return mDistricts.keys();
}

QStringList StreetDownloader::streets() const
{
	return mStreets.keys();
}

Street *StreetDownloader::street(QString name)
{
	return mStreets.value(name, NULL);
}

District *StreetDownloader::district(QString name)
{
	return mDistricts.value(name, NULL);
}

Region *StreetDownloader::region(QString name)
{
	return mRegions.value(name, NULL);
}

void StreetDownloader::onReplyArrived(QNetworkReply *reply)
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
				dist->setRegion(mRegions[req.name]);
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
				mDistricts[req.name]->addStreet(street);
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

void StreetDownloader::onReplyError(QNetworkReply::NetworkError code)
{
	qDebug() << "error" << code;
}

void StreetDownloader::loadCity()
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
}

Street::Street(const QString &name, QObject *parent): QObject(parent), mName(name)
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
	parse();
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

QString Street::housesStr() const
{
	return mHouses.join(',');
}

void Street::addHouse(const QString &house)
{
	mHouses.append(house);
}

void Street::setName(QString arg)
{
	if (mName == arg)
		return;

	mName = arg;
	parse();
	emit nameChanged(arg);
}

void Street::setType(QString arg)
{
	if (mType == arg)
		return;

	mType = arg;
	emit typeChanged(arg);
}

void Street::setNumber(int arg)
{
	if (mNumber == arg)
		return;

	mNumber = arg;
	emit numberChanged(arg);
}

void Street::parse()
{
	QRegExp captureType(mStreetTypes.join("|"));
	captureType.setCaseSensitivity(Qt::CaseInsensitive);
	QRegExp captureNum("(N\\s)?(\\d+)(а|б|в|г|д|е|-(й|ый|ой|ий|я|ая|яя|е|ое|ье))?");
	QRegExp houseNum("(вл\\s*)?(\\d*)(А|Б|В|Г|Д|Е|Ж|З|И|К)?(\\s*(к|с|стр)\\s*\\d*)*");

	if (captureType.indexIn(mName) != -1) {
		mType = captureType.cap(0).toLower();
		mName.remove(captureType);
	}
	if (captureNum.indexIn(mName) != -1) {
		mNumber = captureNum.cap(2).toInt();
		mName.remove(captureNum);
	}
	mName = mName.simplified();
}


District::District(QObject *parent): QObject(parent)
{
}

District::District(const QString &name, QObject *parent): QObject(parent), mName(name)
{
}

QString District::name() const
{
	return mName;
}

Region *District::region() const
{
	return mRegion;
}

void District::addStreet(Street *street)
{
	mStreets[street->name()] = street;
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

void District::setRegion(Region *arg)
{
	if (mRegion == arg)
		return;

	mRegion = arg;
	if (!mRegion->districts().contains(name()))
		mRegion->addDistrict(this);

	emit regionChanged(arg);
}


Region::Region(QObject *parent): QObject(parent)
{
}

Region::Region(const QString &name, QObject *parent): QObject(parent), mName(name)
{
}

QString Region::name() const
{
	return mName;
}

void Region::addDistrict(District *district)
{
	mDistricts[district->name()] = district;
	if (district->region() != this)
		district->setRegion(this);
}

QStringList Region::districts() const
{
	return mDistricts.keys();
}

void Region::setName(QString arg)
{
	if (mName == arg)
		return;

	mName = arg;
	emit nameChanged(arg);
}
