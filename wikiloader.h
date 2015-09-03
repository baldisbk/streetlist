#ifndef WIKILOADER_H
#define WIKILOADER_H

#include <QObject>
#include <QtNetwork>
#include <QGeoCoordinate>

class Street;

class WikiLoader : public QObject
{
	Q_OBJECT
public:
	explicit WikiLoader(QObject *parent = 0);
	~WikiLoader();

	Q_PROPERTY(Street* street READ street WRITE setStreet NOTIFY streetChanged)
	Q_PROPERTY(QString html READ html WRITE setHtml NOTIFY htmlChanged)
	Q_PROPERTY(QGeoCoordinate coordinates READ coordinates WRITE setCoordinates NOTIFY coordinatesChanged)

	Street* street() const;
	QString html() const;
	QGeoCoordinate coordinates() const;

signals:
	void streetChanged(Street* arg);
	void htmlChanged(QString arg);
	void coordinatesChanged(QGeoCoordinate coordinates);

public slots:
	void setStreet(Street* arg);
	void setHtml(QString arg);
	void setCoordinates(QGeoCoordinate coordinates);

private slots:
	void onReplyArrived(QNetworkReply*reply);
	void onReplyError(QNetworkReply::NetworkError);

private:
	QStringList processLine(QString &str);
	void request();

	QNetworkAccessManager mManager;
	bool mRetry;

	Street* mStreet;
	QString mHtml;
	QGeoCoordinate mCoord;
};

#endif // WIKILOADER_H
