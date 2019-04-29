#ifndef SKYBOX_H
#define SKYBOX_H

#include "object.h"
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

class SkyBox : public Object
{
private:
    QOpenGLShaderProgram cubemapProgram;

public:
    void            Create();

    void            Render();
    void            Draw() { }
};

#endif // SKYBOX_H
