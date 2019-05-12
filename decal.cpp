#include "decal.h"
#include "cameramanager.h"
#include "resourceinfo.h"

REGISTER(Decal)

void Decal::Create()
{
    program = CResourceInfo::Inst()->CreateProgram("decal.vsh","decal.fsh","decal");


}

void Decal::CalculateDecalMatrix()
{
    QVector3D pos(position.x,position.y,position.z);
    QVector3D upDir(0, -1, 0);

    QVector3D N = QVector3D(cos(rotation.y),0,sin(rotation.y));
    QVector3D U = QVector3D::crossProduct(upDir, N);
    QVector3D V = QVector3D::crossProduct(N, U);

    N.normalize();
    U.normalize();
    V.normalize();

    decalMatrix.setRow(0, {U.x(), U.y(), U.z(), -QVector3D::dotProduct(U, pos)}); // x
    decalMatrix.setRow(1, {V.x(), V.y(), V.z(), -QVector3D::dotProduct(V, pos)}); // y
    decalMatrix.setRow(2, {N.x(), N.y(), N.z(), -QVector3D::dotProduct(N, pos)}); // z
    decalMatrix.setRow(3, {0, 0, 0, 1});
}

void Decal::DecalRender()
{
   // return;
    CalculateDecalMatrix();
    int screenX = RenderCommon::Inst()->GetScreenX();
    int screenY = RenderCommon::Inst()->GetScreenY();

    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();
    auto gFrameBuffer = CResourceInfo::Inst()->CreateFrameBuffer("GBuffer", screenX, screenY, 3);

    program->bind();

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_2D, gFrameBuffer->vecTexId[EGT_NormalDepth]);
    program->setUniformValue("NormalAndDetph", 0);

    gl->glActiveTexture(GL_TEXTURE1);
    gl->glBindTexture(GL_TEXTURE_2D, decalTex);
    program->setUniformValue("Image", 1);

    gl->glActiveTexture(GL_TEXTURE2);
    gl->glBindTexture(GL_TEXTURE_2D, gFrameBuffer->vecTexId[EGT_Color]);
    program->setUniformValue("RT_Color", 2);

    //Inverse_ViewProjMatrix
    QMatrix4x4 IT_ViewMatrix = Camera::Inst()->GetViewMatrix();
    IT_ViewMatrix = IT_ViewMatrix.inverted();
    program->setUniformValue("Inverse_ViewMatrix", IT_ViewMatrix);

    program->setUniformValue("worldToDecal", decalMatrix);
    program->setUniformValue("decalToWorld", decalMatrix.inverted());
    program->setUniformValue("decalSize",decalSize);
    program->setUniformValue("ModelMatrix",modelMatrix);
    program->setUniformValue("IT_ModelMatrix",IT_modelMatrix);
    program->setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    program->setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());
    program->setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());
    program->setUniformValue("fov", RenderCommon::Inst()->GetFov());
    program->setUniformValue("aspect", RenderCommon::Inst()->GetAspect());

    RenderCommon::Inst()->GetGeometryEngine()->drawCube(program);
}
