#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;
struct VertexData
{
    QVector3D position;
    QVector3D tangent;
    QVector3D normal;
    QVector2D texcoord;
};

struct MeshBuffer
{
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
    int vertexNum;
    int indiceNum;

    MeshBuffer() : indexBuf(QOpenGLBuffer::IndexBuffer)
    {
        arrayBuf.create();
        indexBuf.create();
    }

    ~MeshBuffer()
    {
        arrayBuf.destroy();
        indexBuf.destroy();
    }

    void Init(VertexData* vertex, int num)
    {
        vertexNum = num;
        arrayBuf.bind();
        arrayBuf.allocate(vertex, vertexNum * static_cast<int>(sizeof(VertexData)));
    }

    void Init(GLushort* indice, int num)
    {
        indiceNum = num;
        indexBuf.bind();
        indexBuf.allocate(indice, indiceNum * static_cast<int>(sizeof(GLushort)));
    }

    void bind()
    {
        arrayBuf.bind();
        indexBuf.bind();
    }
};

class GeometryEngine
{
public:
    GeometryEngine();
    virtual ~GeometryEngine();

    bool loadObj(string path);
    void drawObj(string path,QOpenGLShaderProgram* program,bool bTess = false);

    void drawPlane(QOpenGLShaderProgram *program,bool bTess = false); // 曲面细分
    void drawCube(QOpenGLShaderProgram* program,bool bTess = false);
    void drawSphere(QOpenGLShaderProgram* program,bool bTess = false);

    void CalTangent(VertexData& vertex0, VertexData& vertex1, VertexData& vertex2);
    void CalNormalAndTangent(VertexData& vertex1, VertexData& vertex2,
                                         VertexData& vertex3);
private:
    void processNode(vector<VertexData>& vertices, vector<GLushort>& indices, aiNode *node, const aiScene *scene);
    void processMesh(vector<VertexData>& vertices, vector<GLushort>& indices, aiMesh *mesh);

    void initPlaneGeometry();
    void initSphereGeometry();
    void initCubeGeometry();

    MeshBuffer planeBuffer;
    MeshBuffer sphereBuffer;
    MeshBuffer cubeBuffer;

    map<string, MeshBuffer*> mapMesh;
};

#endif // GEOMETRYENGINE_H
