#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QBasicTimer>

class GeometryEngine;

class COpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    enum EKEY_Direction
    {
        KEY_NONE    = 0,
        KEY_LEFT    = 1,
        KEY_RIGHT   = 2,
        KEY_FRONT   = 4,
        KEY_BACK    = 8,
        KEY_UP      = 16,
        KEY_DOWN    = 32,
    };

    explicit COpenGLWidget(QWidget *parent = nullptr);
    ~COpenGLWidget() override;
protected:
    void            keyPressEvent(QKeyEvent* event) override;
    void            keyReleaseEvent(QKeyEvent* event) override;
    void            mousePressEvent(QMouseEvent* event) override;
    void            mouseMoveEvent(QMouseEvent* event) override;
    void            mouseReleaseEvent(QMouseEvent* event) override;
    void            wheelEvent(QWheelEvent* event) override;
    void            initializeGL() override;
    void            resizeGL(int w, int h) override;
    void            paintGL() override;
    void            timerEvent(QTimerEvent *) override;

private:
    QBasicTimer     m_timer;
    int             m_nDir = KEY_NONE;
    bool            m_bMouseDown = false;

    float           m_fMouseX = 0.0f;
    float           m_fMouseY = 0.0f;
    float           m_fAccX = 0.0f;
    float           m_fAccY = 0.0f;

signals:
    void            OnCreate();
};

#endif // OPENGLWIDGET_H
