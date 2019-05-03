#ifndef WATER_H
#define WATER_H

#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include "object.h"

class Water : public Object
{
private:
    QOpenGLTexture* T_Water_N;

public:
    GLuint cubeTex;
    void Create();
    void Render();
    void UpdateLocation();
};

#endif // WATER_H
