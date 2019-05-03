#include "water.h"
#include "rendercommon.h"
#include "cameramanager.h"
#include <QOpenGLShader>
#include <QTime>
#include <QDateTime>

REGISTER(Water)

void Water::Create()
{
    initializeOpenGLFunctions();
    shape = SHA_Plane;

    QString path[] =
    {
        "./right.jpg","./left.jpg","./top.jpg","./bottom.jpg",
        "./back.jpg","./front.jpg",
    };

    glGenTextures(1, &cubeTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    for(int i = 0;i < 6;i++)
    {
        QImage image(path[i]);
        const int count = image.byteCount() / 4 * 3;
        unsigned char* data = new unsigned char[count];
        int cnt = 0;
        for(int j = 0;j < count / 3; j++)
        {
            data[3 * j] = image.bits()[cnt + 2];
            data[3 * j + 1] = image.bits()[cnt + 1];
            data[3 * j + 2] = image.bits()[cnt];
            cnt += 4;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_RGB,
                     image.width(),
                     image.height(),
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     data);
        delete[] data;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    T_Water_N = new QOpenGLTexture(QImage("./T_Water_N.TGA"));
    T_Water_N->setMinificationFilter(QOpenGLTexture::Nearest);
    T_Water_N->setMagnificationFilter(QOpenGLTexture::Linear);
    T_Water_N->setWrapMode(QOpenGLTexture::Repeat);

    QOpenGLShader* vShader = new QOpenGLShader(QOpenGLShader::Vertex);
    vShader->compileSourceFile(":/water.vsh");

    QOpenGLShader* fShader = new QOpenGLShader(QOpenGLShader::Fragment);
    fShader->compileSourceFile(":/water.fsh");

    program.addShader(vShader);
    program.addShader(fShader);
    program.link();
}

void Water::Render()
{
    QDateTime time = QDateTime::currentDateTime();
    qint64 second = QDateTime::currentMSecsSinceEpoch();

    static qint64 startTime = second;

    float times;
    second = (second - startTime);
    times = (float)second / 2000;

    program.bind();

    glActiveTexture(GL_TEXTURE0);
    T_Water_N->bind();
    program.setUniformValue("T_Water_N", 0);

    glActiveTexture(GL_TEXTURE1);
    RenderCommon::Inst()->GetPerlinNoise()->bind();
    program.setUniformValue("T_Perlin_Noise_M", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP,cubeTex);
    program.setUniformValue("cubeMap", 2);

    program.setUniformValue("LightLocation", RenderCommon::Inst()->GetLightLocation());
    program.setUniformValue("totalTime", times);
    program.setUniformValue("ModelMatrix",modelMatrix);
    program.setUniformValue("IT_ModelMatrix", IT_modelMatrix);
    program.setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    program.setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());
    program.setUniformValue("zFar",RenderCommon::Inst()->GetZFarPlane());
    program.setUniformValue("cameraPos",Camera::Inst()->GetCameraPos());
}

void Water::UpdateLocation()
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(position);
    modelMatrix.rotate(270,QVector3D(1,0,0));

    modelMatrix.scale(2.0f, 2.0f, 1.0f);

    IT_modelMatrix = modelMatrix;
    IT_modelMatrix = IT_modelMatrix.inverted();
    IT_modelMatrix = IT_modelMatrix.transposed();
}

