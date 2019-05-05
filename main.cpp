#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>

#ifndef QT_NO_OPENGL
#include "mainwidget.h"
#endif

int main(int argc, char *argv[])
{
    srand(time(nullptr));
    QApplication app(argc, argv);
    QSurfaceFormat format;

    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3,5);
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    app.setApplicationName("OpenGL Demo");
    app.setApplicationVersion("1.0");

#ifndef QT_NO_OPENGL
    MainWidget widget;
    widget.show();
#else
    QLabel note("OpenGL Support required");
    note.show();
#endif
    return app.exec();
}
