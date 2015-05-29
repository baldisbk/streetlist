TEMPLATE = app

QT += qml quick location network

SOURCES += main.cpp \
    streetdownloader.cpp \
    streetmodel.cpp \
    street.cpp \
    region.cpp \
    district.cpp

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
    district.h
