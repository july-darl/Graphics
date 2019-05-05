#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "rendercommon.h"
#include "cameramanager.h"
#include "object.h"
#include "resourceinfo.h"
RenderCommon* RenderCommon::renderCommon = nullptr;

bool fIsZero(float data,float eps)
{
    return fabs(data) < eps;
}

RenderCommon::RenderCommon()
{
    geometryEngine = new GeometryEngine();

    CResourceInfo::Inst()->CreateProgram("shadowmap.vsh","shadowmap.fsh","ShadowMap");
    CResourceInfo::Inst()->CreateProgram("snowmap.vsh","shadowmap.fsh","SnowDepth");


    UpdateLightMatrix();
  //  CreateCubemap();
   // CreateHDRCubemap("test8_Ref.hdr");
    CreateHDRCubemap("./BasketballCourt_3k.hdr");
  //  CreateHDRCubemap("./Ice_Lake_Ref.hdr");

}

void RenderCommon::UpdateEnvironment()
{
    QString name[] =
    {
       "test8_Ref.hdr",
   //     "BasketballCourt_3k.hdr",
       "Tropical_Beach_3k.hdr",
        "Mans_Outside_2k.hdr",
        "Summi_Pool_3k.hdr",
        "Chelsea_Stairs_3k.hdr",
        "Circus_Backstage_3k.hdr",
        "Footprint_Court_2k.hdr",
        "LA_Downtown_Afternoon_Fishing_3k.hdr",
    };
    static int count = 0;
    int max = 8;
    count = (count + 1) % max;
    CreateHDRCubemap(name[count]);
}

