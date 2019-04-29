#ifndef PLANE_H
#define PLANE_H

#include "object.h"
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

class Plane : public Object
{
private:
    QOpenGLTexture* T_Brick_N;
    QOpenGLTexture* T_Brick_D;
    QOpenGLTexture* T_Brick_M;

public:
    int type = 0;
    void Create();
    void Render();
    void UpdateLocation();
};

#endif // PLANE_H
