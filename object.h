#ifndef COBJECT_H
#define COBJECT_H

#include <vector>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include "rendercommon.h"
#include "reflect.h"

using namespace std;

class Object
{
protected:
    QOpenGLShaderProgram* program = nullptr;

    QMatrix4x4           modelMatrix;
    QMatrix4x4           IT_modelMatrix;

    string               m_strName;
    string               m_strObjName;
public:
    Vector3f             position = Vector3f(0,0,0);
    Vector3f             rotation = Vector3f(0,0,0);
    Vector3f             scale    = Vector3f(1,1,1);
    bool                 bCastShadow = true;
    bool                 bRender = true;
    Shape                shape;
    string               type;
    int                  id;
    int                  renderPriority = 1000;

    const string&        GetName() { return m_strName; }
    void                 SetName(const string& name) { m_strName = name;}
    void                 SetObjName(const string& name) { m_strObjName = name;}
    const string&        GetObjName() { return m_strObjName; }

    void                 GenShadowMap();
    void                 GenSnowDepth();
    bool                 IsOnScreen();

    virtual void         UpdateLocation();
    virtual void         Create() = 0;
    virtual void         Render() { }
    virtual void         DecalRender() { }
    virtual void         SecondRender() { }
    virtual void         LateRender() { }
    virtual void         Draw(bool bTess = false);
    virtual void         Draw(QOpenGLShaderProgram*,bool bTess = false);
    virtual              ~Object() { }
};



class ObjectInfo : protected QOpenGLFunctions
{
private:
    QOpenGLTexture*             snow_D;
    QOpenGLTexture*             snow_N;

    map<int, vector<Object*>>   renderQueue;
    vector<Object*>             vecObj;

    Object*                     activeObj = nullptr;

    static ObjectInfo* objInfo;
public:
    static ObjectInfo* Inst() {
        if(!objInfo) {
            objInfo = new ObjectInfo();
        }
        return objInfo;
    }

    void                 SetActiveObject(Object* obj) { activeObj = obj; }
    Object*              GetActiveObject() { return activeObj;}
    void                 SetRenderQueue(Object* obj, int renderPriority);
    void                 Blend();
    Object*              GetObject(size_t idx) { return vecObj[idx];}
    int                  GetObjectCount() { return static_cast<int>(vecObj.size());}
    void                 Create();
    void                 Render();
    Object*              CreateObject(string name, int queueId = 1000);
    void                 DelayRender();
private:
    void                 Load();
};

#endif // COBJECT_H
