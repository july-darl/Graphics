#ifndef VOLUMECLOUD_H
#define VOLUMECLOUD_H

#include "object.h"
#include <QOpenGLShaderProgram>

class QOpenGLTexture;
class VolumeCloud : public Object
{
public:
    float           thickness = 0.5f;
    float           mixRatio = 0.7f;
    float           brightness = 0.5f;
    void            Create();

    void            SecondRender();
};
#endif // VOLUMECLOUD_H
