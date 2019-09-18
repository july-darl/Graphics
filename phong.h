#ifndef PHONG_H
#define PHONG_H

#include "object.h"
#include <QOpenGLTexture>

class Phong : public Object
{
private:
    void        SetImage(const QString& path, GLuint& tex, QImage& img);
public:
    float       ao = 0.4f;
    float       rough = 0.0f;
    float       metal = 0.0f;
    float       alpha = 1.1f;

    GLuint      albedo = 0;
    GLuint      normal = 0;
    GLuint      maskTex = 0;

    QImage      albedoImg;
    QImage      normalImg;
    QImage      maskImg;

    Model*      pModel;

    bool        bFire = false;
    bool        bBloom = false;
    bool        bSSR = false;
    bool        bXRay = false;
    bool        bOutline = false;

    Vector3f    color    = Vector3f(1,1,1);
    void        SetAlbedo(const QString& path);
    void        Create() override;
    void        Render() override;
    void        SecondRender() override;
    void        DecalRender() override;
    void        ForwardRender() override;
};

#endif // PHONG_H
