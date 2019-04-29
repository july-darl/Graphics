#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

struct VertexData
{
    QVector3D position;
    QVector3D tangent;
    QVector3D normal;
    QVector2D texcoord;
};

class GeometryEngine : protected QOpenGLFunctions
{
public:
    GeometryEngine();
    virtual ~GeometryEngine();

    void drawPlane(QOpenGLShaderProgram *program);
    void drawCube(QOpenGLShaderProgram* program);
    void drawSphere(QOpenGLShaderProgram* program);
    void drawIcon(QOpenGLShaderProgram* program);

    void CalNormalAndTangent(VertexData& vertex1, VertexData& vertex2,
                                         VertexData& vertex3);
private:
    void initPlaneGeometry();
    void initSphereGeometry();
    void initCubeGeometry();
    void initIconGeometry();

    QOpenGLBuffer cubeArrayBuf;
    QOpenGLBuffer cubeIndexBuf;

    QOpenGLBuffer planeArrayBuf;
    QOpenGLBuffer planeIndexBuf;

    QOpenGLBuffer sphereArrayBuf;
    QOpenGLBuffer sphereIndexBuf;

    QOpenGLBuffer iconArrayBuf;
    QOpenGLBuffer iconIndexBuf;
};

#endif // GEOMETRYENGINE_H
