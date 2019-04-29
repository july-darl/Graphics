#include "skybox.h"
#include "rendercommon.h"
#include "cameramanager.h"
#include "resourceinfo.h"

REGISTER(SkyBox)

void SkyBox::Create()
{
    initializeOpenGLFunctions();

    program = CResourceInfo::Inst()->CreateProgram("skybox.vsh","skybox.fsh","skybox");
}

void SkyBox::Render()
{
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    program->bind();

    program->setUniformValue("ModelMatrix",modelMatrix);
    program->setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    program->setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());
    program->setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, RenderCommon::Inst()->GetHDREnvCubemap());
    program->setUniformValue("envCubemap", 0);

    RenderCommon::Inst()->GetGeometryEngine()->drawCube(program);

    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
}
