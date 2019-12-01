QT       += core gui widgets

TARGET = meadow
TEMPLATE = app

SOURCES += main.cpp

SOURCES += \
    mainwidget.cpp \
    geometryengine.cpp \
    wavefrontObj.cpp

HEADERS += \
    mainwidget.h \
    geometryengine.h \
    wavefrontObj.h

RESOURCES += \
    shaders.qrc \
    textures.qrc \
    objects.qrc
