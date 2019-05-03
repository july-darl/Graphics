#ifndef GLASS_H
#define GLASS_H

#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

class Glass : protected QOpenGLFunctions
{
private:
    int                     m_nType = 0;
    unsigned int            m_nTextureId;
    QOpenGLShaderProgram    m_program;
public:
    int GetType() { return m_nType; }
    void SetType(int type) { m_nType = type;}
    void Create();
    void Render();
};

#endif // GLASS_H
