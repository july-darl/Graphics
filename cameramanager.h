#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H
#include <QVector3D>
#include <QMatrix4x4>

class Camera
{
private:
    QVector3D DefaultViewDir;
    QVector3D ViewDir = QVector3D(0, 0, 1);
    float rotateX = 0.0f;
    float rotateY = 0.0f;

    QVector3D zNearLeftTop;
    QVector3D zNearRightTop;
    QVector3D zFarLeftTop;
    QVector3D zFarRightTop;

    QVector3D zNearLeftBottom;
    QVector3D zNearRightBottom;
    QVector3D zFarLeftBottom;
    QVector3D zFarRightBottom;

    QVector3D centerPos;
    QVector3D eyeLocation = QVector3D(0, 0, 5);
    QVector3D lookAtLocation = QVector3D(0, 0, 0);
    QMatrix4x4 viewMatrix;
    QMatrix4x4 billboardMatrix;

    void GetWorldPosFromView(QVector3D& viewPos,const QMatrix4x4& inverse_viewMatrix);

    void UpdateBillboard();
    void MoveStep(QVector3D& dir, QVector3D step);
    Camera();

public:
    static Camera* camera;
    static Camera* Inst() {
        if(!camera) {
            camera = new Camera();
        }
        return camera;
    }

    void SetRotateXY(float x, float y);
    float GetRotateX() { return rotateX; }
    float GetRotateY() { return rotateY; }

    void MoveLeft(float step);
    void MoveRight(float step);
    void MoveFront(float step);
    void MoveBack(float step);
    void MoveUp(float step);
    void MoveDown(float step);
    void Zoom(float step);

    void SetDest(const QVector3D& dest, float offset = 5.0f)
    {
        lookAtLocation = dest;
        eyeLocation = lookAtLocation + ViewDir * offset; // 默认就是5那么长啦
        UpdateViewMatrix();
    }

    void UpdateViewMatrix();
    const QMatrix4x4& GetViewMatrix() { return viewMatrix; }
    const QMatrix4x4& GetBillboardMatrix() { return billboardMatrix; }
    QMatrix4x4 GetNoZTransViewMatrix();
    const QVector3D& GetCameraPos() { return eyeLocation; }
    const QVector3D& GetViewDir() { return ViewDir; }

    const QVector3D& GetZNearLeftTopPos() { return zNearLeftTop;}
    const QVector3D& GetZNearRightTopPos() { return zNearRightTop;}
    const QVector3D& GetZFarLeftTopPos() { return zFarLeftTop;}
    const QVector3D& GetZFarRightTopPos() { return zFarRightTop;}

    const QVector3D& GetZNearLeftBottomPos() { return zNearLeftBottom;}
    const QVector3D& GetZNearRightBottomPos() { return zNearRightBottom;}
    const QVector3D& GetZFarLeftBottomPos() { return zFarLeftBottom;}
    const QVector3D& GetZFarRightBottomPos() { return zFarRightBottom;}

    QMatrix4x4 GetRotation();
    QMatrix4x4 GetYRotation();
    QVector3D GetCenterPos() { return centerPos; }
};

#endif // CAMERAMANAGER_H
