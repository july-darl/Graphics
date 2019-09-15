#ifndef RENDERCOMMON_H
#define RENDERCOMMON_H

#include <QVector2D>
#include <QVector3D>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLExtraFunctions>
#include "geometryengine.h"
#include <QImage>
#include "light.h"

bool fIsZero(float data,float eps = 0.000001f);
template<typename T> T clamp(const T& data, const T& min, const T& max);

enum Shape
{
    SHA_Plane,
    SHA_Cube,
    SHA_Sphere,
    SHA_Obj,
    ShapeNum,
};

enum RenderQueue
{
    RQ_Background,
    RQ_Default,
    RQ_AlphaTest,
    RQ_Transparent
};

struct Vector3f
{
    float x,y,z;
    Vector3f() { }
    Vector3f(float _x,float _y,float _z) :x(_x),y(_y),z(_z) { }
};

struct STextureBuffer
{
    int            size = 0;
    GLuint*        vecTexId;
    GLuint         frameBuffer = 0;
    GLuint         depthTexId  = 0;
    int            width = 0;
    int            height = 0;

    STextureBuffer(int s = 1) { size = s; vecTexId = new GLuint[static_cast<size_t>(size)];}
};

enum EGBufferType
{
    EGT_NormalDepth,
    EGT_Color,
    EGT_Param
};

class RenderCommon
{
private:
    CLightInfo      lightInfo;
    QMatrix4x4      snowDepthMat;

    GLuint          hdrCubemap;
    GLuint          irradianceMap;
    GLuint          prefilterMap;
    GLuint          brdfLUTTexture;

    GLuint          hdrTexture = 0;

    QVector3D       lightLocation = QVector3D(-2,10,7);

    QMatrix4x4      viewMatrix;
    QOpenGLTexture* T_Perlin_Noise_M;
    RenderCommon();

    GeometryEngine* geometryEngine;
    QMatrix4x4      snowOrtho;
    QMatrix4x4      projection;
    QMatrix4x4      lightMatrix;
    QVector3D       centerPos;
    float           zFar = 200.0f;
    float           zNear = 2.0f;
    float           fov = 45.0f;
    float           aspect;

    bool            bHDR = true;
    bool            bGamma = true;
    bool            bSnow = false;
    float           hdrParam = 0.6f;
    int             screenX, screenY;

    void            CreateCubemap();


public:
    static RenderCommon* renderCommon;
    static RenderCommon* Inst() {
        if(!renderCommon) {
            renderCommon = new RenderCommon();
        }
        return renderCommon;
    }

    void            CreateHDRCubemap(QString);

    void UpdateLightMatrix();
    void UpdateSnowDepth();

    void UpdateScreenXY(int x, int y);
    int GetScreenX() { return screenX; }
    int GetScreenY() { return screenY; }

    GLuint GetHDREnvCubemap() {return hdrCubemap;}
    GLuint GetIrradianceMap() { return irradianceMap;}
    GLuint GetprefilterMap() { return prefilterMap; }
    GLuint GetBRDFLutMap() { return brdfLUTTexture;}

    QMatrix4x4& GetSnowDirMatrix() { return snowDepthMat;}

    CLightInfo* GetLightInfo() { return &lightInfo;}

    QOpenGLTexture* GetPerlinNoise() { return T_Perlin_Noise_M; }
    QVector3D& GetLightLocation() {return lightLocation;}

    QMatrix4x4& GetProjectMatrix() { return projection; }
    QMatrix4x4& GetSnowOrthoMatrix() { return snowOrtho; }
    QMatrix4x4& GetLightMatrix() { return lightMatrix;}
    GeometryEngine* GetGeometryEngine() { return geometryEngine;}

    float GetZFarPlane() { return zFar; }
    float GetZNearPlane() { return zNear; }
    float GetFov() { return fov * 3.14159f/180; }
    float GetAspect() { return aspect; }

    bool* GetHDR() { return &bHDR;}
    bool* GetGamma() { return &bGamma; }
    float* GetExposure() {return &hdrParam;}

    float GetHDRParam() { return hdrParam;}
    void SetHDRParam(float param) { hdrParam = param;}

    void UpdateEnvironment();
    bool IsSnow() {return bSnow;}
    void SetSnow(bool b) { bSnow = b;}

    float GetTime();
};


template<typename T>
T clamp(const T& data, const T& min, const T& max)
{
    T result = data;
    if(data >= max) result = max;
    else if(data <= min) result = min;
    return result;
}



#endif // RENDERCOMMON_H
