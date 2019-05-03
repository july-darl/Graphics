#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <string>
using namespace std;

class RenderComponent
{
protected:
    QOpenGLShaderProgram program;
    QMatrix4x4           transformMatrix;

    void AddShader(string vShaderName,string fShaderName);
public:
    virtual void         Create() { }
    virtual void         Render() { }
    virtual ~RenderComponent() { }
};

#endif // RENDERCOMPONENT_H
