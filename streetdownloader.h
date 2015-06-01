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
		RTCity = 0,
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

	Q_INVOKABLE void processEvents() const;

signals:
	void progress(int val, int max, RType type);
	void finished();
	void loadError(QString errorText);

public slots:
	void download();
	void loadFiles(QString dirName);
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

#endif // STREETDOWNLOADER_H
