#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

struct VertexData
{
    QVector3D position;
    QVector3D tangent;
    QVector3D normal;
    QVector2D texcoord;
};

class GeometryEngine : protected QOpenGLExtraFunctions
{
public:
    GeometryEngine();
    virtual ~GeometryEngine();

    void drawPlane(QOpenGLShaderProgram *program,bool bTess = false);
    void drawCube(QOpenGLShaderProgram* program,bool bTess = false);
    void drawSphere(QOpenGLShaderProgram* program,bool bTess = false);

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
