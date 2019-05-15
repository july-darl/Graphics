
#include "geometryengine.h"
#include "resourceinfo.h"
#include <QVector2D>
#include <QVector3D>
#include <QVector>


GeometryEngine::GeometryEngine()
{
    initPlaneGeometry();
    initSphereGeometry();
    initCubeGeometry();
}

GeometryEngine::~GeometryEngine()
{

}

bool GeometryEngine::loadObj(string path, Model*& pModel)
{
    if(mapModel.find(path) != mapModel.end())
    {
        return true;
    }

    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        qDebug() << "ERROR::ASSIMP::" << import.GetErrorString() ;
        return false;
    }
    string directory = path.substr(0, path.find_last_of('/'));

    processNode(path, scene->mRootNode, scene);
    pModel = &mapModel[path];
    return true;
}

void GeometryEngine::drawObj(MeshBuffer* meshBuffer, QOpenGLShaderProgram* program,bool bTess)
{
    meshBuffer->bind();

    auto gl = QOpenGLContext::currentContext()->extraFunctions();

    int offset = 0;

    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int tangentLocation = program->attributeLocation("a_tangent");
    program->enableAttributeArray(tangentLocation);
    program->setAttributeBuffer(tangentLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    if(bTess)
    {
        gl->glPatchParameteri(GL_PATCH_VERTICES, 3);
        gl->glDrawElements(GL_PATCHES, meshBuffer->indiceNum, GL_UNSIGNED_SHORT, nullptr);
    }
    else
    {
        gl->glDrawElements(GL_TRIANGLES, meshBuffer->indiceNum, GL_UNSIGNED_SHORT, nullptr);
    }
}


void GeometryEngine::drawObj(string path,QOpenGLShaderProgram* program,bool bTess)
{
    Model* pModel;
    if(mapModel.find(path) == mapModel.end())
    {
        if(!loadObj(path, pModel))
        {
            return;
        }
    }

    auto vecMesh = mapModel[path];
    for(size_t i = 0;i < vecMesh.Count();i++)
    {
        auto meshBuffer = vecMesh.GetMeshBuffer(i);
        drawObj(meshBuffer,program, bTess);
    }
}

void GeometryEngine::processMesh(vector<VertexData>& vertices, vector<GLushort>& indices, QOpenGLTexture*& albedo, aiMesh *mesh, const aiScene *scene)
{
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        VertexData vertex;
        if(mesh->mVertices)
        {
            vertex.position = QVector3D(mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z);
        }
        if(mesh->mTextureCoords[0])
        {
            vertex.texcoord = QVector2D(mesh->mTextureCoords[0][i].x,mesh->mTextureCoords[0][i].y);
        }
        if(mesh->mNormals)
        {
            vertex.normal = QVector3D(mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z);
            vertex.normal.normalized();
        }
        if(mesh->mTangents)
        {
            vertex.tangent = QVector3D(mesh->mTangents[i].x,mesh->mTangents[i].y,mesh->mTangents[i].z);
        }
        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(static_cast<GLushort>(face.mIndices[j]));
        }
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    for(unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
    {
        aiString str;
        material->GetTexture(aiTextureType_DIFFUSE, i, &str);
        qDebug() << i << str.C_Str();
        albedo = CResourceInfo::Inst()->CreateTexture(str.C_Str());
    }
}



void GeometryEngine::processNode(const string& path, aiNode *node, const aiScene *scene)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        vector<VertexData> vertices;
        vector<GLushort> indices;
        QOpenGLTexture* albedo = nullptr;
        aiMesh *aimesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(vertices, indices, albedo, aimesh, scene);

        MeshBuffer* meshBuffer = new MeshBuffer();
        for(size_t i = 0;i < indices.size() / 3; i++)
        {
            CalTangent(vertices[indices[3 * i]],vertices[indices[3 * i + 1]],vertices[indices[3 * i + 2]]);
        }

        meshBuffer->Init(vertices.data(),static_cast<int>(vertices.size()));
        meshBuffer->Init(indices.data(),static_cast<int>(indices.size()));

        Mesh* mesh = new Mesh();
        mesh->name = node->mName.C_Str();
        mesh->buffer = meshBuffer;
        mesh->albedo = albedo;
        mapModel[path].Push(mesh);
    }

    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(path, node->mChildren[i], scene);
    }
}

