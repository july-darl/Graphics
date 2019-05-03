#include "object.h"
#include "skybox.h"
#include "rendercommon.h"
#include "hatch.h"
#include "water.h"
#include "cartoon.h"
#include "icon.h"
#include "phong.h"
#include "cameramanager.h"
#include "resourceinfo.h"
#include <QOpenGLTexture>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QFile>
#include <QDateTime>
#include "noise.h"
ObjectInfo* ObjectInfo::objInfo = nullptr;

void Object::Draw()
{
   // qDebug() << shape;
    switch(shape)
    {
    case SHA_Cube:
        RenderCommon::Inst()->GetGeometryEngine()->drawCube(program);
        break;
    case SHA_Sphere:
        RenderCommon::Inst()->GetGeometryEngine()->drawSphere(program);
        break;
    case SHA_Plane:
        RenderCommon::Inst()->GetGeometryEngine()->drawPlane(program);
        break;
    default:
        RenderCommon::Inst()->GetGeometryEngine()->drawPlane(program);
    }
}

void Object::Draw(QOpenGLShaderProgram* p)
{
   // qDebug() << shape;
    switch(shape)
    {
    case SHA_Cube:
        RenderCommon::Inst()->GetGeometryEngine()->drawCube(p);
        break;
    case SHA_Sphere:
        RenderCommon::Inst()->GetGeometryEngine()->drawSphere(p);
        break;
    case SHA_Plane:
        RenderCommon::Inst()->GetGeometryEngine()->drawPlane(p);
        break;
    default:
        RenderCommon::Inst()->GetGeometryEngine()->drawPlane(p);
    }
}

void Object::GenShadowMap()
{
    QOpenGLShaderProgram* shadowMapProgram = CResourceInfo::Inst()->GetProgram("ShadowMap");
    shadowMapProgram->setUniformValue("LightMatrix",   RenderCommon::Inst()->GetLightMatrix());
    shadowMapProgram->setUniformValue("ProjectMatrix", RenderCommon::Inst()->GetProjectMatrix());
    shadowMapProgram->setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());
    shadowMapProgram->setUniformValue("ModelMatrix",  modelMatrix);
    shadowMapProgram->setUniformValue("IT_ModelMatrix",  IT_modelMatrix);
    Draw(shadowMapProgram);
}

void Object::GenSnowDepth()
{
    QOpenGLShaderProgram* snowDepthProgram = CResourceInfo::Inst()->GetProgram("SnowDepth");
    snowDepthProgram->setUniformValue("SnowMatrix",   RenderCommon::Inst()->GetSnowDirMatrix());
    snowDepthProgram->setUniformValue("OrthoMatrix", RenderCommon::Inst()->GetSnowOrthoMatrix());
    snowDepthProgram->setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());
    snowDepthProgram->setUniformValue("ModelMatrix",  modelMatrix);
    snowDepthProgram->setUniformValue("IT_ModelMatrix",  IT_modelMatrix);
    Draw();
}

bool Object::IsOnScreen()
{
    QVector4D pos = RenderCommon::Inst()->GetProjectMatrix() *(Camera::Inst()->GetViewMatrix() * (modelMatrix * QVector4D(position.x,position.y,position.z,1)));
    float x = pos.x()/pos.w();
    float y = pos.y()/pos.w();
    float z = pos.z()/pos.w();
    return (x >= -1 && x <= 1 && y >= -1 && y <= 1 && z >=-1 && z <=1);
}

void Object::UpdateLocation()
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(QVector3D(position.x,position.y,position.z));
    modelMatrix.rotate(rotation.x,QVector3D(1,0,0));
    modelMatrix.rotate(rotation.y,QVector3D(0,1,0));
    modelMatrix.rotate(rotation.z,QVector3D(0,0,1));
    modelMatrix.scale(QVector3D(scale.x,scale.y,scale.z));
    IT_modelMatrix = modelMatrix;
    IT_modelMatrix = IT_modelMatrix.transposed();
    IT_modelMatrix = IT_modelMatrix.inverted();
}

bool GetVec3FromString(const char* str, float &x, float &y, float &z)
{
    if (sscanf_s( str,"%f,%f,%f",&x,&y,&z ) == 3)
    {
        return true;
    }
    return false;
}

