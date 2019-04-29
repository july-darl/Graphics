#ifndef HATCH_H
#define HATCH_H


#include "object.h"
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
class Hatch : public Object
{
private:
    GLuint p[6];
public:
    void Create();
    void Render();
    void UpdateLocation();
};

#endif // HATCH_H
