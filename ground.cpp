#include "ground.h"
#include "rendercommon.h"
#include "cameramanager.h"

REGISTER(Ground)

void Ground::Create()
{
    initializeOpenGLFunctions();
    shape = SHA_Plane;

    QOpenGLShader* vShader = new QOpenGLShader(QOpenGLShader::Vertex);
    vShader->compileSourceFile(":/gbuffer.vsh");

    QOpenGLShader* fShader = new QOpenGLShader(QOpenGLShader::Fragment);
    fShader->compileSourceFile(":/gbuffer.fsh");

    program.addShader(vShader);
    program.addShader(fShader);
    program.link();
}

void Ground::Render()
{
    program.bind();
    program.setUniformValue("ModelMatrix",modelMatrix);
    program.setUniformValue("IT_ModelMatrix",IT_modelMatrix);
    program.setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    program.setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());
    program.setUniformValue("IT_ViewMatrix", Camera::Inst()->GetViewMatrix().inverted());
    program.setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());
}

void Ground::UpdateLocation()
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(QVector3D(position.x,position.y,position.z));
    modelMatrix.rotate(270,QVector3D(1,0,0));
    modelMatrix.scale(10,10,1);

    IT_modelMatrix = modelMatrix;
    IT_modelMatrix = IT_modelMatrix.transposed();
    IT_modelMatrix = IT_modelMatrix.inverted();
}

