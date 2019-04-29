#ifndef CARTOON_H
#define CARTOON_H

#include "object.h"

class Cartoon : public Object
{
private:
    QOpenGLShaderProgram program1;
public:
    void Create();
    void Render();
    void PostProcess();
    void UpdateLocation();
};

#endif // CARTOON_H
