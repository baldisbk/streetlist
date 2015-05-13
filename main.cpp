#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "streetdownloader.h"
#include "streetmodel.h"
#include "street.h"
#include "district.h"
#include "region.h"

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	qmlRegisterType<Street>("Streets", 1, 0, "Street");
	qmlRegisterType<Region>("Streets", 1, 0, "Region");
	qmlRegisterType<District>("Streets", 1, 0, "District");
	qmlRegisterType<StreetList>("Streets", 1, 0, "StreetList");
	qmlRegisterType<StreetModel>("Streets", 1, 0, "StreetModel");
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	return app.exec();
}
