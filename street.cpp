#include "street.h"

#include <QDebug>

#include "district.h"

QList<QRegExp> StreetParser::mStreetTypes;
QStringList StreetParser::mStreetSecondaries;
QStringList StreetParser::mPostfixes;
QStringList StreetParser::mUAlpha;
QStringList StreetParser::mLAlpha;

void StreetParser::init()
{
	if (mStreetTypes.isEmpty()) {
		QStringList types;
		types
			<< "улица"
			<< "квартал"
			<< "проспект"
			<< "шоссе"
			<< "проезд"
			<< "переулок"
			<< "бульвар"
			<< "площадь"
			<< "набережная"
			<< "аллея"
			<< "мост"
			<< "тупик"
			<< "парк"
			<< "линия"
			<< "просека"
			<< "микрорайон"
			<< "посёлок"
			<< "деревня"
			;
		mStreetSecondaries
			<< "Нижн"
			<< "Верхн"
			<< "Средн"
			<< "Больш"
			<< "Мал"
			;
		mPostfixes
			<< "ий"
			<< "ый"
			<< "й"
			<< "ая"
			<< "яя"
			<< "я"
			<< "ое"
			<< "ее"
			<< "ье"
			<< "е"
			<< "ие"
			<< "ые"
			;
		mUAlpha
			<< "А" << "Б" << "В" << "Г" << "Д" << "Е" << "Ё" << "Ж"
			<< "З" << "И" << "Й" << "К" << "Л" << "М" << "Н" << "О"
			<< "П" << "Р" << "С" << "Т" << "У" << "Ф" << "Х" << "Ц"
			<< "Ч" << "Ш" << "Щ" << "Ъ" << "Ы" << "Ь" << "Э" << "Ю"
			<< "Я";
		mLAlpha
			<< "а" << "б" << "в" << "г" << "д" << "е" << "ё" << "ж"
			<< "з" << "и" << "й" << "к" << "л" << "м" << "н" << "о"
			<< "п" << "р" << "с" << "т" << "у" << "ф" << "х" << "ц"
			<< "ч" << "ш" << "щ" << "ъ" << "ы" << "ь" << "э" << "ю"
			<< "я";
		foreach (QString type, types) {
			QRegExp re(QString("\\b%1\\b").arg(type));
			re.setCaseSensitivity(Qt::CaseInsensitive);
			mStreetTypes.append(re);
		}
	}
}

QString StreetParser::normalize(QString name)
{
	init();
	return join(split(name));
}

QStringList StreetParser::split(QString name)
{
	init();
	QStringList res;
	for(int i = 0; i < nfNumberOfFields; ++i)
		res.append(QString());

	QRegExp captureNum1("N\\s(\\d+)");
	QString numRE = QString("\\b(\\d+)((%1)|-(%2))?\\b").
		arg(mLAlpha.join("|")).
		arg(mPostfixes.join("|"));
	QRegExp captureNum2(numRE);

	QString n = name;
	n.remove(",");
	if (captureNum1.indexIn(n) != -1) {
		res[nfNumber] = captureNum1.cap(1);
		n.remove(captureNum1);
	}
	if (captureNum2.indexIn(n) != -1) {
		res[nfNumber] = captureNum2.cap(1) + captureNum2.cap(3);
		n.remove(captureNum2);
	}
	foreach(QRegExp re, mStreetTypes)
		if (re.indexIn(n) != -1) {
			res[nfType] = re.cap(0).toLower();
			n.remove(re);
			break;
		}

	res[nfName] = n.simplified();

	return res;
}

QString StreetParser::join(QStringList names)
{
	init();
	QStringList out;
	out << names[nfNumber] << names[nfSecondary]
	       << names[nfName] << names[nfType];
	return names.join(' ').simplified();
}

QStringList StreetParser::splitHouses(QString houses)
{
	QStringList toParse = houses.simplified().remove("&nbsp;").split(',');
	QStringList res;
	QRegExp own("вл");
	QRegExp cor("к(\\d+)");
	QRegExp bld("(с|стр)\\d+)");
	QRegExp num(QString("(\\d+)(%1)?(/(\\d+)(%1)?)?").arg(mUAlpha.join('|')));

	QString numberRe = QString("(\\d+)(%1)?").arg(mUAlpha.join('|'));
	QRegExp total(QString("(вл)?(%1)(/%1)?((к|с|стр)%1)*").arg(numberRe));
	foreach(QString p, toParse) {
		QString house = p;


		if (total.indexIn(house)) {
			house.remove(total);
			if (!house.isEmpty())
				qDebug() << "something left" << house << p;
		} else
			qDebug() << "dont match" << house;
		continue;


		bool isOwn = own.indexIn(house);
		house.remove(own);

		int corNo = -1;
		if (cor.indexIn(house)) {
			corNo = cor.cap(1).toInt();
			house.remove(cor);
		}
		int bldNo = -1;
		if (bld.indexIn(house)) {
			bldNo = bld.cap(2).toInt();
			house.remove(bld);
		}
		int mainNum = -1;
		int secNum = -1;
		QString lit1, lit2;
		if (num.indexIn(house)) {
			mainNum = num.cap(1).toInt();
			lit1 = num.cap(2);
			QString sec = num.cap(3);
			if (!sec.isEmpty()) {
				secNum = num.cap(4).toInt();
				lit2 = num.cap(5);
			}
		} else
			qDebug() << "very strange num" << p << house;
		house.remove(num);
		if (!house.isEmpty())
			qDebug() << "strange num" << p << house;
	}

	return res;
}


Street::Street(QObject *parent): QObject(parent)
{
}

Street::Street(const QString &name, QObject *parent): QObject(parent), mWholeName(name)
{
	parseName();
	joinName();
}

Street::Street(const QVariantMap &props, QObject *parent): QObject(parent)
{
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
	StreetParser::splitHouses(house);
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

QString Street::secondary() const
{
	return mSecondary;
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
	QStringList list = StreetParser::split(wholeName());
	if (wholeName() != StreetParser::join(list))
		qDebug() << "Unnormalized wholename" << wholeName() << list;
	if (name() != list[nfName])
		qDebug() << "Strange name" << wholeName() << name() << list[nfName];
	if (type() != list[nfType])
		qDebug() << "Strange type" << wholeName() << type() << list[nfType];
	if (number() != list[nfNumber])
		qDebug() << "Strange number" << wholeName() << number() << list[nfNumber];
	if (secondary() != list[nfSecondary])
		qDebug() << "Strange secondary" << wholeName() << secondary() << list[nfSecondary];

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

void Street::setSecondary(QString arg)
{
	if (mSecondary == arg)
		return;

	mSecondary = arg;
	emit secondaryChanged(arg);
	joinName();
}

void Street::joinName()
{
	QString whole = StreetParser::join(nameList());
	if (mWholeName != whole) {
		mWholeName = whole;
		emit wholeNameChanged(whole);
	}
}

void Street::parseName()
{
	QStringList list = StreetParser::split(wholeName());

	if (mType != list[nfType]) {
		mType = list[nfType];
		emit secondaryChanged(mType);
	}
	if (mNumber != list[nfNumber]) {
		mNumber = list[nfNumber];
		emit secondaryChanged(mNumber);
	}
	if (mName != list[nfName]) {
		mName = list[nfName];
		emit secondaryChanged(mName);
	}
	if (mSecondary != list[nfSecondary]) {
		mSecondary = list[nfSecondary];
		emit secondaryChanged(mSecondary);
	}
}

void Street::processHouses()
{

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
