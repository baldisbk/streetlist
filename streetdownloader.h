#ifndef STREETDOWNLOADER_H
#define STREETDOWNLOADER_H

#include <QObject>
#include <QtNetwork>

class Street;
class District;
class Region;

class StreetList : public QObject
{
	Q_OBJECT
	Q_ENUMS(RType)

public:
	enum RType {
		RTCity,
		RTRegion,
		RTDistrict,
		RTStreet
	};

	struct Request {
		RType type;
		QString name;
		QString url;
		bool started;
	};

	explicit StreetList(QObject *parent = 0);
	~StreetList();

	Q_INVOKABLE QStringList regions() const;
	Q_INVOKABLE QStringList districts() const;
	Q_INVOKABLE QStringList streets() const;
	
	Q_INVOKABLE void addStreet(const QVariantMap& props);
	Q_INVOKABLE void addRegion(const QVariantMap& props);
	Q_INVOKABLE void addDistrict(const QVariantMap& props);
	Q_INVOKABLE void addStreetToDistrict(const QVariantMap& props);

	Q_INVOKABLE Street* street(QString name) const;
	Q_INVOKABLE District* district(QString name) const;
	Q_INVOKABLE Region* region(QString name) const;

	Q_INVOKABLE QVariantMap streetV(QString name) const;
	Q_INVOKABLE QVariantMap districtV(QString name) const;
	Q_INVOKABLE QVariantMap regionV(QString name) const;

signals:
	void progress(int val, int max, RType type);
	void finished();
	void loadError(QString errorText);

public slots:
	void download();
	void loadFiles();
	void check() const;
	void clear();

private slots:
	void onReplyArrived(QNetworkReply*);
	void onReplyError(QNetworkReply::NetworkError code);

private:
	QList<Request*> parseRequest(QIODevice* input, Request *parent);
	QList<Request*> parseCity(QIODevice* input, Request *parent);
	QList<Request*> parseRegion(QIODevice* input, Request *parent);
	QList<Request*> parseDistrict(QIODevice* input, Request *parent);
	QList<Request*> parseStreet(QIODevice* input, Request *parent);

private:
	QMap<QString, Street*> mStreets;
	QMap<QString, District*> mDistricts;
	QMap<QString, Region*> mRegions;

	int mRegNum, mDistNum, mStreetNum;

	QNetworkAccessManager mManager;

	QMap<QString, Request*> mRequests;
	int mRequestCounter;
};

class Region : public QObject {
	Q_OBJECT
public:
	Region(QObject* parent = NULL);
	Region(const QString& name, QObject* parent = NULL);
	Region(const QVariantMap& props, QObject* parent = NULL);
	~Region();

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

	QString name() const;

	Q_INVOKABLE QStringList districts() const;
	Q_INVOKABLE void addDistrict(District* district);
	Q_INVOKABLE void removeDistrict(District* district);

public slots:
	void setName(QString arg);

	void check() const;

signals:
	void nameChanged(QString arg);

private:
	QString mName;
	QMap<QString, District*> mDistricts;
};

class Street : public QObject {
	Q_OBJECT
public:
	enum NameFields {
		nfName = 0,
		nfType,
		nfNumber,
		nfSecondary,

		nfNumberOfFields
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

	QString name() const;
	QString type() const;
	QString number() const;
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

	static QString normalize(QString name);
	static QStringList split(QString name);
	static QString join(QStringList names);

public slots:
	void setName(QString arg);
	void setType(QString arg);
	void setNumber(QString arg);
	void setWholeName(QString arg);

	void check() const;

	void setHouses(QString) {}

signals:
	void nameChanged(QString arg);
	void typeChanged(QString arg);
	void numberChanged(QString arg);
	void wholeNameChanged(QString arg);
	void housesChanged(QString arg);

private:
	void joinName();
	void parseName();
	void processHouses();
	QStringList nameList() const;

	static void initTypes();

private:
	static QStringList mStreetTypes;

	QStringList mHouses;
	QString mName;
	QString mType;
	QString mNumber;
	QString mWholeName;

	QMap<QString, District*> mDistricts;
};

class District : public QObject {
	Q_OBJECT
public:
	District(QObject* parent = NULL);
	District(const QString& name, QObject* parent = NULL);
	District(const QVariantMap& props, QObject* parent = NULL);
	~District();

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(Region* region READ region WRITE setRegion NOTIFY regionChanged)

	QString name() const;
	Region* region() const;

	Q_INVOKABLE QStringList streets() const;
	Q_INVOKABLE void addStreet(Street* street);
	Q_INVOKABLE void removeStreet(Street* street);

public slots:
	void setName(QString arg);
	void setRegion(Region* region);

	void check() const;

signals:
	void nameChanged(QString arg);
	void regionChanged(Region* arg);

private:
	QMap<QString, Street*> mStreets;
	QString mName;
	Region* mRegion;
};

#endif // STREETDOWNLOADER_H
