#include "cartoon.h"
#include "rendercommon.h"
#include "cameramanager.h"

REGISTER(Cartoon)

void Cartoon::Create()
{
    initializeOpenGLFunctions();
    shape = SHA_Sphere;
{
    QOpenGLShader* vShader = new QOpenGLShader(QOpenGLShader::Vertex);
    vShader->compileSourceFile(":/cartoon.vsh");

    QOpenGLShader* fShader = new QOpenGLShader(QOpenGLShader::Fragment);
    fShader->compileSourceFile(":/cartoon.fsh");

    program.addShader(vShader);
    program.addShader(fShader);
    program.link();
}
{
    QOpenGLShader* vShader = new QOpenGLShader(QOpenGLShader::Vertex);
    vShader->compileSourceFile(":/outline.vsh");

    QOpenGLShader* fShader = new QOpenGLShader(QOpenGLShader::Fragment);
    fShader->compileSourceFile(":/outline.fsh");

    program1.addShader(vShader);
    program1.addShader(fShader);
    program1.link();
}

}

void Cartoon::Render()
{
    program.bind();


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

void Cartoon::UpdateLocation()
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(QVector3D(position.x,position.y,position.z));

    IT_modelMatrix = modelMatrix;
    IT_modelMatrix = IT_modelMatrix.transposed();
    IT_modelMatrix = IT_modelMatrix.inverted();
}

void Cartoon::PostProcess()
{
    program1.bind();


  //glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_2D, RenderCommon::Inst()->GetColorTex());
  //program1.setUniformValue("Color", 0);
  //
  //glActiveTexture(GL_TEXTURE1);
  //glBindTexture(GL_TEXTURE_2D, RenderCommon::Inst()->GetNormalTex());
  //program1.setUniformValue("Normal", 1);

   program1.setUniformValue("ModelMatrix",modelMatrix);
   program1.setUniformValue("IT_ModelMatrix",IT_modelMatrix);
   program1.setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
   program1.setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());
   program1.setUniformValue("cameraPos",Camera::Inst()->GetCameraPos());
   program1.setUniformValue("LightLocation", RenderCommon::Inst()->GetLightLocation());
   program1.setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());
   program1.setUniformValue("ScreenX", RenderCommon::Inst()->GetScreenX());
   program1.setUniformValue("ScreenY", RenderCommon::Inst()->GetScreenY());

    RenderCommon::Inst()->GetGeometryEngine()->drawPlane(&program1);

}
