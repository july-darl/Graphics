#include "phong.h"
#include "cameramanager.h"
#include "resourceinfo.h"

REGISTER(Phong)
void Phong::Create()
{
    initializeOpenGLFunctions();
    program = CResourceInfo::Inst()->CreateProgram("gbuffer.vsh","gbuffer.fsh","gbuffer");
}

void Phong::Render()
{
    program->bind();

    program->setUniformValue("ModelMatrix",modelMatrix);
    program->setUniformValue("IT_ModelMatrix",IT_modelMatrix);
    program->setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    program->setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());

    program->setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());
    program->setUniformValue("metal",metal);
    program->setUniformValue("id",id);

    if(albedo != 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo);
        program->setUniformValue("albedo", 0);
        program->setUniformValue("color",QVector3D(-1,-1,-1));
    }
    else
    {
        program->setUniformValue("color",QVector3D(color.x,color.y,color.z));
    }

    if(normal != 0)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normal);
        program->setUniformValue("normal", 1);
        program->setUniformValue("bUseNormalMap", true);
    }
    else
    {
        program->setUniformValue("bUseNormalMap", false);
    }

    if(pbrTex != 0)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, pbrTex);
        program->setUniformValue("pbr", 2);
        program->setUniformValue("rough", -1);
        program->setUniformValue("ao", -1);
    }
    else
    {
        program->setUniformValue("rough", rough);
        program->setUniformValue("ao", ao);
    }

   //  glBindTexture(GL_TEXTURE_2D, 0);
}

void Phong::SetImage(const QString& path, GLuint& tex, QImage& img)
{
    img.load(path);
    img = img.convertToFormat(QImage::Format_RGBA8888);

    glDeleteTextures(1, &tex);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(),
       img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Phong::SetAlbedo(const QString& path)
{

}