void ObjectInfo::Load()
{
    QFile file(":/objectInfo.xml");
    if(!file.open(QFile::ReadOnly))
    {
        qDebug() << "fail open";
        return;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
    {
        qDebug() << "Error: Parse error at line " << errorLine << ", "
                                                 << "column " << errorColumn;
        return;
    }
    QDomElement root = doc.documentElement();
    if (root.tagName() != "objectlist")
    {
        qDebug() << "failed load object list";
        return;
    }

    QDomNode child = root.firstChild();
    while (!child.isNull())
    {
        QDomElement element = child.toElement();
        if (element.tagName() == "object")
        {
            QString type     = element.attribute("type");
            QString name     = element.attribute("name");
            QString position = element.attribute("position");
            QString rotation = element.attribute("rotation");
            QString scale    = element.attribute("scale");
            QString bShadow  = element.attribute("bCastShadow");
            QString shape    = element.attribute("shape");

            qDebug() << name;
            if(!type.isEmpty())
            {
                Object* obj = CreateObject(type.toStdString());
                if(obj)
                {
                    if(shape == "cube")
                    {
                        obj->shape = SHA_Cube;
                    }
                    else if(shape == "sphere")
                    {
                        obj->shape = SHA_Sphere;
                    }
                    else if(shape == "plane")
                    {
                        obj->shape = SHA_Plane;
                    }
                    obj->SetName(name.toStdString());
                    obj->bCastShadow = bShadow != "0";
                    GetVec3FromString(position.toStdString().c_str(),obj->position.x,obj->position.y,obj->position.z);
                    GetVec3FromString(rotation.toStdString().c_str(),obj->rotation.x,obj->rotation.y,obj->rotation.z);
                    GetVec3FromString(scale.toStdString().c_str(),obj->scale.x,obj->scale.y,obj->scale.z);

                    if(type == "Billboard")
                    {
                        Billboard* billboard = static_cast<Billboard*>(obj);

                        QString path = element.attribute("image");
                        QString billType = element.attribute("billtype");

                        if(!path.isEmpty())
                        {
                            QImage img(path);
                            billboard->SetImage(img);
                        }
                        if(!billType.isEmpty())
                        {
                            if(billType == "screen")
                            {
                                billboard->SetBillboardType(BILL_Screen);
                            }
                            else if(billType == "axisY")
                            {
                                billboard->SetBillboardType(BILL_YAxis);
                            }
                            else if(billType == "world")
                            {
                                billboard->SetBillboardType(BILL_World);
                            }
                        }
                    }
                    else if(type == "Phong")
                    {
                        Phong* phong = static_cast<Phong*>(obj);
                        QString albedo = element.attribute("albedo");
                        QString normal = element.attribute("normal");
                        if(!albedo.isEmpty())
                        {
                            //phong->albedo = QImage(albedo);
                        }
                    }
                }
            }
        }
        child = child.nextSibling();
    }
}

void ObjectInfo::Create()
{
    initializeOpenGLFunctions();
    CResourceInfo::Inst()->CreateProgram("default.vsh", "default.fsh","Delay");

    Noise n;
    n.InitPerlinNoise();
    Load();

    snow_D = new QOpenGLTexture(QImage("T_snow_b_d.TGA"));
    snow_D->setMinificationFilter(QOpenGLTexture::Nearest);
    snow_D->setMagnificationFilter(QOpenGLTexture::Linear);
    snow_D->setWrapMode(QOpenGLTexture::Repeat);

    snow_N = new QOpenGLTexture(QImage("T_snow_b_n.TGA"));
    snow_N->setMinificationFilter(QOpenGLTexture::Nearest);
    snow_N->setMagnificationFilter(QOpenGLTexture::Linear);
    snow_N->setWrapMode(QOpenGLTexture::Repeat);
}

Object* ObjectInfo::CreateObject(string name)
{
    Object* obj = Helper::Inst()->CreateObject(name);
    if (obj)
    {
        obj->Create();
        obj->type = name;
        obj->id = static_cast<int>(vecObj.size());
        vecObj.push_back(obj);
    }
    return obj;
}

void ObjectInfo::Render()
{
    int screenX = RenderCommon::Inst()->GetScreenX();
    int screenY = RenderCommon::Inst()->GetScreenY();
    // 更新位置
    glClearColor(1,1,1,1);
    for(size_t i = 0;i < vecObj.size(); i++)
    {
        vecObj[i]->UpdateLocation();
    }

    // 阴影绘制
    auto shadowMapFrameBuffer = CResourceInfo::Inst()->CreateFrameBuffer("ShadowMap", screenX, screenY);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFrameBuffer->frameBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QOpenGLShaderProgram* shadowMapProgram = CResourceInfo::Inst()->GetProgram("ShadowMap");//RenderCommon::Inst()->GetShadowMapProgram();
    shadowMapProgram->bind();
    for(size_t i = 0;i < vecObj.size(); i++)
    {
        if(vecObj[i]->bCastShadow)
        {
            vecObj[i]->GenShadowMap();
        }
    }

    auto snowMapFrameBuffer = CResourceInfo::Inst()->CreateFrameBuffer("SnowMap", screenX, screenY);
    glBindFramebuffer(GL_FRAMEBUFFER, snowMapFrameBuffer->frameBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     // 生成雪的遮挡
    QOpenGLShaderProgram* snowDepthProgram = CResourceInfo::Inst()->GetProgram("SnowDepth");
    snowDepthProgram->bind();
    for(size_t i = 0;i < vecObj.size(); i++)
    {
        if(vecObj[i]->bCastShadow)
        {
             vecObj[i]->GenSnowDepth();
        }
    }

    // 写入g-buffer
    auto gFrameBuffer = CResourceInfo::Inst()->CreateFrameBuffer("GBuffer", screenX, screenY, 3);
    glBindFramebuffer(GL_FRAMEBUFFER, gFrameBuffer->frameBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(size_t i = 0;i < vecObj.size(); i++)
    {
        vecObj[i]->Render();
        vecObj[i]->Draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
     auto bloomBuffer = CResourceInfo::Inst()->CreateFrameBuffer("Bloom",screenX/2,screenY/2);
    glBindFramebuffer(GL_FRAMEBUFFER, bloomBuffer->frameBuffer);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 二次渲染
    for(size_t i = 0;i < vecObj.size(); i++)
    {
        vecObj[i]->SecondRender();
        //vecObj[i]->Draw();
    }

    // 延迟渲染部分
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    DelayRender();

    // 屏幕空间后绘制
  // for(size_t i = 0;i < vecObj.size(); i++)
  // {
  //     vecObj[i]->LateRender();
  // }
}

void ObjectInfo::DelayRender()
{
    float times = RenderCommon::Inst()->GetTime();
    int screenX = RenderCommon::Inst()->GetScreenX();
    int screenY = RenderCommon::Inst()->GetScreenY();

    auto delayProgram = CResourceInfo::Inst()->GetProgram("Delay");
    auto shadowMapFrameBuffer = CResourceInfo::Inst()->CreateFrameBuffer("ShadowMap", screenX, screenY);
    auto snowMapFrameBuffer = CResourceInfo::Inst()->CreateFrameBuffer("SnowMap", screenX, screenY);
    auto gFrameBuffer = CResourceInfo::Inst()->CreateFrameBuffer("GBuffer", screenX, screenY, 3);
     auto bloomBuffer = CResourceInfo::Inst()->CreateFrameBuffer("Bloom", screenX/2, screenY/2);

    delayProgram->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gFrameBuffer->vecTexId[EGT_NormalDepth]);
    delayProgram->setUniformValue("NormalAndDetph", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gFrameBuffer->vecTexId[EGT_Param]);
    delayProgram->setUniformValue("Param", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gFrameBuffer->vecTexId[EGT_Color]);
    delayProgram->setUniformValue("Color", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, shadowMapFrameBuffer->vecTexId[0]);
    delayProgram->setUniformValue("ShadowMap", 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, RenderCommon::Inst()->GetIrradianceMap());
    delayProgram->setUniformValue("irradianceMap", 4);

    glActiveTexture(GL_TEXTURE5);

            //RenderCommon::Inst()->GetprefilterMap()
    glBindTexture(GL_TEXTURE_CUBE_MAP,    RenderCommon::Inst()->GetprefilterMap());
  //  glBindTexture(GL_TEXTURE_CUBE_MAP,    RenderCommon::Inst()->GetHDREnvCubemap());
    delayProgram->setUniformValue("prefilterMap", 5);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, RenderCommon::Inst()->GetBRDFLutMap());
    delayProgram->setUniformValue("brdfLUT",6);

    glActiveTexture(GL_TEXTURE7);
    snow_D->bind();
    delayProgram->setUniformValue("Snow_D",7);

    glActiveTexture(GL_TEXTURE8);
    snow_N->bind();
    delayProgram->setUniformValue("Snow_N",8);

    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, snowMapFrameBuffer->vecTexId[0]);
    delayProgram->setUniformValue("SnowDepth",9);

    glActiveTexture(GL_TEXTURE10);
     glBindTexture(GL_TEXTURE_2D, bloomBuffer->vecTexId[0]);
    delayProgram->setUniformValue("Bloom",10);

    delayProgram->setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    delayProgram->setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());

    delayProgram->setUniformValue("LightMatrix",   RenderCommon::Inst()->GetLightMatrix());
    delayProgram->setUniformValue("SnowMatrix",   RenderCommon::Inst()->GetSnowDirMatrix());
    delayProgram->setUniformValue("OrthoMatrix",   RenderCommon::Inst()->GetSnowOrthoMatrix());

    QMatrix4x4 I_ViewMatrix = Camera::Inst()->GetViewMatrix().inverted();
    delayProgram->setUniformValue("Inverse_ViewMatrix", I_ViewMatrix);

    QMatrix4x4 I_ProjectMatrix = RenderCommon::Inst()->GetProjectMatrix().inverted();
    delayProgram->setUniformValue("Inverse_ProjectMatrix", I_ProjectMatrix);

    delayProgram->setUniformValue("cameraPos",Camera::Inst()->GetCameraPos());

    {
        int location = delayProgram->uniformLocation("lightPos");
        QVector3D lightPos[10];
        for(size_t i = 0;i < 10;i++)
        {
            lightPos[i] = RenderCommon::Inst()->GetLightInfo()->vecLight[i].lightPos;
        }
        delayProgram->setUniformValueArray(location,lightPos,10);
    }


    {
        int location = delayProgram->uniformLocation("Kq");
        float Kq[10];
        for(size_t i = 0;i < 10;i++)
        {
            Kq[i] = RenderCommon::Inst()->GetLightInfo()->vecLight[i].Kq;
        }
        delayProgram->setUniformValueArray(location,Kq,10,1);
    }
    {
        int location = delayProgram->uniformLocation("lightColor");
        QVector3D lightColor[10];
        for(size_t i = 0;i < 10;i++)
        {
            lightColor[i] = RenderCommon::Inst()->GetLightInfo()->vecLight[i].lightColor;
        }
        delayProgram->setUniformValueArray(location,lightColor,10);
    }

    delayProgram->setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());
    delayProgram->setUniformValue("zNear", RenderCommon::Inst()->GetZNearPlane());
    delayProgram->setUniformValue("aspect", RenderCommon::Inst()->GetAspect());
    delayProgram->setUniformValue("ScreenX", screenX);
    delayProgram->setUniformValue("ScreenY", screenY);
    delayProgram->setUniformValue("fov", RenderCommon::Inst()->GetFov());
    delayProgram->setUniformValue("bHDR", *RenderCommon::Inst()->GetHDR());
    delayProgram->setUniformValue("bGamma", *RenderCommon::Inst()->GetGamma());
    delayProgram->setUniformValue("centerPos",Camera::Inst()->GetCenterPos());
    delayProgram->setUniformValue("hdrExp",RenderCommon::Inst()->GetHDRParam());
    delayProgram->setUniformValue("bSnow",RenderCommon::Inst()->IsSnow());
    delayProgram->setUniformValue("lightPosition", RenderCommon::Inst()->GetLightLocation());
    delayProgram->setUniformValue("fTime", times/3);

    RenderCommon::Inst()->GetGeometryEngine()->drawPlane(delayProgram);
}
