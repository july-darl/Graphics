#ifndef SHADERINFO_H
#define SHADERINFO_H
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <map>
#include "rendercommon.h"

using namespace std;

class CResourceInfo
{
private:
    map<string, QOpenGLShader*>         mapVShader;
    map<string, QOpenGLShader*>         mapFShader;
    map<string, QOpenGLTexture*>        mapTexture;
    map<string, QOpenGLShaderProgram*>  mapProgram;
    map<string, STextureBuffer*>        mapTextureBuffer;

    CResourceInfo(){}

public:
    static CResourceInfo*         resourceInfo;
    static CResourceInfo*         Inst()
    {
        if(!resourceInfo)
        {
            resourceInfo = new CResourceInfo();
        }
        return resourceInfo;
    }

    QOpenGLShader*              CreateShader(string name, QOpenGLShader::ShaderType type);
    QOpenGLTexture*             CreateTexture(string name, QOpenGLTexture::WrapMode wrapMode = QOpenGLTexture::Repeat);
    STextureBuffer*             CreateFrameBuffer(string name, int screenX, int screenY, int size = 1);

    QOpenGLShaderProgram*       CreateTessProgram(string tcShaderName, string teShaderName, string vShaderName, string fShaderName, string name);
    QOpenGLShaderProgram*       CreateProgram(string vShaderName, string fShaderName, string name);
    QOpenGLShaderProgram*       CreateProgram(string vShaderName, string fShaderName);

    QOpenGLShaderProgram*       GetProgram(string name);
    void                        DestroyProgram(QOpenGLShaderProgram* program);
    void                        DestroyProgram(string name);
};

#endif // SHADERINFO_H
