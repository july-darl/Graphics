#include "volumecloud.h"
#include "cameramanager.h"
#include "rendercommon.h"
#include "resourceinfo.h"
#include <QDateTime>
REGISTER(VolumeCloud)

void VolumeCloud::Create()
{
    program = CResourceInfo::Inst()->CreateProgram("volumecloud.vsh","volumecloud.fsh","VolumeCloud");
}

void VolumeCloud::SecondRender()
{
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();

    int screenX = RenderCommon::Inst()->GetScreenX();
    int screenY = RenderCommon::Inst()->GetScreenY();
    auto gFrameBuffer = CResourceInfo::Inst()->CreateFrameBuffer("GBuffer", screenX, screenY, 3);

    QDateTime time = QDateTime::currentDateTime();
    qint64 second = QDateTime::currentMSecsSinceEpoch();

    static qint64 startTime = second;

    float times;
    second = second - startTime;
    times = static_cast<float>(second) / 2000;


    program->bind();

    program->setUniformValue("ModelMatrix",modelMatrix);
    program->setUniformValue("IT_ModelMatrix",IT_modelMatrix);
    program->setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    program->setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());
    program->setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());
    program->setUniformValue("time",times);
    program->setUniformValue("cameraPos",Camera::Inst()->GetCameraPos());
    program->setUniformValue("thickness",thickness);
    program->setUniformValue("mixRatio",mixRatio);
    program->setUniformValue("brightness",brightness);

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_2D, gFrameBuffer->vecTexId[EGT_Color]);
    program->setUniformValue("color", 0);

    gl->glActiveTexture(GL_TEXTURE1);
    auto perlin = CResourceInfo::Inst()->CreateTexture("T_Perlin_Noise_M.TGA");
    perlin->bind();
    program->setUniformValue("perlinNoise", 1);
}
