#ifndef STREET_H
#define STREET_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QStringList>
#include <QVariant>

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

class StreetParser {
public:
	static QString normalize(QString name);
	static QStringList split(QString name);
	static QString join(QStringList names);
	static QStringList splitHouses(QString houses);

private:
	static void init();

	static QList<QRegExp> mStreetTypes;
	static QStringList mStreetSecondaries;
	static QStringList mPostfixes;
	static QStringList mUAlpha;
	static QStringList mLAlpha;
};

class Street : public QObject {
	Q_OBJECT
public:
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

	QString name() const;
	QString type() const;
	QString number() const;
	QString secondary() const;
	QString wholeName() const;
	QString houses() const;

	Q_INVOKABLE void addHouse(const QString& house);
	Q_INVOKABLE QStringList houseList() const;

	Q_INVOKABLE QStringList districts() const;
	Q_INVOKABLE void addDistrict(District* district);
	Q_INVOKABLE void removeDistrict(District* district);

//	Q_INVOKABLE int parseNumber(QString house) const;
//	Q_INVOKABLE QString parseLiter(QString house) const;
//	Q_INVOKABLE int parseBuilding(QString house) const;
//	Q_INVOKABLE QString parseCorpse(QString house) const;
//	Q_INVOKABLE int parseCorner(QString house) const;
//	Q_INVOKABLE bool parseIsProperty(QString house) const;

public slots:
	void setName(QString arg);
	void setType(QString arg);
	void setNumber(QString arg);
	void setSecondary(QString arg);
	void setWholeName(QString arg);

	void check() const;

	void setHouses(QString) {}

signals:
	void nameChanged(QString arg);
	void typeChanged(QString arg);
	void numberChanged(QString arg);
	void wholeNameChanged(QString arg);
	void housesChanged(QString arg);

	void secondaryChanged(QString arg);

private:
	void joinName();
	void parseName();
	void processHouses();
	QStringList nameList() const;

private:
	QStringList mHouses;
	QString mName;
	QString mType;
	QString mNumber;
	QString mSecondary;
	QString mWholeName;

	QMap<QString, District*> mDistricts;
};

#endif // STREET_H
