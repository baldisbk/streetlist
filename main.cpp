#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "streetdownloader.h"
#include "streetmodel.h"
#include "districtmodel.h"
#include "regionmodel.h"
#include "housemodel.h"
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
	qmlRegisterType<DistrictModel>("Streets", 1, 0, "DistrictModel");
	qmlRegisterType<RegionModel>("Streets", 1, 0, "RegionModel");
	qmlRegisterType<HouseModel>("Streets", 1, 0, "HouseModel");
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	return app.exec();
}
