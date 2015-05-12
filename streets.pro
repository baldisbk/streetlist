TEMPLATE = app

QT += qml quick location network

SOURCES += main.cpp \
    streetdownloader.cpp \
    streetmodel.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    streetdownloader.h \
    streetmodel.h
