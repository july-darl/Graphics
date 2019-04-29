#ifndef COBJECT_H
#define COBJECT_H

#include <vector>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include "rendercommon.h"
#include "reflect.h"

using namespace std;

class Object : protected QOpenGLExtraFunctions
{
protected:
    QOpenGLShaderProgram* program = nullptr;

    QMatrix4x4           modelMatrix;
    QMatrix4x4           IT_modelMatrix;

    string               m_strName;
public:
    Vector3f             position = Vector3f(0,0,0);
    Vector3f             rotation = Vector3f(0,0,0);
    Vector3f             scale    = Vector3f(1,1,1);
    bool                 bCastShadow = true;
    Shape                shape;
    string               type;
    int                  id;

    const string&        GetName() { return m_strName; }
    void                 SetName(const string& name) { m_strName = name;}

    void                 GenShadowMap();
    void                 GenSnowDepth();
    bool                 IsOnScreen();

    virtual void         UpdateLocation();
    virtual void         Create() = 0;
    virtual void         Render() { }
    virtual void         SecondRender() { }
    virtual void         LateRender() { }
    virtual void         Draw();

    virtual              ~Object() { }
};



class ObjectInfo : protected QOpenGLFunctions
{
private:
    QOpenGLTexture*      snow_D;
    QOpenGLTexture*      snow_N;

    vector<Object*>      vecObj;

    static ObjectInfo* objInfo;
public:
    static ObjectInfo* Inst() {
        if(!objInfo) {
            objInfo = new ObjectInfo();
        }
        return objInfo;
    }

    Object*              GetObject(size_t idx) { return vecObj[idx];}
    int                  GetObjectCount() { return static_cast<int>(vecObj.size());}
    void                 Create();
    void                 Render();
    Object*              CreateObject(string name);
    void                 DelayRender();
private:
    void                 Load();
};

#endif // COBJECT_H
