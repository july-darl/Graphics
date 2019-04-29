﻿#ifndef PHONG_H
#define PHONG_H

#include "object.h"
#include <QOpenGLTexture>

class Phong : public Object
{
private:
    void        SetImage(const QString& path, GLuint& tex, QImage& img);
public:
    float       ao = 1.0f;
    float       rough = 0.0f;
    float       metal = 0.0f;

    GLuint      albedo = 0;
    GLuint      normal = 0;
    GLuint      pbrTex = 0;

    QImage      albedoImg;
    QImage      normalImg;
    QImage      pbrImg;

    Vector3f    color    = Vector3f(1,1,1);
    void        SetAlbedo(const QString& path);
    void        Create() override;
    void        Render() override;
};

#endif // PHONG_H
