TEMPLATE = app

QT += qml quick location network

TARGET = MoscowStreetList

SOURCES += main.cpp \
    streetdownloader.cpp \
    streetmodel.cpp \
    street.cpp \
    region.cpp \
    district.cpp \
    districtmodel.cpp \
    regionmodel.cpp \
    housemodel.cpp \
    systemfunc.cpp \
    tokenparser.cpp \
    settings.cpp

RESOURCES += qml.qrc \
    pictures.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    streetdownloader.h \
    streetmodel.h \
    street.h \
    region.h \
    district.h \
    districtmodel.h \
    regionmodel.h \
    housemodel.h \
    systemfunc.h \
    tokenparser.h \
    settings.h

DISTFILES += \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
