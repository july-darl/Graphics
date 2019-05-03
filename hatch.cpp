#include "hatch.h"
#include "rendercommon.h"
#include "cameramanager.h"

REGISTER(Hatch)
void Hatch::Create()
{
    initializeOpenGLFunctions();
    shape = SHA_Sphere;
    glGenTextures(6, p);

    for(int i = 0;i < 6;i++)
    {
        QImage img[4];
        for(int j = 0;j < 4;j++)
        {
            img[j].load("./p" + QString::number(i+1) + "_" + QString::number(j) + ".png");
            img[j] = img[j].convertToFormat(QImage::Format_RGBA8888);
        }

        glBindTexture(GL_TEXTURE_2D, p[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[0].width(), img[0].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img[0].bits());
        glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, img[1].width(), img[1].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img[1].bits());
        glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA, img[2].width(), img[2].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img[2].bits());
        glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA, img[3].width(), img[3].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img[3].bits());
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    QOpenGLShader* vShader = new QOpenGLShader(QOpenGLShader::Vertex);
    vShader->compileSourceFile(":/hatch.vsh");

    QOpenGLShader* fShader = new QOpenGLShader(QOpenGLShader::Fragment);
    fShader->compileSourceFile(":/hatch.fsh");

    program.addShader(vShader);
    program.addShader(fShader);
    program.link();

}

void Hatch::Render()
{
    program.bind();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, p[0]);
    program.setUniformValue("p1",1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, p[1]);
    program.setUniformValue("p2",2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, p[2]);
    program.setUniformValue("p3",3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, p[3]);
    program.setUniformValue("p4",4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, p[4]);
    program.setUniformValue("p5",5);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, p[5]);
    program.setUniformValue("p6",6);

    program.setUniformValue("ModelMatrix",modelMatrix);
    program.setUniformValue("IT_ModelMatrix",IT_modelMatrix);
    program.setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    program.setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());
    program.setUniformValue("cameraPos",Camera::Inst()->GetCameraPos());
    program.setUniformValue("LightLocation", RenderCommon::Inst()->GetLightLocation());
    program.setUniformValue("LightMatrix",RenderCommon::Inst()->GetLightMatrix());
    program.setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());
    program.setUniformValue("ScreenX", RenderCommon::Inst()->GetScreenX());
    program.setUniformValue("ScreenY", RenderCommon::Inst()->GetScreenY());
}

void Hatch::UpdateLocation()
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(position);

    IT_modelMatrix = modelMatrix;
    IT_modelMatrix = IT_modelMatrix.transposed();
    IT_modelMatrix = IT_modelMatrix.inverted();
}
