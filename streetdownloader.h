#ifndef STREETDOWNLOADER_H
#define STREETDOWNLOADER_H

#include <QObject>
#include <QtNetwork>

class Street;
class District;
class Region;

class StreetDownloader : public QObject
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

	explicit StreetDownloader(QObject *parent = 0);
	~StreetDownloader();

	Q_INVOKABLE QStringList regions() const;
	Q_INVOKABLE QStringList districts() const;
	Q_INVOKABLE QStringList streets() const;
	
	Q_INVOKABLE Street* street(QString name);
	Q_INVOKABLE District* district(QString name);
	Q_INVOKABLE Region* region(QString name);

signals:
	void progress(int val, int max, RType type);
	void finished();
	void loadError(QString errorText);

public slots:
	void loadCity();

private slots:
	void onReplyArrived(QNetworkReply*);
	void onReplyError(QNetworkReply::NetworkError code);

private:
	QMap<QString, Street*> mStreets;
	QMap<QString, District*> mDistricts;
	QMap<QString, Region*> mRegions;

	int mRegNum, mDistNum, mStreetNum;

	QNetworkAccessManager mManager;

	struct Request {
		RType type;
		QString name;
	};

	QMap<QString, Request> mRequests;
};

class Region : public QObject {
	Q_OBJECT
public:
	Region(QObject* parent = NULL);
	Region(const QString& name, QObject* parent = NULL);

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

	QString name() const;
	
	Q_INVOKABLE void addDistrict(District* district);
	Q_INVOKABLE QStringList districts() const;

public slots:
	void setName(QString arg);

signals:
	void nameChanged(QString arg);

private:
	QString mName;
	QMap<QString, District*> mDistricts;
};

class Street : public QObject {
	Q_OBJECT
public:
	Street(QObject* parent = NULL);
	Street(const QString& name, QObject* parent = NULL);

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
	Q_PROPERTY(int number READ number WRITE setNumber NOTIFY numberChanged)

	QString name() const;
	QString type() const;
	int number() const;

	Q_INVOKABLE QStringList houses() const;
	Q_INVOKABLE QString housesStr() const;
	Q_INVOKABLE void addHouse(const QString& house);

//	Q_INVOKABLE int parseNumber(QString house) const;
//	Q_INVOKABLE QString parseLiter(QString house) const;
//	Q_INVOKABLE int parseBuilding(QString house) const;
//	Q_INVOKABLE QString parseCorpse(QString house) const;
//	Q_INVOKABLE int parseCorner(QString house) const;
//	Q_INVOKABLE bool parseIsProperty(QString house) const;

	Q_INVOKABLE void parse();

public slots:
	void setName(QString arg);
	void setType(QString arg);
	void setNumber(int arg);

signals:
	void nameChanged(QString arg);
	void typeChanged(QString arg);
	void numberChanged(int arg);

private:
	static QStringList mStreetTypes;

	QStringList mHouses;
	QString mName;
	QString mType;
	int mNumber;
};

class District : public QObject {
	Q_OBJECT
public:
	District(QObject* parent = NULL);
	District(const QString& name, QObject* parent = NULL);

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(Region* region READ region WRITE setRegion NOTIFY regionChanged)

	QString name() const;
	Region* region() const;

	Q_INVOKABLE void addStreet(Street* street);
	Q_INVOKABLE QStringList streets() const;

public slots:
	void setName(QString arg);
	void setRegion(Region* arg);

signals:
	void nameChanged(QString arg);
	void regionChanged(Region* arg);

private:
	QMap<QString, Street*> mStreets;
	QString mName;
	Region* mRegion;
};

#endif // STREETDOWNLOADER_H
