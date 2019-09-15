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
    vector<bool> renderFlags = { bSSR, bBloom, bXRay,bOutline };
    int offset = 0;
    for(auto renderFlag : renderFlags)
    {
        if(renderFlag)
        {
            mode += 1 << offset;
        }
        offset++;
    }

    program->bind();

    program->setUniformValue("ModelMatrix",modelMatrix);
    program->setUniformValue("IT_ModelMatrix",IT_modelMatrix);
    program->setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    program->setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());

    program->setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());
    program->setUniformValue("zNear", RenderCommon::Inst()->GetZNearPlane());
    program->setUniformValue("metal",metal);
    program->setUniformValue("id",mode);

    program->setUniformValue("rough", rough);
    program->setUniformValue("ao", ao);
    program->setUniformValue("rq", renderPriority);

    if(shape == SHA_Obj && pModel)
    {
        for(size_t i = 0;i < pModel->Count(); i++)
        {
            auto mesh = pModel->GetMesh(i);
            if(mesh->buffer)
            {
                if( mesh->albedo)
                {
                    gl->glActiveTexture(GL_TEXTURE0);
                    mesh->albedo->bind();
                    program->setUniformValue("albedo", 0);
                    program->setUniformValue("color",QVector3D(-1,-1,-1));
                }

                RenderCommon::Inst()->GetGeometryEngine()->drawObj(mesh->buffer, program);
            }
        }
    }
    else
    {
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
        Draw(program, false);
    }
}

void Phong::DecalRender()
{
    if(bXRay)
    {
        QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();
        QOpenGLShaderProgram* program = CResourceInfo::Inst()->CreateProgram("xray.vsh","xray.fsh","xray");
        program->bind();
        gl->glDepthFunc(GL_GEQUAL);

        program->setUniformValue("ModelMatrix",modelMatrix);
        program->setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
        program->setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());

        Draw(program, false);
        gl->glDepthFunc(GL_LESS);
    }
 //   qDebug() << "DecalRender";
}

void Phong::SecondRender()
{
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();

    QOpenGLShaderProgram* bloomProgram = nullptr;
    if(!bFire)
    {
        bloomProgram = CResourceInfo::Inst()->CreateProgram("bloom.vsh","bloom.fsh","bloom");
        bloomProgram->bind();

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

        bloomProgram->setUniformValue("bBloom", bBloom);
    }
    else
    {

        bloomProgram = CResourceInfo::Inst()->CreateTessProgram("fire.tcsh","fire.tesh","fire.vsh","fire.fsh","fire");
        bloomProgram->bind();

        gl->glEnable(GL_BLEND);
        gl->glBlendFunc(GL_ONE, GL_ONE);

        gl->glActiveTexture(GL_TEXTURE2);
        CResourceInfo::Inst()->CreateTexture("Noise2.TGA")->bind();
        bloomProgram->setUniformValue("noise", 2);

        float radius = max(scale.x,scale.y);
        radius = max(scale.x,scale.z);
        bloomProgram->setUniformValue("time",RenderCommon::Inst()->GetTime());
        bloomProgram->setUniformValue("cameraPos",Camera::Inst()->GetCameraPos());
        bloomProgram->setUniformValue("zFar",RenderCommon::Inst()->GetZFarPlane());
        bloomProgram->setUniformValue("zNear",RenderCommon::Inst()->GetZNearPlane());
        bloomProgram->setUniformValue("radius",radius);

        QVector3D windDir(0.4f,1.0f,0.0f);
        windDir.normalized();
        bloomProgram->setUniformValue("windDir",windDir);
    }

    bloomProgram->setUniformValue("ModelMatrix",modelMatrix);
    bloomProgram->setUniformValue("IT_ModelMatrix",IT_modelMatrix);
    bloomProgram->setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    bloomProgram->setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());


    if(bFire)
    {
        if(shape == SHA_Obj && pModel)
        {
            RenderCommon::Inst()->GetGeometryEngine()->drawObj(m_strObjName,bloomProgram,true);
        }
        else
        {
            Draw(bloomProgram, true);
        }
        gl->glDisable(GL_BLEND);
    }
    else
    {
        Draw(bloomProgram, false);
    }
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
