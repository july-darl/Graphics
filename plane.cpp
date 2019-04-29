#include "plane.h"
#include "rendercommon.h"
#include "cameramanager.h"

REGISTER(Plane)

void Plane::Create()
{
    initializeOpenGLFunctions();
    shape = SHA_Sphere;
    T_Brick_N = new QOpenGLTexture(QImage("./T_Brick_Clay_New_N.TGA"));
    T_Brick_N->setMinificationFilter(QOpenGLTexture::Nearest);
    T_Brick_N->setMagnificationFilter(QOpenGLTexture::Linear);
    T_Brick_N->setWrapMode(QOpenGLTexture::Repeat);

    T_Brick_D = new QOpenGLTexture(QImage("./T_Brick_Clay_New_D.TGA"));
    T_Brick_D->setMinificationFilter(QOpenGLTexture::Nearest);
    T_Brick_D->setMagnificationFilter(QOpenGLTexture::Linear);
    T_Brick_D->setWrapMode(QOpenGLTexture::Repeat);

    T_Brick_M = new QOpenGLTexture(QImage("./T_Brick_Clay_New_M.TGA"));
    T_Brick_M->setMinificationFilter(QOpenGLTexture::Nearest);
    T_Brick_M->setMagnificationFilter(QOpenGLTexture::Linear);
    T_Brick_M->setWrapMode(QOpenGLTexture::Repeat);

    QOpenGLShader* vShader = new QOpenGLShader(QOpenGLShader::Vertex);
    vShader->compileSourceFile(":/parallex.vsh");

    QOpenGLShader* fShader = new QOpenGLShader(QOpenGLShader::Fragment);
    fShader->compileSourceFile(":/parallex.fsh");

    program.addShader(vShader);
    program.addShader(fShader);
    program.link();

}

void Plane::Render()
{
    program.bind();

    glActiveTexture(GL_TEXTURE0);
    T_Brick_N->bind();
    program.setUniformValue("brick_N",0);

    glActiveTexture(GL_TEXTURE1);
    T_Brick_D->bind();
    program.setUniformValue("brick_D",1);

    glActiveTexture(GL_TEXTURE2);
    T_Brick_M->bind();
    program.setUniformValue("brick_M",2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, RenderCommon::Inst()->GetShadowMapTex());
    program.setUniformValue("shadowMap",3);

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

void Plane::UpdateLocation()
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(QVector3D(position.x,position.y,position.z));

    IT_modelMatrix = modelMatrix;
    IT_modelMatrix = IT_modelMatrix.transposed();
    IT_modelMatrix = IT_modelMatrix.inverted();
}
