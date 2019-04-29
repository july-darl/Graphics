#include "glass.h"
#include "rendercommon.h"
#include "cameramanager.h"
#include <iostream>

void Glass::Create()
{
    initializeOpenGLFunctions();

    QOpenGLShader* fShader = new QOpenGLShader(QOpenGLShader::Fragment);
    fShader->compileSourceFile(":/fShader.glsl");

    m_program.addShader(RenderCommon::Inst()->GetCommonTexVShader());
    m_program.addShader(fShader);
    m_program.link();


    glGenTextures(1, &m_nTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_nTextureId);

    glEnable(GL_TEXTURE_CUBE_MAP);
    QString path[] =
    {
        ":/right.jpg",":/left.jpg",":/top.jpg",":/bottom.jpg",
        ":/back.jpg",":/front.jpg",
    };
    for(int i = 0;i < 6;i++)
    {
        QImage image(path[i]);
        const int count = image.byteCount() / 4 * 3;
        unsigned char* data = new unsigned char[count];
        int cnt = 0;
        for(int j = 0;j < count / 3; j++)
        {
            data[3 * j] = image.bits()[cnt + 2];
            data[3 * j + 1] = image.bits()[cnt + 1];
            data[3 * j + 2] = image.bits()[cnt];
            cnt += 4;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_RGB,
                     image.width(),
                     image.height(),
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     data);
        delete[] data;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Glass::Render()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    QMatrix4x4 sphereModelMatrix;
//    sphereModelMatrix.scale(2.0f, 2.0f, 2.0f);

    QMatrix4x4 IT_ModelMatrix = sphereModelMatrix;
    IT_ModelMatrix = IT_ModelMatrix.transposed();
    IT_ModelMatrix = IT_ModelMatrix.inverted();
    m_program.bind();

    m_program.setUniformValue("ModelMatrix",sphereModelMatrix);
    m_program.setUniformValue("IT_ModelMatrix",IT_ModelMatrix);
    m_program.setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    m_program.setUniformValue("ViewMatrix", Camera::Inst()->GetViewMatrix());
    m_program.setUniformValue("cameraPos",Camera::Inst()->GetCameraPos());
    m_program.setUniformValue("type", m_nType);
    m_program.setUniformValue("lightPos", RenderCommon::Inst()->GetLightLocation());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_nTextureId);
    m_program.setUniformValue("cubemap",0);


    RenderCommon::Inst()->GetGeometryEngine()->drawSphere(&m_program);
}
