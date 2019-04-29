#include "decal.h"
#include "cameramanager.h"

REGISTER(Decal)

void Decal::Create()
{
   // qDebug() << "Create Decal";
    initializeOpenGLFunctions();
    shape = SHA_Cube;

    QImage img("./Bush_A.TGA");
    img = img.mirrored();
    BushTex = new QOpenGLTexture(img);
    BushTex->setMinificationFilter(QOpenGLTexture::Nearest);
    BushTex->setMagnificationFilter(QOpenGLTexture::Linear);
    BushTex->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);

    AddShader("decal.vsh", "decal.fsh");

    CalculateDecalMatrix();
}

void Decal::CalculateDecalMatrix()
{
    QVector3D upDir(0, 1, 0);

    QVector3D N = QVector3D(1,0,-1);
    QVector3D U = QVector3D::crossProduct(upDir, N);
    QVector3D V = QVector3D::crossProduct(N, U);

    N.normalize();
    U.normalize();
    V.normalize();

    decalMatrix.setRow(0, {U.x(), U.y(), U.z(), -QVector3D::dotProduct(U, position)}); // x
    decalMatrix.setRow(1, {V.x(), V.y(), V.z(), -QVector3D::dotProduct(V, position)}); // y
    decalMatrix.setRow(2, {N.x(), N.y(), N.z(), -QVector3D::dotProduct(N, position)}); // z
    decalMatrix.setRow(3, {0, 0, 0, 1});
}

void Decal::SecondRender()
{
    program.bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, RenderCommon::Inst()->GetNormalAndDepthTex());
    program.setUniformValue("NormalAndDetph", 0);

    glActiveTexture(GL_TEXTURE1);
    BushTex->bind();
    program.setUniformValue("Bush", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, RenderCommon::Inst()->GetColorTex());
    program.setUniformValue("RT_Color", 2);

    //Inverse_ViewProjMatrix
    QMatrix4x4 IT_ViewProjMatrix = RenderCommon::Inst()->GetProjectMatrix() * Camera::Inst()->GetViewMatrix();
    IT_ViewProjMatrix = IT_ViewProjMatrix.inverted();
    program.setUniformValue("Inverse_ViewProjMatrix", IT_ViewProjMatrix);

    program.setUniformValue("worldToDecal", decalMatrix);
    program.setUniformValue("decalToWorld", decalMatrix.inverted());
    program.setUniformValue("decalSize",decalSize);
    program.setUniformValue("ModelMatrix",modelMatrix);
    program.setUniformValue("IT_ModelMatrix",IT_modelMatrix);
    program.setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    program.setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());
    program.setUniformValue("zFar", RenderCommon::Inst()->GetZFarPlane());

    RenderCommon::Inst()->GetGeometryEngine()->drawCube(&program);
   // qDebug() << decalMatrix << decalMatrix.inverted();
}

void Decal::UpdateLocation()
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(position);
    modelMatrix.scale(0.5,0.5,0.5);
    IT_modelMatrix = modelMatrix;
    IT_modelMatrix = IT_modelMatrix.transposed();
    IT_modelMatrix = IT_modelMatrix.inverted();
}