void RenderCommon::CreateCubemap()
{
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();
    QString path[] =
    {
        "./right.jpg","./left.jpg","./top.jpg","./bottom.jpg",
        "./back.jpg","./front.jpg",
    };

    GLuint envCubemap;
    gl->glGenTextures(1, &envCubemap);
    gl->glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for(unsigned int i = 0;i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        QImage image(path[i]);
        image = image.convertToFormat(QImage::Format_RGB888);
        gl->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
            image.width(),image.height(),
                     0, GL_RGB, GL_UNSIGNED_BYTE,image.bits());
    }
    gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void RenderCommon::CreateHDRCubemap(QString fileName)
{
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();

    gl->glDisable(GL_CULL_FACE);
    gl->glDeleteTextures(1, &hdrCubemap);
    gl->glDeleteTextures(1, &hdrTexture);
    gl->glDeleteTextures(1, &irradianceMap);
    gl->glDeleteTextures(1, &prefilterMap);
    gl->glDeleteTextures(1, &prefilterMap);

    QOpenGLShaderProgram* cubemapProgram    = CResourceInfo::Inst()->CreateProgram("cubemap.vsh","cubemap.fsh");
    QOpenGLShaderProgram* irradianceProgram = CResourceInfo::Inst()->CreateProgram("irradiancemap.vsh","irradiancemap.fsh");
    QOpenGLShaderProgram* prefilterProgram  = CResourceInfo::Inst()->CreateProgram("prefilter.vsh","prefilter.fsh");
    QOpenGLShaderProgram* brdfLUTProgram    = CResourceInfo::Inst()->CreateProgram("brdflut.vsh","brdflut.fsh");

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(fileName.toStdString().c_str(),&width, &height, &nrComponents, 0);
    //float* data = stbi_loadf("./Tropical_Beach_3k.hdr",&width, &height, &nrComponents, 0);
    if(data)
    {
        qDebug() << "success load hdr image";
        GLuint          captureFBO;
        QMatrix4x4      captureViews[6];

        QMatrix4x4 projection;
        projection.perspective(90,1.0f,0.1f,10.0f);
        captureViews[0].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f)); // right
        captureViews[1].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(-1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f)); // left
        captureViews[2].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  1.0f,  0.0f), QVector3D(0.0f,  0.0f,  1.0f)); // top
        captureViews[3].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f, -1.0f,  0.0f), QVector3D(0.0f,  0.0f, -1.0f)); // bottom
        captureViews[4].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  0.0f,  1.0f), QVector3D(0.0f, -1.0f,  0.0f)); // back
        captureViews[5].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D( 0.0f,  0.0f, -1.0f), QVector3D(0.0f, -1.0f,  0.0f)); // front

        gl->glGenTextures(1, &hdrCubemap);
        gl->glBindTexture(GL_TEXTURE_CUBE_MAP, hdrCubemap);
        for(unsigned int i = 0;i < 6; ++i)
        {
            // note that we store each face with 16 bit floating point values
            gl->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // ===================================
        gl->glGenTextures(1, &hdrTexture);
        gl->glBindTexture(GL_TEXTURE_2D, hdrTexture);
        gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        gl->glGenFramebuffers(1, &captureFBO);

        cubemapProgram->bind();

        cubemapProgram->setUniformValue("ProjectMatrix",projection);
        gl->glActiveTexture(GL_TEXTURE0);
        gl->glBindTexture(GL_TEXTURE_2D, hdrTexture);
        cubemapProgram->setUniformValue("equirectangularMap", 0);

        gl->glViewport(0, 0, 512, 512);
        // don't forget to configure the viewport to the capture dimensions.
        gl->glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for( unsigned int i = 0; i < 6; ++i)
        {
            cubemapProgram->setUniformValue("ViewMatrix", captureViews[i]);
            gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, hdrCubemap, 0);
            gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            GetGeometryEngine()->drawCube(cubemapProgram);
        }

       gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

       // ===============================


       gl->glGenTextures(1, &irradianceMap);
       gl->glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
       for (unsigned int i = 0; i < 6; ++i)
       {
           gl->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
                        GL_RGB, GL_FLOAT, nullptr);
       }
       gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
       gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
       gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

       gl->glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
       irradianceProgram->bind();

       irradianceProgram->setUniformValue("ProjectMatrix",projection);
       gl->glActiveTexture(GL_TEXTURE0);
       gl->glBindTexture(GL_TEXTURE_CUBE_MAP, hdrCubemap);
       irradianceProgram->setUniformValue("environmentMap", 0);

       gl->glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
       gl->glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
       for (unsigned int i = 0; i < 6; ++i)
       {
           irradianceProgram->setUniformValue("ViewMatrix", captureViews[i]);
           gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
           gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

           GetGeometryEngine()->drawCube(irradianceProgram);
       }
       gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

       // ===============================

       int specularSize = 128;
       gl->glGenTextures(1, &prefilterMap);
       gl->glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
       for(unsigned int i = 0; i < 6; ++i)
       {
           gl->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, specularSize, specularSize, 0,
               GL_RGB, GL_FLOAT, nullptr);
       }
       gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
       gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
       gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
       gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

       gl->glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

       prefilterProgram->setUniformValue("ProjectMatrix",projection);
       gl->glActiveTexture(GL_TEXTURE0);
       gl->glBindTexture(GL_TEXTURE_CUBE_MAP, hdrCubemap);
       prefilterProgram->setUniformValue("environmentMap", 0);

       gl->glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
       unsigned int maxMipLevels = 5;
       for(unsigned int mip = 0; mip < maxMipLevels; ++mip)
       {
           // resize framebuffer according to mip-level size.
           unsigned int mipWidth = specularSize * std::pow(0.5, mip);
           unsigned int mipHeight = specularSize * std::pow(0.5, mip);

           gl->glViewport(0, 0, mipWidth, mipHeight);

           float roughness = (float)mip / (float)(maxMipLevels - 1);
           prefilterProgram->setUniformValue("roughness", roughness);
           for(unsigned int i = 0;i < 6; ++i)
           {
               prefilterProgram->setUniformValue("ViewMatrix", captureViews[i]);
               gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

               gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
               GetGeometryEngine()->drawCube(prefilterProgram);
           }
       }
       gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

       // ===============================

       gl->glGenTextures(1, &brdfLUTTexture);

       // pre-allocate enough memory for the LUT texture.
       gl->glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
       gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
       gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
       gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

       gl->glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

       gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

       gl->glViewport(0, 0, 512, 512);
       brdfLUTProgram->bind();
       gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       GetGeometryEngine()->drawPlane(brdfLUTProgram);

       gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

       delete cubemapProgram;
       delete irradianceProgram;
       delete prefilterProgram;
       delete brdfLUTProgram;
    }
    else
    {
        qDebug() << "Failed to load HDR image.";
    }
    gl->glEnable(GL_CULL_FACE);
}

