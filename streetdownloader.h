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

	explicit StreetList(QObject *parent = 0);
	~StreetList();

	// common interface
	Q_INVOKABLE QStringList regions() const;
	Q_INVOKABLE QStringList districts() const;
	Q_INVOKABLE QStringList streets() const;
	
	Q_INVOKABLE void addStreet(const QVariantMap& props);
	Q_INVOKABLE void addRegion(const QVariantMap& props);
	Q_INVOKABLE void addDistrict(const QVariantMap& props);
	Q_INVOKABLE void addStreetToDistrict(const QVariantMap& props);

	// QML interface
	Q_INVOKABLE QVariantMap street(QString name) const;
	Q_INVOKABLE QVariantMap district(QString name) const;
	Q_INVOKABLE QVariantMap region(QString name) const;

	// C++ interface
	Street* streetPtr(QString name) const;
	District* districtPtr(QString name) const;
	Region* regionPtr(QString name) const;

signals:
	void progress(int val, int max, RType type);
	void finished();
	void loadError(QString errorText);

public slots:
	void download();

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
	Region(const QVariantMap& props, QObject* parent = NULL);

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

	QString name() const;
	
	void addDistrict(QString district);
	QStringList districts() const;

public slots:
	void setName(QString arg);

signals:
	void nameChanged(QString arg);

private:
	QString mName;
	QStringList mDistricts;
};

class Street : public QObject {
	Q_OBJECT
public:
	Street(QObject* parent = NULL);
	Street(const QString& name, QObject* parent = NULL);
	Street(const QVariantMap& props, QObject* parent = NULL);

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
	Q_PROPERTY(int number READ number WRITE setNumber NOTIFY numberChanged)
	Q_PROPERTY(QString wholeName READ wholeName WRITE setWholeName NOTIFY wholeNameChanged)

	QString name() const;
	QString type() const;
	int number() const;
	QString wholeName() const;

	QStringList houses() const;
	void addHouse(const QString& house);

//	Q_INVOKABLE int parseNumber(QString house) const;
//	Q_INVOKABLE QString parseLiter(QString house) const;
//	Q_INVOKABLE int parseBuilding(QString house) const;
//	Q_INVOKABLE QString parseCorpse(QString house) const;
//	Q_INVOKABLE int parseCorner(QString house) const;
//	Q_INVOKABLE bool parseIsProperty(QString house) const;

public slots:
	void setName(QString arg);
	void setType(QString arg);
	void setNumber(int arg);
	void setWholeName(QString arg);

signals:
	void nameChanged(QString arg);
	void typeChanged(QString arg);
	void numberChanged(int arg);
	void wholeNameChanged(QString arg);

private:
	void joinName();
	void parseName();
	void processHouses();

	static void initTypes();

private:
	static QStringList mStreetTypes;

	QStringList mHouses;
	QString mName;
	QString mType;
	int mNumber;
	QString mWholeName;
};

class District : public QObject {
	Q_OBJECT
public:
	District(QObject* parent = NULL);
	District(const QString& name, QObject* parent = NULL);
	District(const QVariantMap& props, QObject* parent = NULL);

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString region READ region WRITE setRegion NOTIFY regionChanged)

	QString name() const;
	QString region() const;

	void addStreet(QString street);
	QStringList streets() const;

public slots:
	void setName(QString arg);
	void setRegion(QString arg);

signals:
	void nameChanged(QString arg);
	void regionChanged(QString arg);

private:
	QStringList mStreets;
	QString mName;
	QString mRegion;
};

#endif // STREETDOWNLOADER_H
