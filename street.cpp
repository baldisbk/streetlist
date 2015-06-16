#include "street.h"

#include <QDebug>

#include "district.h"

#define MAX_NUMERIC 19
//#define PARSE_HOUSES

QStringList StreetParser::mStreetTypes;
QStringList StreetParser::mStreetSecondaries;
QStringList StreetParser::mPostfixes;
QStringList StreetParser::mUAlpha;
QStringList StreetParser::mLAlpha;
QMap<int, QString> StreetParser::mNumerics;
QMap<QString, StreetParser::Gender> StreetParser::mGenders;

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
			<< "Центральн"
			;
		mPostfixes
			<< "ий"
			<< "ый"
			<< "ой"
			<< "й"
			<< "ая"
			<< "яя"
			<< "ья"
			<< "я"
			<< "ое"
			<< "ее"
			<< "ье"
			<< "е"
//			<< "ие"
//			<< "ые"
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
		//	QRegExp re(QString("\\b%1\\b").arg(type));
		//	re.setCaseSensitivity(Qt::CaseInsensitive);
			mStreetTypes.append(type);
		}

		mNumerics.insert(1, "перв");
		mNumerics.insert(2, "втор");
		mNumerics.insert(3, "трет");
		mNumerics.insert(4, "четверт");
		mNumerics.insert(5, "пят");
		mNumerics.insert(6, "шест");
		mNumerics.insert(7, "седьм");
		mNumerics.insert(8, "восьм");
		mNumerics.insert(9, "девят");
		mNumerics.insert(10, "десят");
		mNumerics.insert(11, "одиннадцат");
		mNumerics.insert(12, "двенадцат");
		mNumerics.insert(13, "тринадцат");
		mNumerics.insert(14, "четырнадцат");
		mNumerics.insert(15, "пятнадцат");
		mNumerics.insert(16, "шестнадцат");
		mNumerics.insert(17, "семнадцат");
		mNumerics.insert(18, "восемнадцат");
		mNumerics.insert(19, "девятнадцат");

		foreach(QString type, types) {
			QChar last = type.at(type.size() - 1);
			if (last == L'е')
				mGenders[type] = Other;
			else if (last == L'а')
				mGenders[type] = Female;
			else if (last == L'я')
				mGenders[type] = Female;
			else if (last == L'ь')
				mGenders[type] = Female;
			else
				mGenders[type] = Male;
		}
	}
}

QString StreetParser::numeric(int num, Gender gnd)
{
	int actnum = num%20;
	if (actnum != num || actnum == 0)
		return QString();

	int endIndex;
	switch (gnd) {
	case Male:
		if (actnum == 3)
			endIndex = 0;
		else if (actnum == 2 || actnum == 6 || actnum == 7 || actnum == 8)
			endIndex = 2;
		else
			endIndex = 1;
		break;
	case Female:
		if (actnum == 3)
			endIndex = 6;
		else
			endIndex = 4;
		break;
	case Other:
		if (actnum == 3)
			endIndex = 10;
		else
			endIndex = 8;
		break;
	}
	return mNumerics[actnum] + mPostfixes[endIndex];
}

ElemNumber StreetParser::parseElemNumber(int &pos, const QString &name)
{
	QRegExp re(QString("(\\d+)|(%1)").arg(mUAlpha.join("|")));
	ElemNumber res;
	while(re.indexIn(name, pos) == pos) {
		QString cap = re.cap(0);
		if (mUAlpha.contains(cap)) {
			if (res.liter != 0)
				break;
			res.liter = cap.at(0).unicode();
		} else {
			if (res.num != 0)
				break;
			res.num = cap.toInt();
		}
		pos += cap.size();
	}
	return res;
}

Number StreetParser::parseNumber(int &pos, const QString &name)
{
	Number num;
	num.first = parseElemNumber(pos, name);
	if (num.first.isNull())
		return num;
	if (pos >= name.size())
		return num;
	if (name.at(pos) == QChar('-'))
		++pos;
	ElemNumber en = parseElemNumber(pos, name);
	if (!en.isNull())
		num.second = en;
	return num;
}

enum AutomataStates {
	Start,		// Own(own), HasNumber(num)
	Own,
	Corpse,
	Build
};

ElemHouse StreetParser::parseElemHouse(int &pos, const QString &name)
{
	QString own("вл");
	QString bld("с");
	QString cor("к");
	QRegExp re(QString("%1|%2|%3").arg(own).arg(cor).arg(bld));
	ElemHouse eh;
	AutomataStates state = Start;
	while(true) {
		if (re.indexIn(name, pos) == pos) {
			QString cap = re.cap(0);
			pos += cap.size();
			if (cap == own) state = Own;
			else if (cap == bld) state = Build;
			else if (cap == cor) state = Corpse;
		}
		Number num = parseNumber(pos, name);
		if (num.isNull()) {
			if (state != Start)
				qDebug() << "Error N" << state << name << pos;
			return eh;
		}
		switch (state) {
		case Start:
			if (!eh.main.isNull())
				qDebug() << "Error DM" << name;
			eh.main = num; break;
		case Own:
			if (!eh.own.isNull())
				qDebug() << "Error DO" << name;
			eh.own = num; break;
		case Build:
			if (!eh.bld.isNull())
				qDebug() << "Error DB" << name;
			eh.bld = num; break;
		case Corpse:
			if (!eh.cor.isNull())
				qDebug() << "Error DC" << name;
			eh.cor = num; break;
		}
		state = Start;
		if (pos >= name.size())
			return eh;
	}
	return eh;
}