void GeometryEngine::initPlaneGeometry()
{
    const int vertexNum = 4;
    VertexData Vertices[vertexNum];
    Vertices[0].position = QVector3D(-1.0f, -1.0f, 0.0f); Vertices[0].texcoord = QVector2D(0,0);
    Vertices[1].position = QVector3D(1.0f,  -1.0f, 0.0f); Vertices[1].texcoord = QVector2D(1,0);
    Vertices[2].position = QVector3D(1.0f,  1.0f,  0.0f); Vertices[2].texcoord = QVector2D(1,1);
    Vertices[3].position = QVector3D(-1.0f, 1.0f,  0.0f); Vertices[3].texcoord = QVector2D(0,1);

    GLushort Indices[] = {
           0, 1, 2, 2, 3, 0
    };

    int AdjoinCount[vertexNum];
    for(int i = 0;i<vertexNum;i++)
    {
        AdjoinCount[i] = 0;
    }

    for(int i = 0;i < 2;i++)
    {
        CalNormalAndTangent(Vertices[Indices[3 * i]],Vertices[Indices[3 * i + 1]],Vertices[Indices[3 * i + 2]]);
    }

    planeBuffer.Init(Vertices, 4);
    planeBuffer.Init(Indices, 6);
}


void GeometryEngine::initSphereGeometry()
{
    const int latitude = 30;
    const int longtitude = 30;
    const float PI = 3.14159f;
    QVector<VertexData> Vertices;

    for(int i = 0;i <= latitude;i++) // 纬度
    {
        float y = cos(i * PI / latitude);
        for(int j = 0;j <= longtitude; j++) // 经度
        {
            VertexData vertex;
            float delta = sin(i * PI / latitude);
            float x = delta * sin(2 * j * PI / longtitude);
            float z = delta * cos(2 * j * PI / longtitude);
            vertex.position = QVector3D(x,y,z);

            float v = 1.0 * (y + 1) / 4 ;
            float u = 1.0 * j / longtitude ;

           // float u = 2.0 * j / (longtitude) ;
           // if(u > 1) u = 2 - u;
            vertex.texcoord = QVector2D(u,v);

            Vertices.push_back(vertex);
        }
    }

    QVector<GLushort> Indices;

    for(int i = 0;i < latitude;i++) // 纬度
    {
        for(int j = 0;j < longtitude; j++) // 经度
        {
            GLushort p1 = (longtitude + 1) * i + j;
            GLushort p2 = (longtitude + 1) * (i + 1) + j;
            GLushort p3 = (longtitude + 1) * (i + 1) + j + 1;
            GLushort p4 = (longtitude + 1) * i + j + 1;

            if(Vertices[p1].position != Vertices[p4].position)
            {
                Indices.push_back(p4);
                Indices.push_back(p1);
                Indices.push_back(p2);
            }
            Indices.push_back(p4);
            Indices.push_back(p2);
            Indices.push_back(p3);
        }
    }

    for(int i=0;i<Indices.size()/3;i++)
    {
        CalNormalAndTangent(Vertices[Indices[3 * i]],Vertices[Indices[3 * i + 1]],Vertices[Indices[3 * i + 2]]);
    }

    sphereBuffer.Init(Vertices.data(), Vertices.size());
    sphereBuffer.Init(Indices.data(), Indices.size());
}


