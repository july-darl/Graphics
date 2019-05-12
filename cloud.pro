QT       += core gui widgets opengl xml

TARGET = cube
TEMPLATE = app

SOURCES += main.cpp \
    rendercommon.cpp \
    skybox.cpp \
    cameramanager.cpp \
    light.cpp \
    object.cpp \
    reflect.cpp \
    phong.cpp \
    icon.cpp \
    openglwidget.cpp \
    lightwidget.cpp \
    pbrwidget.cpp \
    outlinewidget.cpp \
    propertywidget.cpp \
    environmentwidget.cpp \
    skybox.cpp \
    particlesystem.cpp \
    baseobjectwidget.cpp \
    noise.cpp \
    volumecloud.cpp \
    resourceinfo.cpp \
    decal.cpp \
    decalwidget.cpp

SOURCES += \
    mainwidget.cpp \
    geometryengine.cpp

HEADERS += \
    mainwidget.h \
    geometryengine.h \
    rendercommon.h \
    skybox.h \
    cameramanager.h \
    light.h \
    object.h \
    reflect.h \
    phong.h \
    icon.h \
    openglwidget.h \
    lightwidget.h \
    pbrwidget.h \
    outlinewidget.h \
    propertywidget.h \
    environmentwidget.h \
    stb_image.h \
    skybox.h \
    particlesystem.h \
    baseobjectwidget.h \
    noise.h \
    volumecloud.h \
    resourceinfo.h \
    decal.h \
    decalwidget.h

RESOURCES += \
    shaders.qrc \
    config.qrc

DLLDESTDIR += bin

INCLUDEPATH += include

LIBS += -L$$PWD/lib/ \
        -lassimp-vc140-mt