QString StreetParser::normalize(QString name)
{
	init();
	QStringList r = split(name);
	QString s = join(r);
	return s;
	return join(split(name));
}

QStringList StreetParser::split(QString name)
{
	init();
	QStringList res;
	for(int i = 0; i < nfNumberOfFields; ++i)
		res.append(QString());

	QRegExp captureNum1(QString("N\\s(\\d+(%1)?)").arg(mLAlpha.join("|")));
	QString numRE = QString("(\\d+)((%1)|-(%2))").
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

	foreach(QString type, mStreetTypes) {
		QRegExp re(QString("\\b%1\\b").arg(type));
		if (re.indexIn(n) != -1) {
			res[nfType] = re.cap(0).toLower();
			n.remove(re);
			break;
		}
	}

	n = n.simplified();

	QRegExp captureSec(QString("\\b(%1)(%2)\\b").
		arg(mStreetSecondaries.join("|")).arg(mPostfixes.join("|")));
	if (captureSec.indexIn(n) != -1) {
		QString cap = captureSec.cap(0);
		if (cap != n) {
			n.remove(cap);
			res[nfSecondary] = cap;
		}
	}

	res[nfName] = n.simplified();

	return res;
}

QString StreetParser::join(QStringList names)
{
	init();
	QStringList out;
	out << (names[nfNumber].isEmpty()?"":("N "+names[nfNumber]))
	    << names[nfSecondary] << names[nfName] << names[nfType];
	return out.join(' ').simplified();
}

QList<House> StreetParser::splitHouses(QString houses)
{
	QStringList toParse = houses.simplified().remove("&nbsp;").split(',');
	QList<House> res;

//	QString numberRe = QString("(\\d*)(-\\d*)?(%1)?").arg(mUAlpha.join('|'));
//	QRegExp total(QString("((|/|вл|к|с)%1)*").arg(numberRe));

	foreach(QString p, toParse) {
		House house;
		house.origin = p;
#ifdef PARSE_HOUSES
		int pos = 0;
		while(true) {
			ElemHouse eh = parseElemHouse(pos, p);
			if (eh.isNull())
				qDebug() << "Error HN" << p;
			else
				house.numbers.append(eh);
			if (pos < p.size()) {
				if (p.at(pos) == QChar('/'))
					++pos;
				else {
					qDebug() << "Error U" << p << pos << house.toString();
					break;
				}
			} else
				break;
		}
#endif
		res.append(house);
	}

	return res;
}

QString StreetParser::canonical(QStringList names)
{
	QStringList res;
	QString num;
	if (!names.at(nfNumber).isEmpty()) {
		bool ok;
		int n = names.at(nfNumber).toInt(&ok);
		if (ok && n <= MAX_NUMERIC)
			res << numeric(n, mGenders.value(names.at(nfType)));
		else
			num = names.at(nfNumber);
	}
	if (!names.at(nfSecondary).isEmpty())
		res << names.at(nfSecondary);
	res << names.at(nfName);
	if (!num.isEmpty())
		res << num;
	return res.join(" ").toLower();
}

int StreetParser::letterNumber(QChar letter)
{
	return mLAlpha.indexOf(letter) + 1;
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
		mNumber = props.value("number").toString();
		mSecondary = props.value("sec").toString();
		mHouses = StreetParser::splitHouses(props.value("houses").toString());
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
	QStringList houses;
	foreach(House h, mHouses)
		houses.append(h.toString());
	return houses.join(',');
}

int Street::letterNumber(int index) const
{
	QString canonical = StreetParser::canonical(nameList()).remove(" ");
	if (index <= 0 || index > canonical.size())
		return 0;
	return StreetParser::letterNumber(canonical.toLower().at(index - 1));
}

void Street::addHouse(const QString &house)
{
	QList<House> list = StreetParser::splitHouses(house);
	mHouses.append(list);
}

QStringList Street::houseList() const
{
	QStringList houses;
	foreach(House h, mHouses)
		houses.append(h.toString());
	return houses;
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
		emit typeChanged(mType);
	}
	if (mNumber != list[nfNumber]) {
		mNumber = list[nfNumber];
		emit numberChanged(mNumber);
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
	res[nfSecondary] = secondary();
	res[nfNumber] = number();

	return res;
}


QString House::toString() const
{
#ifdef PARSE_HOUSES
	QStringList res;
	foreach(const ElemHouse& eh, numbers)
		res.append(eh.toString());
	return res.join('/');
#else
	return origin;
#endif
}

QString ElemHouse::toString() const
{
	QString res = main.toString();
	if (!own.isNull())
		res += "вл" + own.toString();
	if (!cor.isNull())
		res += "к" + cor.toString();
	if (!bld.isNull())
		res += "с" + bld.toString();
	return res;
}

bool ElemHouse::isNull() const
{
	return main.isNull() && own.isNull() && bld.isNull() && cor.isNull();
}

QString ElemNumber::toString() const
{
	if (isNull())
		return QString();
	return (num==0?"":QString::number(num)) + (liter==0?"":QString(QChar(liter)));
}

bool ElemNumber::isNull() const
{
	return liter == 0 && num == 0;
}

QString Number::toString() const
{
	return first.toString() + (second.isNull()?"":("-"+second.toString()));
}

bool Number::isNull() const
{
	return first.isNull();
}
