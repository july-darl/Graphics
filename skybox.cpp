#include "skybox.h"
#include "rendercommon.h"
#include "cameramanager.h"
#include "resourceinfo.h"

REGISTER(SkyBox)

void SkyBox::Create()
{
    program = CResourceInfo::Inst()->CreateProgram("skybox.vsh","skybox.fsh","skybox");
}

void SkyBox::Render()
{
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();

    gl->glDepthMask(GL_FALSE);
    gl->glDisable(GL_CULL_FACE);
    gl->glDepthFunc(GL_LEQUAL);

    program->bind();

    program->setUniformValue("ModelMatrix",modelMatrix);
    program->setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    program->setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());
    program->setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());

    gl->glActiveTexture(GL_TEXTURE0);

    gl->glBindTexture(GL_TEXTURE_CUBE_MAP, RenderCommon::Inst()->GetHDREnvCubemap());
    program->setUniformValue("envCubemap", 0);

    RenderCommon::Inst()->GetGeometryEngine()->drawCube(program);

    gl->glDepthFunc(GL_LESS);
    gl->glEnable(GL_CULL_FACE);
    gl->glDepthMask(GL_TRUE);
}
