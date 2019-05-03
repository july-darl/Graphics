#ifndef DECAL_H
#define DECAL_H

#include "object.h"

class Decal : public Object
{
private:
    QOpenGLTexture* BushTex;
    QMatrix4x4 decalMatrix;
    float decalSize = 2.0f;
public:
    void Create();
    void SecondRender();
    void UpdateLocation();
    void CalculateDecalMatrix();
    void Draw() { }
};

#endif // DECAL_H
