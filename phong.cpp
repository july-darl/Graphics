#include "phong.h"
#include "cameramanager.h"
#include "resourceinfo.h"

REGISTER(Phong)
void Phong::Create()
{
    program = CResourceInfo::Inst()->CreateProgram("gbuffer.vsh","gbuffer.fsh","gbuffer");
}

void Phong::Render()
{
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();

    int mode = 0;
    if(bSSR)
    {
        mode += 1;
    }
    if(bFire)
    {
        mode += 2;
    }

    program->bind();

    program->setUniformValue("ModelMatrix",modelMatrix);
    program->setUniformValue("IT_ModelMatrix",IT_modelMatrix);
    program->setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    program->setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());

    program->setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());
    program->setUniformValue("metal",metal);
    program->setUniformValue("id",mode);



    if(albedo != 0)
    {
        gl->glActiveTexture(GL_TEXTURE0);
        gl->glBindTexture(GL_TEXTURE_2D, albedo);
        program->setUniformValue("albedo", 0);
        program->setUniformValue("color",QVector3D(-1,-1,-1));
    }
    else
    {
        program->setUniformValue("color",QVector3D(color.x,color.y,color.z));
    }

    if(normal != 0)
    {
        gl->glActiveTexture(GL_TEXTURE1);
        gl->glBindTexture(GL_TEXTURE_2D, normal);
        program->setUniformValue("normal", 1);
        program->setUniformValue("bUseNormalMap", true);
    }
    else
    {
        program->setUniformValue("bUseNormalMap", false);
    }

    if(maskTex != 0)
    {
        gl->glActiveTexture(GL_TEXTURE2);
        gl->glBindTexture(GL_TEXTURE_2D, maskTex);
        program->setUniformValue("mask", 2);
    }

    program->setUniformValue("rough", rough);
    program->setUniformValue("ao", ao);
}

void Phong::SecondRender()
{
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();

    int screenX = RenderCommon::Inst()->GetScreenX();
    int screenY = RenderCommon::Inst()->GetScreenY();
    int width = screenX / 2;
    int height = screenY / 2;

    gl->glViewport(0,0,width,height);

    QOpenGLShaderProgram* bloomProgram = nullptr;
    if(!bFire)
    {
        bloomProgram = CResourceInfo::Inst()->CreateProgram("bloom.vsh","bloom.fsh","bloom");
    }
    else
    {
        bloomProgram = CResourceInfo::Inst()->CreateTessProgram("fire.tcsh","fire.tesh","bloom.vsh","bloom.fsh","bloomTess");
    }
   //auto p = CResourceInfo::Inst()->CreateTessProgram("test.tcsh","test.tesh","test.vsh","test.fsh","bloomTess");
   //p->bind();
   //
   //RenderCommon::Inst()->GetGeometryEngine()->drawSphere(p,true);

    bloomProgram->bind();

    bloomProgram->setUniformValue("ModelMatrix",modelMatrix);
    bloomProgram->setUniformValue("IT_ModelMatrix",IT_modelMatrix);
    bloomProgram->setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    bloomProgram->setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());

    if(albedo != 0)
    {
        gl->glActiveTexture(GL_TEXTURE0);
        gl->glBindTexture(GL_TEXTURE_2D, albedo);
        bloomProgram->setUniformValue("albedo", 0);
        bloomProgram->setUniformValue("color",QVector3D(-1,-1,-1));
    }
    else
    {
        bloomProgram->setUniformValue("color",QVector3D(color.x,color.y,color.z));
    }
    gl->glActiveTexture(GL_TEXTURE1);
    gl->glBindTexture(GL_TEXTURE_2D, maskTex);
    bloomProgram->setUniformValue("Mask", 1);

    bloomProgram->setUniformValue("bFire",bFire);
    bloomProgram->setUniformValue("bBloom", bBloom);
    bloomProgram->setUniformValue("width", width);
    bloomProgram->setUniformValue("height",height);


    if(bFire)
    {
        gl->glActiveTexture(GL_TEXTURE2);
        CResourceInfo::Inst()->CreateTexture("T_Perlin_Noise_M.TGA")->bind();
        bloomProgram->setUniformValue("perlin", 2);
        bloomProgram->setUniformValue("time",RenderCommon::Inst()->GetTime());
    }

    gl->glActiveTexture(GL_TEXTURE3);
    CResourceInfo::Inst()->CreateTexture("T_Fire_Tiled_D.TGA")->bind();
    bloomProgram->setUniformValue("fire",3);

    if(bFire)
    {
        Draw(bloomProgram, true);
    }
    else
    {
        Draw(bloomProgram, false);
    }
    gl->glViewport(0,0,screenX,screenY);

}

void Phong::SetImage(const QString& path, GLuint& tex, QImage& img)
{
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();

    img.load(path);
    img = img.convertToFormat(QImage::Format_RGBA8888);

    gl->glDeleteTextures(1, &tex);
    gl->glGenTextures(1, &tex);
    gl->glBindTexture(GL_TEXTURE_2D, tex);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(),
       img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    gl->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    gl->glBindTexture(GL_TEXTURE_2D, 0);
}

void Phong::SetAlbedo(const QString& path)
{

}