void RenderCommon::UpdateScreenXY(int w, int h)
{
    screenX = w;
    screenY = h;

    aspect = float(w) / float(h ? h : 1);

    projection.setToIdentity();
    projection.perspective(fov, aspect, zNear, zFar);
}

void RenderCommon::UpdateSnowDepth()
{
    qDebug() << "UpdateSnowDepth" ;
    QVector3D upDir(0, 0, -1);
    QVector3D N = QVector3D(0, 1, 0);
    QVector3D U = QVector3D::crossProduct(upDir, N);
    QVector3D V = QVector3D::crossProduct(N, U);

    int height = 1000;
    QVector3D pos = QVector3D(0,height,0);
    N.normalize();
    U.normalize();
    V.normalize();

    snowDepthMat.setRow(0, {U.x(), U.y(), U.z(), -QVector3D::dotProduct(U, pos)}); // x
    snowDepthMat.setRow(1, {V.x(), V.y(), V.z(), -QVector3D::dotProduct(V, pos)}); // y
    snowDepthMat.setRow(2, {N.x(), N.y(), N.z(), -QVector3D::dotProduct(N, pos)}); // z
    snowDepthMat.setRow(3, {0, 0, 0, 1});

    QVector3D p1 = Camera::Inst()->GetZFarLeftBottomPos();
    QVector3D p2 = Camera::Inst()->GetZFarLeftTopPos();
    QVector3D p3 = Camera::Inst()->GetZFarRightBottomPos();
    QVector3D p4 = Camera::Inst()->GetZFarRightTopPos();
    QVector3D p5 = Camera::Inst()->GetZNearLeftBottomPos();
    QVector3D p6 = Camera::Inst()->GetZNearLeftTopPos();
    QVector3D p7 = Camera::Inst()->GetZNearRightBottomPos();
    QVector3D p8 = Camera::Inst()->GetZNearRightTopPos();

  //  qDebug() << p1 << p2 << p3 << p4 << p5 << p6 << p7 << p8;
   // qDebug() << snowDepthMat;
    float XArr[] =
    {
        p1.x(),p2.x(),p3.x(),p4.x(),p5.x(),p6.x(),p7.x(),p8.x()
    };
    float YArr[] =
    {
        p1.y(),p2.y(),p3.y(),p4.y(),p5.y(),p6.y(),p7.y(),p8.y()
    };

    float xMax = -1;
    for(int i=0;i < 8;i++)
    {
         xMax = max(xMax,XArr[i]);
    }

    float xMin = 10000;
    for(int i=0;i < 8;i++)
    {
         xMin = min(xMin,XArr[i]);
    }

    float yMax = -1;
    for(int i=0;i < 8;i++)
    {
         yMax = max(yMax,YArr[i]);
    }

    float yMin = 10000;
    for(int i=0;i < 8;i++)
    {
         yMin = min(yMin,YArr[i]);
    }

   // qDebug() << "xMin = " << xMin;
   // qDebug() << "xMax = " << xMax;
   // qDebug() << "yMin = " << yMin;
   // qDebug() << "yMax = " << yMax;
    snowOrtho.setToIdentity();
    snowOrtho.ortho(-10, 10, -10, 10, 1, height);
}

void RenderCommon::UpdateLightMatrix()
{
    QVector3D upDir(0, 1, 0);
    QVector3D N = lightLocation;
    QVector3D U = QVector3D::crossProduct(upDir, N);
    QVector3D V = QVector3D::crossProduct(N, U);

    QVector3D pos = lightLocation;
    N.normalize();
    U.normalize();
    V.normalize();

    lightMatrix.setRow(0, {U.x(), U.y(), U.z(), -QVector3D::dotProduct(U, pos)}); // x
    lightMatrix.setRow(1, {V.x(), V.y(), V.z(), -QVector3D::dotProduct(V, pos)}); // y
    lightMatrix.setRow(2, {N.x(), N.y(), N.z(), -QVector3D::dotProduct(N, pos)}); // z
    lightMatrix.setRow(3, {0, 0, 0, 1});

}

#include <QDateTime>
float RenderCommon::GetTime()
{
    qint64 second = QDateTime::currentMSecsSinceEpoch();

    static qint64 startTime = second;

    second = second - startTime;
    return static_cast<float>(second) / 2000;
}
