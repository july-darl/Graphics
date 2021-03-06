#include "resourceinfo.h"
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
    else if(type == QOpenGLShader::TessellationControl)
    {
        if(mapTCShader.find(name) != mapTCShader.end())
        {
            return mapTCShader[name];
        }
        else
        {
            mapTCShader[name] = new QOpenGLShader(QOpenGLShader::TessellationControl);
            mapTCShader[name]->compileSourceFile(QString::fromStdString(":/" + name));
            return mapTCShader[name];
        }
    }
    else if(type == QOpenGLShader::TessellationEvaluation)
    {
        if(mapTEShader.find(name) != mapTEShader.end())
        {
            return mapTEShader[name];
        }
        else
        {
            mapTEShader[name] = new QOpenGLShader(QOpenGLShader::TessellationEvaluation);
            mapTEShader[name]->compileSourceFile(QString::fromStdString(":/" + name));
            return mapTEShader[name];
        }
    }
    return nullptr;
}

QOpenGLTexture* CResourceInfo::CreateTexture(string name, QOpenGLTexture::WrapMode wrapMode)
{
    if(mapTexture.find(name) != mapTexture.end())
    {
        return mapTexture[name];
    }
    else
    {
        QImage image(QString(name.c_str()));
        mapTexture[name] = new QOpenGLTexture(image);
        mapTexture[name]->setMinificationFilter(QOpenGLTexture::Nearest);
        mapTexture[name]->setMagnificationFilter(QOpenGLTexture::Linear);
        mapTexture[name]->setWrapMode(wrapMode);
        return mapTexture[name];
    }
}

GLuint CResourceInfo::CreateTexture(const char* pixels, int width, int height)
{
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();
    GLuint tex_id;
    gl->glGenTextures(1, &tex_id);
    gl->glBindTexture(GL_TEXTURE_2D, tex_id);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    return tex_id;
}

QOpenGLShaderProgram* CResourceInfo::CreateTessProgram(string tcShaderName, string teShaderName,string vShaderName, string fShaderName, string name)
{
    if(mapProgram.find(name) != mapProgram.end())
    {
        return mapProgram[name];
    }
    else
    {
        qDebug() << QString::fromStdString(name) ;
        mapProgram[name] = new QOpenGLShaderProgram();
        QOpenGLShader* tcShader = CreateShader(tcShaderName,QOpenGLShader::TessellationControl);
        QOpenGLShader* teShader = CreateShader(teShaderName,QOpenGLShader::TessellationEvaluation);
        QOpenGLShader* fShader = CreateShader(fShaderName,QOpenGLShader::Fragment);
        QOpenGLShader* vShader = CreateShader(vShaderName,QOpenGLShader::Vertex);

        if(!mapProgram[name]->addShader(tcShader))
            qDebug() << "err";
        if(!mapProgram[name]->addShader(teShader))
            qDebug() << "err";
        mapProgram[name]->addShader(fShader);
        mapProgram[name]->addShader(vShader);
        mapProgram[name]->link();

        return mapProgram[name];
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
        QOpenGLShader* vShader = CreateShader(vShaderName,QOpenGLShader::Vertex);
        QOpenGLShader* fShader = CreateShader(fShaderName,QOpenGLShader::Fragment);

        mapProgram[name]->addShader(vShader);
        mapProgram[name]->addShader(fShader);
        mapProgram[name]->link();

        return mapProgram[name];
    }
}

QOpenGLShaderProgram* CResourceInfo::CreateProgram(string vShaderName, string fShaderName)
{
    auto program = new QOpenGLShaderProgram();
    QOpenGLShader* vShader = CreateShader(vShaderName,QOpenGLShader::Vertex);
    QOpenGLShader* fShader = CreateShader(fShaderName,QOpenGLShader::Fragment);

    program->addShader(vShader);
    program->addShader(fShader);
    program->link();

    return program;
}

STextureBuffer* CResourceInfo::CreateFrameBuffer(string name, int screenX, int screenY, int size)
{
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();
    QOpenGLExtraFunctions* extraGl = QOpenGLContext::currentContext()->extraFunctions();
    STextureBuffer* buffer = nullptr;
    if(mapTextureBuffer.find(name) == mapTextureBuffer.end())
    {
        buffer = new STextureBuffer(size);
        buffer->size = size;
        gl->glGenFramebuffers(1, &buffer->frameBuffer);
        mapTextureBuffer[name] = buffer;
    }
    else
    {
        buffer = mapTextureBuffer[name];
    }

    if(buffer->width != screenX || buffer->height != screenY)
    {
        qDebug() << "reload" << QString::fromStdString(name);
        qDebug() << buffer->width << buffer->height;
        qDebug() << screenX << screenY;
        gl->glBindFramebuffer(GL_FRAMEBUFFER, buffer->frameBuffer);

        gl->glDeleteTextures(buffer->size, buffer->vecTexId);
        gl->glDeleteTextures(1, &buffer->depthTexId);

        gl->glGenTextures(buffer->size, buffer->vecTexId);
        for(int i = 0;i < buffer->size;i++)
        {
            gl->glBindTexture(GL_TEXTURE_2D, buffer->vecTexId[i]);
            gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, screenX, screenY, 0, GL_RGBA, GL_FLOAT, nullptr);
            gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            gl->glBindTexture(GL_TEXTURE_2D, 0);
            gl->glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i), GL_TEXTURE_2D, buffer->vecTexId[i], 0);
        }

        gl->glGenTextures(1, &buffer->depthTexId);
        gl->glBindTexture(GL_TEXTURE_2D, buffer->depthTexId);
        gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, screenX, screenY, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
        gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer->depthTexId, 0);

        if(buffer->size > 1)
        {
            size_t size = static_cast<size_t>(buffer->size);
            GLuint* attachments = new GLuint[size];
            for(size_t i = 0;i<size;i++)
            {
                attachments[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i);
            }
            extraGl->glDrawBuffers(buffer->size, attachments);
        }

    }
    buffer->width = screenX;
    buffer->height = screenY;

    return buffer;
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
