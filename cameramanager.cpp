#include "cameramanager.h"
#include "rendercommon.h"
#include <algorithm>

Camera* Camera::camera = nullptr;

Camera::Camera()
{

    ViewDir = eyeLocation - lookAtLocation;
    DefaultViewDir  = ViewDir;
    UpdateViewMatrix();
    UpdateBillboard();
}

QMatrix4x4 Camera::GetRotation()
{
    QMatrix4x4 rotation;
    rotation.rotate(rotateX, QVector3D(1,0,0));
    rotation.rotate(rotateY, QVector3D(0,1,0));

    return rotation.inverted();
}

QMatrix4x4 Camera::GetYRotation()
{
    QMatrix4x4 rotation;
    rotation.rotate(rotateY, QVector3D(0,1,0));

    return rotation.inverted();
}

void Camera::UpdateBillboard()
{
   //QMatrix4x4 rotation;
   //rotation.rotate(rotateX, QVector3D(1,0,0));
   //rotation.rotate(rotateY, QVector3D(0,1,0));
   //
   //rotation = rotation.inverted();
   //billboardMatrix  =  viewMatrix;
}

void Camera::SetRotateXY(float x, float y)
{
    rotateX = x;
    rotateY = y;

    QMatrix4x4 rotation;
    rotation.rotate(rotateX, QVector3D(1,0,0));
    rotation.rotate(rotateY, QVector3D(0,1,0));

    ViewDir = DefaultViewDir * rotation;
    lookAtLocation = eyeLocation - ViewDir;

    UpdateViewMatrix();
}

QMatrix4x4 Camera::GetNoZTransViewMatrix()
{
    QMatrix4x4 ret;
    QVector3D upDir(0, 1, 0);

    QVector3D N = eyeLocation - lookAtLocation;
    QVector3D U = QVector3D::crossProduct(upDir, N);
    QVector3D V = QVector3D::crossProduct(N, U);

    N.normalize();
    U.normalize();
    V.normalize();

    ret.setRow(0, {U.x(), U.y(), U.z(), -QVector3D::dotProduct(U, eyeLocation)}); // x
    ret.setRow(1, {V.x(), V.y(), V.z(), -QVector3D::dotProduct(V, eyeLocation)}); // y
    ret.setRow(2, {N.x(), N.y(), N.z(), -5}); // z
    ret.setRow(3, {0, 0, 0, 1});

    return ret;
}

void Camera::UpdateViewMatrix()
{
    QVector3D upDir(0, 1, 0);

    QVector3D N = eyeLocation - lookAtLocation;
    QVector3D U = QVector3D::crossProduct(upDir, N);
    QVector3D V = QVector3D::crossProduct(N, U);

    N.normalize();
    U.normalize();
    V.normalize();

    viewMatrix.setRow(0, {U.x(), U.y(), U.z(), -QVector3D::dotProduct(U, eyeLocation)}); // x
    viewMatrix.setRow(1, {V.x(), V.y(), V.z(), -QVector3D::dotProduct(V, eyeLocation)}); // y
    viewMatrix.setRow(2, {N.x(), N.y(), N.z(), -QVector3D::dotProduct(N, eyeLocation)}); // z
    viewMatrix.setRow(3, {0, 0, 0, 1});

    float t = tan(RenderCommon::Inst()->GetFov()/2);
    float aspect = RenderCommon::Inst()->GetAspect();
    float zNear = RenderCommon::Inst()->GetZNearPlane();
    float zFar = RenderCommon::Inst()->GetZFarPlane();

    float zFarX = zFar * t * aspect;
    float zFarY = zFar * t;
    float zNearX = zNear * t * aspect;
    float zNearY = zNear * t;

    zNearLeftTop =  QVector3D(-zNearX, zNearY, -zNear);
    zNearLeftBottom =  QVector3D(-zNearX, -zNearY, -zNear);
    zNearRightTop =  QVector3D(zNearX, zNearY, -zNear);
    zNearRightBottom =  QVector3D(zNearX, -zNearY, -zNear);

    zFarLeftTop =  QVector3D(-zFarX, zFarY, -zFar);
    zFarLeftBottom =  QVector3D(-zFarX, -zFarY, -zFar);
    zFarRightTop =  QVector3D(zFarX, zFarY, -zFar);
    zFarRightBottom =  QVector3D(zFarX, -zFarY, -zFar);

    QMatrix4x4 inverse_viewMatrix = viewMatrix.inverted();

    GetWorldPosFromView(zNearLeftTop,inverse_viewMatrix);
    GetWorldPosFromView(zNearRightTop,inverse_viewMatrix);
    GetWorldPosFromView(zNearRightBottom,inverse_viewMatrix);
    GetWorldPosFromView(zNearLeftBottom,inverse_viewMatrix);

    GetWorldPosFromView(zFarLeftTop,inverse_viewMatrix);
    GetWorldPosFromView(zFarLeftBottom,inverse_viewMatrix);
    GetWorldPosFromView(zFarRightTop,inverse_viewMatrix);
    GetWorldPosFromView(zFarRightBottom,inverse_viewMatrix);


    //UpdateBillboard();
    //centerPos = eyeLocation + RenderCommon::Inst()->GetZNearPlane() * N;
}

void Camera::GetWorldPosFromView(QVector3D& viewPos,const QMatrix4x4& inverse_viewMatrix)
{
    QVector4D pos = inverse_viewMatrix * QVector4D(viewPos,1);
    viewPos = QVector3D(pos.x(),pos.y(),pos.z())/pos.w();
}

void Camera::MoveStep(QVector3D& dir, QVector3D step)
{
    dir.setX(dir.x() + step.x());
    dir.setZ(dir.z() + step.z());
}

void Camera::MoveLeft(float step)
{
    QVector3D upDir(0, 1, 0);
    QVector3D U = QVector3D::crossProduct(upDir, ViewDir);
    MoveStep(lookAtLocation, -step * U);
    MoveStep(eyeLocation, -step * U);

    UpdateViewMatrix();
}

void Camera::MoveRight(float step)
{
    QVector3D upDir(0, 1, 0);
    QVector3D U = QVector3D::crossProduct(upDir, ViewDir);

    MoveStep(lookAtLocation, step * U);
    MoveStep(eyeLocation, step * U);

    UpdateViewMatrix();
}

void Camera::MoveFront(float step)
{
    MoveStep(lookAtLocation, -step * ViewDir);
    MoveStep(eyeLocation, -step * ViewDir);

    UpdateViewMatrix();
}

void Camera::MoveBack(float step)
{
    MoveStep(lookAtLocation, step * ViewDir);
    MoveStep(eyeLocation, step * ViewDir);

    UpdateViewMatrix();
}

void Camera::MoveUp(float step)
{
    eyeLocation.setY(eyeLocation.y() + step);
    lookAtLocation.setY(lookAtLocation.y() + step);
    UpdateViewMatrix();
}

void Camera::MoveDown(float step)
{
    eyeLocation.setY(eyeLocation.y() - step);
    lookAtLocation.setY(lookAtLocation.y() - step);
    UpdateViewMatrix();
}

void Camera::Zoom(float step)
{
    lookAtLocation -= step * ViewDir;
    eyeLocation -= step * ViewDir;

    UpdateViewMatrix();
}

