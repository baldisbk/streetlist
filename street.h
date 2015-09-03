#ifndef STREET_H
#define STREET_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QStringList>
#include <QVariant>
#include <QGeoCoordinate>

class Region;
class District;
class StreetList;

enum NameFields {
	nfName = 0,
	nfType,
	nfNumber,
	nfSecondary,

	nfNumberOfFields
};

struct ElemNumber {
	unsigned short num;
	unsigned short liter;
	ElemNumber(): num(0), liter(0) {}
	QString toString() const;
	bool isNull() const;
};

struct Number {
	ElemNumber first;
	ElemNumber second;
	QString toString() const;
	bool isNull() const;
};

struct ElemHouse {
	Number main;
	Number own;
	Number cor;
	Number bld;
	QString toString() const;
	bool isNull() const;
};

struct House {
	QList<ElemHouse> numbers;
	QString origin;
	QString toString() const;
};

class StreetParser {
public:
	static QString normalize(QString name);
	static QStringList split(QString name);
	static QString join(QStringList names);
	static QList<House> splitHouses(QString houses);
	static QString canonical(QStringList names);
	static QString wikiRequest(QStringList names);
	static int letterNumber(QChar letter);

private:
	enum Gender {
		Male,
		Female,
		Other,
		Unknown
	};

	static void init();

	static QString numPostfix(int num, Gender gnd);
	static Gender genderForWord(QString word);

	static ElemNumber parseElemNumber(int& pos, const QString& name);
	static Number parseNumber(int& pos, const QString& name);
	static ElemHouse parseElemHouse(int& pos, const QString& name);

	static QStringList mStreetTypes;
	static QStringList mStreetSecondaries;
	static QStringList mPostfixes;
	static QStringList mUAlpha;
	static QStringList mLAlpha;
	static QMap<int, QString> mNumerics;
	static QMap<QString, Gender> mGenders;
};

class Street : public QObject {
	Q_OBJECT
	Q_ENUMS(SourceType)

public:
	enum SourceType {
		stNone = 0,
		stWiki,
		stGeoCode,
		stUser
	};

	Street(QObject* parent = NULL);
	Street(const QString& name, QObject* parent = NULL);
	Street(const QVariantMap& props, QObject* parent = NULL);
	~Street();

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
	Q_PROPERTY(QString number READ number WRITE setNumber NOTIFY numberChanged)
	Q_PROPERTY(QString wholeName READ wholeName WRITE setWholeName NOTIFY wholeNameChanged)
	Q_PROPERTY(QString houses READ houses WRITE setHouses NOTIFY housesChanged)
	Q_PROPERTY(QString secondary READ secondary WRITE setSecondary NOTIFY secondaryChanged)
	Q_PROPERTY(QGeoCoordinate coordinates READ coordinates WRITE setCoordinates NOTIFY coordinatesChanged)
	Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
	Q_PROPERTY(QString canonical READ canonical)

	QString name() const;
	QString type() const;
	QString number() const;
	QString secondary() const;
	QString wholeName() const;
	QString houses() const;
	QString canonical() const;
	QString wikiRequest() const;
	QGeoCoordinate coordinates() const;
	QString description() const;

	int letterNumber(int index) const;

	Q_INVOKABLE void addHouse(const QString& house);
	Q_INVOKABLE QStringList houseList() const;

	Q_INVOKABLE QStringList districts() const;
	Q_INVOKABLE void addDistrict(District* district);
	Q_INVOKABLE void removeDistrict(District* district);

	Q_INVOKABLE SourceType descSrc() const;
	Q_INVOKABLE void setDescSrc(const SourceType &descSrc);
	Q_INVOKABLE SourceType coordSrc() const;
	Q_INVOKABLE void setCoordSrc(const SourceType &coordSrc);
	Q_INVOKABLE int version() const;

public slots:
	void setName(QString arg);
	void setType(QString arg);
	void setNumber(QString arg);
	void setSecondary(QString arg);
	void setWholeName(QString arg);
	void setHouses(QString) {}
	void setCoordinates(QGeoCoordinate coordinates);
	void setDescription(QString description);

	void check() const;

signals:
	void nameChanged(QString arg);
	void typeChanged(QString arg);
	void numberChanged(QString arg);
	void wholeNameChanged(QString arg);
	void housesChanged(QString arg);
	void secondaryChanged(QString arg);
	void coordinatesChanged(QGeoCoordinate coordinates);

	void descriptionChanged(QString description);

private:
	void joinName();
	void parseName();
	void processHouses();
	QStringList nameList() const;

private:
	QList<House> mHouses;
	QString mName;
	QString mType;
	QString mNumber;
	QString mSecondary;
	QString mWholeName;
	QGeoCoordinate mCoordinates;
	QString mDescription;
	SourceType mDescSrc, mCoordSrc;

	QMap<QString, District*> mDistricts;
};

#endif // STREET_H
