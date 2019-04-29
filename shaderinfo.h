#ifndef SHADERINFO_H
#define SHADERINFO_H
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <map>

using namespace std;

class CResourceInfo
{
private:
    map<string, QOpenGLShader*>         mapVShader;
    map<string, QOpenGLShader*>         mapFShader;
    map<string, QOpenGLTexture*>        mapTexture;
    map<string, QOpenGLShaderProgram*>  mapProgram;

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
    QOpenGLTexture*             CreateTexture(string name);
    void                        CreateFrameBuffer(GLuint& frameBuffer, GLuint& texId);

    QOpenGLShaderProgram*       CreateProgram(string vShaderName, string fShaderName, string name);
    QOpenGLShaderProgram*       CreateProgram(string vShaderName, string fShaderName);

    QOpenGLShaderProgram*       GetProgram(string name);
    void                        DestroyProgram(QOpenGLShaderProgram* program);
    void                        DestroyProgram(string name);
};

#endif // SHADERINFO_H
