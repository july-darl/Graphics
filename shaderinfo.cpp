#include "shaderInfo.h"
#include "rendercommon.h"
#include <QImage>
#include <QOpenGLFunctions>

CResourceInfo* CResourceInfo::resourceInfo = nullptr;

QOpenGLShader* CResourceInfo::CreateShader(string name, QOpenGLShader::ShaderType type)
{
    if(type == QOpenGLShader::Vertex)
    {
        if(mapVShader.find(name) != mapVShader.end())
        {
            return mapVShader[name];
        }
        else
        {
            mapVShader[name] = new QOpenGLShader(QOpenGLShader::Vertex);
            mapVShader[name]->compileSourceFile(QString::fromStdString(":/" + name));
            return mapVShader[name];
        }
    }
    else if(type == QOpenGLShader::Fragment)
    {
        if(mapFShader.find(name) != mapFShader.end())
        {
            return mapFShader[name];
        }
        else
        {
            mapFShader[name] = new QOpenGLShader(QOpenGLShader::Fragment);
            mapFShader[name]->compileSourceFile(QString::fromStdString(":/" + name));
            return mapFShader[name];
        }
    }
}

QOpenGLTexture* CResourceInfo::CreateTexture(string name)
{
    if(mapTexture.find(name) != mapTexture.end())
    {
        return mapTexture[name];
    }
    else
    {
        QImage image(QString(name.c_str()));
        mapTexture[name] = new QOpenGLTexture(image);
        return mapTexture[name];
    }
}

QOpenGLShaderProgram* CResourceInfo::CreateProgram(string vShaderName, string fShaderName, string name)
{
    if(mapProgram.find(name) != mapProgram.end())
    {
        return mapProgram[name];
    }
    else
    {
        mapProgram[name] = new QOpenGLShaderProgram();
        QOpenGLShader* vShader = CResourceInfo::Inst()->CreateShader(vShaderName,QOpenGLShader::Vertex);
        QOpenGLShader* fShader = CResourceInfo::Inst()->CreateShader(fShaderName,QOpenGLShader::Fragment);

        mapProgram[name]->addShader(vShader);
        mapProgram[name]->addShader(fShader);
        mapProgram[name]->link();

        return mapProgram[name];
    }
}

QOpenGLShaderProgram* CResourceInfo::CreateProgram(string vShaderName, string fShaderName)
{
    auto program = new QOpenGLShaderProgram();
    QOpenGLShader* vShader = CResourceInfo::Inst()->CreateShader(vShaderName,QOpenGLShader::Vertex);
    QOpenGLShader* fShader = CResourceInfo::Inst()->CreateShader(fShaderName,QOpenGLShader::Fragment);

    program->addShader(vShader);
    program->addShader(fShader);
    program->link();

    return program;
}

void CResourceInfo::CreateFrameBuffer(GLuint& frameBuffer, GLuint& texId)
{
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();

    gl->glGenFramebuffers(1, &frameBuffer);
    gl->glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    int screenX = RenderCommon::Inst()->GetScreenX();
    int screenY = RenderCommon::Inst()->GetScreenY();

    gl->glGenTextures(1, &texId);
    gl->glBindTexture(GL_TEXTURE_2D, texId);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, screenX, screenY, 0, GL_RG, GL_FLOAT, nullptr);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    gl->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    gl->glBindTexture(GL_TEXTURE_2D, 0);

    gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);

    GLuint depthTexId;
    gl->glGenTextures(1, &depthTexId);
    gl->glBindTexture(GL_TEXTURE_2D, depthTexId);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, screenX, screenY, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexId, 0);

}

QOpenGLShaderProgram* CResourceInfo::GetProgram(string name)
{
    if(mapProgram.find(name) != mapProgram.end())
    {
        return mapProgram[name];
    }
    else
    {
        return nullptr;
    }
}

void CResourceInfo::DestroyProgram(QOpenGLShaderProgram* program)
{
    for(auto it = mapProgram.begin(); it!= mapProgram.end(); it++)
    {
        if(it->second == program)
        {
            mapProgram.erase(it);
            delete program;
            break;
        }
    }
}

void CResourceInfo::DestroyProgram(string name)
{
    auto it = mapProgram.find(name);
    if(it!= mapProgram.end())
    {
        auto program = it->second;
        mapProgram.erase(it);
        delete program;
    }
}