void GeometryEngine::initCubeGeometry()
{

    VertexData Vertices[] = {
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector3D(0,0,0),QVector3D(0,0,0) ,QVector2D(0,0)}, // v0
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector3D(0,0,0),QVector3D(0,0,0),QVector2D(1,0)}, // v1
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector3D(0,0,0),QVector3D(0,0,0) ,QVector2D(0,1)}, // v2
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector3D(0,0,0),QVector3D(0,0,0),QVector2D(1,1)}, // v3

        {QVector3D( 1.0f, -1.0f,  1.0f), QVector3D(0,0,0),QVector3D(0,0,0) ,QVector2D(0,0)}, // v4
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector3D(0,0,0),QVector3D(0,0,0) ,QVector2D(1,0)}, // v5
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector3D(0,0,0),QVector3D(0,0,0)  ,QVector2D(0,1)}, // v6
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector3D(0,0,0),QVector3D(0,0,0) ,QVector2D(1,1)}, // v7

        {QVector3D( 1.0f, -1.0f, -1.0f), QVector3D(0,0,0), QVector3D(0,0,0),QVector2D(0,0) }, // v8
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector3D(0,0,0), QVector3D(0,0,0) ,QVector2D(1,0)}, // v9
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector3D(0,0,0), QVector3D(0,0,0),QVector2D(0,1) }, // v10
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector3D(0,0,0), QVector3D(0,0,0) ,QVector2D(1,1) }, // v11

        {QVector3D(-1.0f, -1.0f, -1.0f), QVector3D(0,0,0), QVector3D(0,0,0),QVector2D(0,0) }, // v12
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector3D(0,0,0), QVector3D(0,0,0) ,QVector2D(1,0) }, // v13
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector3D(0,0,0), QVector3D(0,0,0),QVector2D(0,1) }, // v14
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector3D(0,0,0), QVector3D(0,0,0) ,QVector2D(1,1) }, // v15

        {QVector3D(-1.0f, -1.0f, -1.0f), QVector3D(0,0,0),QVector3D(0,0,0),QVector2D(0,0) }, // v16
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector3D(0,0,0),QVector3D(0,0,0),QVector2D(1,0) }, // v17
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector3D(0,0,0),QVector3D(0,0,0),QVector2D(0,1) }, // v18
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector3D(0,0,0),QVector3D(0,0,0),QVector2D(1,1) }, // v19

        {QVector3D(-1.0f,  1.0f,  1.0f), QVector3D(0,0,0),QVector3D(0,0,0) ,QVector2D(0,0) }, // v20
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector3D(0,0,0),QVector3D(0,0,0) ,QVector2D(1,0) }, // v21
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector3D(0,0,0),QVector3D(0,0,0) ,QVector2D(0,1) }, // v22
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector3D(0,0,0),QVector3D(0,0,0) ,QVector2D(1,1) }, // v23
    };

    GLushort Indices[] = {
         0,  1,  3, 3, 2, 0,
         4,  5,  7, 7, 6, 4,
        8, 9, 11, 11, 10, 8,
        12, 13, 15, 15, 14, 12,
        16, 17, 19, 19, 18, 16,
        20, 21, 23, 23, 22, 20,
    };

    for(int i = 0;i < 12;i++)
    {
        CalNormalAndTangent(Vertices[Indices[3 * i]],Vertices[Indices[3 * i + 1]],Vertices[Indices[3 * i + 2]]);
    }

    cubeBuffer.Init(Vertices, 24);
    cubeBuffer.Init(Indices, 36);
}

void GeometryEngine::drawCube(QOpenGLShaderProgram *program, bool bTess)
{
    auto gl = QOpenGLContext::currentContext()->extraFunctions();

    cubeBuffer.bind();

    if(bTess)
        gl->glPatchParameteri(GL_PATCH_VERTICES, 3);

    int offset = 0;

    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int tangentLocation = program->attributeLocation("a_tangent");
    program->enableAttributeArray(tangentLocation);
    program->setAttributeBuffer(tangentLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    if(bTess)
    {
        gl->glPatchParameteri(GL_PATCH_VERTICES,3);
        gl->glDrawElements(GL_PATCHES, 36, GL_UNSIGNED_SHORT, nullptr);
    }
    else
    {
        gl->glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
    }
}

void GeometryEngine::drawPlane(QOpenGLShaderProgram *program, bool bTess)
{
    auto gl = QOpenGLContext::currentContext()->extraFunctions();

    planeBuffer.bind();

    int offset = 0;

    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int tangentLocation = program->attributeLocation("a_tangent");
    program->enableAttributeArray(tangentLocation);
    program->setAttributeBuffer(tangentLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));


    if(bTess)
    {
        gl->glPatchParameteri(GL_PATCH_VERTICES, 3);
        gl->glDrawElements(GL_PATCHES, 6, GL_UNSIGNED_SHORT, nullptr);
       // gl->glDrawArrays(GL_PATCHES, 0,6);
    }
    else
    {
        gl->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    }

}

