#ifndef WIKILOADER_H
#define WIKILOADER_H

#include <QObject>
#include <QtNetwork>

class Street;

class WikiLoader : public QObject
{
	Q_OBJECT
public:
	explicit WikiLoader(QObject *parent = 0);
	~WikiLoader();

	Q_PROPERTY(Street* street READ street WRITE setStreet NOTIFY streetChanged)
	Q_PROPERTY(QString html READ html WRITE setHtml NOTIFY htmlChanged)

	Street* street() const;
	QString html() const;

signals:
	void streetChanged(Street* arg);
	void htmlChanged(QString arg);

public slots:
	void setStreet(Street* arg);
	void setHtml(QString arg);

private slots:
	void onReplyArrived(QNetworkReply*reply);
	void onReplyError(QNetworkReply::NetworkError code);

private:
	QStringList processLine(QString &str);
	void request();

	QNetworkAccessManager mManager;
	bool mRetry;

	Street* mStreet;
	QString mHtml;
};

#endif // WIKILOADER_H
