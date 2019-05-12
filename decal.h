#ifndef DECAL_H
#define DECAL_H

#include "object.h"

class Decal : public Object
{
public:
    QMatrix4x4  decalMatrix;
    float       decalSize = 2.0f;

    GLuint      decalTex = 0;

    QImage      decalImg;

public:
    void Create();
    void DecalRender();
    void CalculateDecalMatrix();
    void Draw(bool) { }
    void Draw(QOpenGLShaderProgram*,bool) { }
};

#endif // DECAL_H
