#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "streetdownloader.h"

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	qmlRegisterType<StreetList>("Streets", 1, 0, "Street");
	qmlRegisterType<StreetList>("Streets", 1, 0, "Region");
	qmlRegisterType<StreetList>("Streets", 1, 0, "District");
	qmlRegisterType<StreetList>("Streets", 1, 0, "StreetList");
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	return app.exec();
}
