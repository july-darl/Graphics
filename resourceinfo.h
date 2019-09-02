#ifndef SHADERINFO_H
#define SHADERINFO_H
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <unordered_map>
#include "rendercommon.h"

using namespace std;

class CResourceInfo
{
private:
    unordered_map<string, QOpenGLShader*>         mapVShader;
    unordered_map<string, QOpenGLShader*>         mapFShader;
    unordered_map<string, QOpenGLShader*>         mapTCShader;
    unordered_map<string, QOpenGLShader*>         mapTEShader;

    unordered_map<string, QOpenGLTexture*>        mapTexture;
    unordered_map<string, QOpenGLShaderProgram*>  mapProgram;
    unordered_map<string, STextureBuffer*>        mapTextureBuffer;

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
