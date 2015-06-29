#include "wikiloader.h"
#include "street.h"

WikiLoader::WikiLoader(QObject *parent) : QObject(parent)
{
	connect(&mManager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(onReplyArrived(QNetworkReply*)));
}

WikiLoader::~WikiLoader()
{
}

Street *WikiLoader::street() const
{
	return mStreet;
}

QString WikiLoader::html() const
{
	return mHtml;
}

void WikiLoader::setStreet(Street *arg)
{
	if (mStreet == arg)
		return;

	mStreet = arg;
	mRetry = false;
	setHtml(QString());
	request();
	emit streetChanged(arg);
}

void WikiLoader::setHtml(QString arg)
{
	if (mHtml == arg)
		return;

	mHtml = arg;
	emit htmlChanged(arg);
}


QString space(int level)
{
	QString res;
	for(int i = 0; i<level; ++i)
		res += "    ";
	return res;
}

QStringList WikiLoader::processLine(QString& str)
{
	int level = 0;

	QRegExp re("<(\\/?)([a-z]+)(.*)>");
	QRegExp re2("class=\"(.*)\"");
	re.setMinimal(true);
	re2.setMinimal(true);

	QStack<QString> tags;
	QStringList res;
	int index = 0;
	QStringList noBrackTags, skipTags, skipAllTags, skipClasses;
	noBrackTags << "br" << "img" << "hr";
	skipTags << "a" << "img" << "link";
	skipAllTags << "script" << "form" << "sup";
	skipClasses << "edit-page" << "header" << "last-modified-bar"
		<< "pre-content" << "thumbimage" << "wikidata-claim"
		<< "external" << "dablink" << "metadata" << "references"
		<< "navbox" << "footer-info" << "footer-places";
	// TODO class=geo - coords
	bool skipMode = false;
	QString skipTag;
	int skipLevel = 0;
	while(true) {
		int newindex = re.indexIn(str, index);
		if (newindex != -1) {
			if (!skipMode && index != newindex)
				res.append(space(level)+str.mid(index, newindex - index));
			QString tag = re.cap(2);
			QString contents = re.cap(3);
			bool single = !contents.isEmpty() &&
				contents.at(contents.length()-1) == '/';
			bool end = !re.cap(1).isEmpty() && !single;
			bool start = re.cap(1).isEmpty() && !single;
			QStringList classes;
			if (re2.indexIn(contents) != -1)
				classes = re2.cap(1).split(' ');

			bool skip = skipAllTags.contains(tag);
			foreach(QString cls, classes)
				if (skipClasses.contains(cls)) {
					skip = true;
					break;
				}

			if (!skipMode && start && skip) {
				skipTag = tag;
				skipLevel = level;
				skipMode = true;
			}

			if (end) {
				--level;
				if (tags.top() != tag)
					qDebug() << tags.top() << tag;
				tags.pop();
			}
			if (!skipMode && !skipTags.contains(tag))
				res.append(space(level)+re.cap());
			if (start && !noBrackTags.contains(tag)) {
				++level;
				tags.push(tag);
			}

			if (skipMode && end && tag == skipTag && level == skipLevel)
				skipMode = false;

			index = newindex + re.cap().length();
		} else {
			res.append(space(level)+str.right(str.length()-index));
			break;
		}
	}

	return res;
}

void WikiLoader::request()
{
	QString url = mRetry ?
		QString("https://ru.m.wikipedia.org/wiki/%1").
			arg(street()->canonical()).replace(" ", "_") :
		QString("https://ru.m.wikipedia.org/wiki/%1_(Москва)").
			arg(street()->canonical()).replace(" ", "_");
	QNetworkReply* reply = mManager.get(QNetworkRequest(QUrl(url)));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
		this, SLOT(onReplyError(QNetworkReply::NetworkError)));
}

void WikiLoader::onReplyArrived(QNetworkReply *reply)
{
	QString res;
	while (!reply->atEnd()) {
		QByteArray line = reply->readLine();
		QString str = line.data();
		res += str.trimmed();
	}
	reply->deleteLater();
	res = processLine(res).join(' ').simplified();
	setHtml(res);
}

void WikiLoader::onReplyError(QNetworkReply::NetworkError code)
{
	QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
	if (!reply)
		return;
	reply->deleteLater();
	if (!mRetry) {
		mRetry = true;
		request();
	}
}

