#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "streetdownloader.h"

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	qmlRegisterType<StreetDownloader>("Streets", 1, 0, "Street");
	qmlRegisterType<StreetDownloader>("Streets", 1, 0, "Region");
	qmlRegisterType<StreetDownloader>("Streets", 1, 0, "District");
	qmlRegisterType<StreetDownloader>("Streets", 1, 0, "StreetDownloader");
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	return app.exec();
}
