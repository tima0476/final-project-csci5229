QT       += core gui widgets

TARGET = meadow
TEMPLATE = app

SOURCES += main.cpp

SOURCES += \
    mainwidget.cpp \
    geometryengine.cpp

HEADERS += \
    mainwidget.h \
    geometryengine.h

RESOURCES += \
    shaders.qrc \
    textures.qrc
