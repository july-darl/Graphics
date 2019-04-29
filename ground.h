#ifndef GROUND_H
#define GROUND_H

#include "object.h"
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

class Ground : public Object
{
public:
    void Create();
    void Render();
    void UpdateLocation();
};

#endif // GROUND_H
