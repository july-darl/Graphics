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
    rendercomponent.cpp \
    environmentwidget.cpp \
    skybox.cpp \
    particlesystem.cpp \
    baseobjectwidget.cpp \
    noise.cpp \
    volumecloud.cpp \
    resourceinfo.cpp \
    volumecloudwidget.cpp

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
    rendercomponent.h \
    environmentwidget.h \
    stb_image.h \
    skybox.h \
    particlesystem.h \
    baseobjectwidget.h \
    noise.h \
    volumecloud.h \
    resourceinfo.h \
    volumecloudwidget.h

RESOURCES += \
    shaders.qrc \
    config.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/cube
INSTALLS += target

DISTFILES +=
