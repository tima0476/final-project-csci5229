QT       += core gui widgets

TARGET = meadow
TEMPLATE = app

SOURCES += main.cpp

SOURCES += \
    mainwidget.cpp \
    geometryengine.cpp \
    vboindexer.cpp

HEADERS += \
    mainwidget.h \
    geometryengine.h \
    vboindexer.h

RESOURCES += \
    shaders.qrc \
    textures.qrc \
    objects.qrc