void GeometryEngine::drawSphere(QOpenGLShaderProgram* program, bool bTess)
{
    auto gl = QOpenGLContext::currentContext()->extraFunctions();

    sphereBuffer.bind();

    int offset = 0;
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int tangentLocation = program->attributeLocation("a_tangent");
    program->enableAttributeArray(tangentLocation);
    program->setAttributeBuffer(tangentLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    if(bTess)
    {
        gl->glPatchParameteri(GL_PATCH_VERTICES, 3);
        gl->glDrawElements(GL_PATCHES, 54000, GL_UNSIGNED_SHORT, nullptr);
    }
    else
    {
        gl->glDrawElements(GL_TRIANGLES, 54000, GL_UNSIGNED_SHORT, nullptr);
    }
}


void GeometryEngine::CalTangent(VertexData& vertex0, VertexData& vertex1, VertexData& vertex2)
{
    float u0 = vertex0.texcoord.x();
    float v0 = vertex0.texcoord.y();

    float u1 = vertex1.texcoord.x();
    float v1 = vertex1.texcoord.y();

    float u2 = vertex2.texcoord.x();
    float v2 = vertex2.texcoord.y();

    float t1 = u1 - u0;
    float b1 = v1 - v0;

    float t2 = u2 - u0;
    float b2 = v2 - v0;

    QVector3D e0 = vertex1.position - vertex0.position;
    QVector3D e1 = vertex2.position - vertex0.position;

    float k = t1 * b2 - b1 * t2;

    QVector3D tangent;
    tangent = k * QVector3D(b2 * e0.x() - b1 * e1.x(),b2 * e0.y() - b1 * e1.y(),b2 * e0.z() - b1 * e1.z());

    QVector<VertexData*> vertexArr = { &vertex0, &vertex1, &vertex2};
    QVector<int> adjoinPlane;
    adjoinPlane.resize(vertexArr.size());
    for(int i = 0;i < vertexArr.size();i++)
    {
        adjoinPlane[i]++;
        float ratio = 1.0f / adjoinPlane[i];
        vertexArr[i]->tangent = vertexArr[i]->tangent * (1 - ratio) + tangent * ratio;
        vertexArr[i]->tangent.normalize();
    }
}


void GeometryEngine::CalNormalAndTangent(VertexData& vertex0, VertexData& vertex1, VertexData& vertex2)
{
    float u0 = vertex0.texcoord.x();
    float v0 = vertex0.texcoord.y();

    float u1 = vertex1.texcoord.x();
    float v1 = vertex1.texcoord.y();

    float u2 = vertex2.texcoord.x();
    float v2 = vertex2.texcoord.y();

    float t1 = u1 - u0;
    float b1 = v1 - v0;

    float t2 = u2 - u0;
    float b2 = v2 - v0;

    QVector3D e0 = vertex1.position - vertex0.position;
    QVector3D e1 = vertex2.position - vertex0.position;

    float k = t1 * b2 - b1 * t2;

    QVector3D tangent;
    tangent = k * QVector3D(b2 * e0.x() - b1 * e1.x(),b2 * e0.y() - b1 * e1.y(),b2 * e0.z() - b1 * e1.z());

    QVector3D normal;
    normal = QVector3D::crossProduct(e0, e1);

    QVector<VertexData*> vertexArr = { &vertex0, &vertex1, &vertex2};
    QVector<int> adjoinPlane;
    adjoinPlane.resize(vertexArr.size());
    for(int i = 0;i < vertexArr.size();i++)
    {
        adjoinPlane[i]++;
        float ratio = 1.0f / adjoinPlane[i];
        vertexArr[i]->normal = vertexArr[i]->normal * (1 - ratio) + normal * ratio;
        vertexArr[i]->normal.normalize();
        vertexArr[i]->tangent = vertexArr[i]->tangent * (1 - ratio) + tangent * ratio;
        vertexArr[i]->tangent.normalize();
    }
}
