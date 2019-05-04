#include "openglwidget.h"
#include "cameramanager.h"
#include "rendercommon.h"
#include "object.h"
#include <QMouseEvent>

COpenGLWidget::COpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
}

COpenGLWidget::~COpenGLWidget()
{

}

void COpenGLWidget::keyPressEvent(QKeyEvent* event)
{
    if(!event->isAutoRepeat())
    {
        if(event->key() == Qt::Key_W)
        {
            m_nDir |= KEY_FRONT;
        }
        else if(event->key() == Qt::Key_S)
        {
            m_nDir |= KEY_BACK;
        }
        else if(event->key() == Qt::Key_A)
        {
            m_nDir |= KEY_LEFT;
        }
        else if(event->key() == Qt::Key_D)
        {
            m_nDir |= KEY_RIGHT;
        }
        else if(event->key() == Qt::Key_Q)
        {
            m_nDir |= KEY_UP;
        }
        else if(event->key() == Qt::Key_E)
        {
            m_nDir |= KEY_DOWN;
        }
        else if(event->key() == Qt::Key_F)
        {
           RenderCommon::Inst()->UpdateEnvironment();
        }
        else if(event->key() == Qt::Key_G)
        {
           RenderCommon::Inst()->SetSnow(!RenderCommon::Inst()->IsSnow());
        }
    }
}

void COpenGLWidget::keyReleaseEvent(QKeyEvent* event)
{
    if(!event->isAutoRepeat())
    {
        if(event->key() == Qt::Key_W)
        {
            m_nDir = ~(m_nDir ^ ~KEY_FRONT);
        }
        else if(event->key() == Qt::Key_S)
        {
            m_nDir = ~(m_nDir ^ ~KEY_BACK);
        }
        else if(event->key() == Qt::Key_A)
        {
            m_nDir = ~(m_nDir ^ ~KEY_LEFT);
        }
        else if(event->key() == Qt::Key_D)
        {
            m_nDir = ~(m_nDir ^ ~KEY_RIGHT);
        }
        else if(event->key() == Qt::Key_Q)
        {
            m_nDir = ~(m_nDir ^ ~KEY_UP);
        }
        else if(event->key() == Qt::Key_E)
        {
            m_nDir = ~(m_nDir ^ ~KEY_DOWN);
        }
    }
}

void COpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_DEPTH_TEST);

    ObjectInfo::Inst()->Create();

    m_timer.start(20, this);

    emit(OnCreate());
}


void COpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    m_bMouseDown = true;

    m_fMouseX = event->x();
    m_fMouseY = event->y();

    update();
}

void COpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons() & Qt::MidButton && m_bMouseDown)
    {
        float x = event->x();
        float y = event->y();

        float ax = (x - m_fMouseX) / 220;
        m_fMouseX = x;

        float ay = (y - m_fMouseY) / 80;
        m_fMouseY = y;

        Camera::Inst()->MoveUp(ay);
        Camera::Inst()->MoveLeft(ax);
        update();
    }
    if(event->buttons() & Qt::RightButton && m_bMouseDown)
    {
        float x = event->x();
        float y = event->y();

        m_fAccY += (x - m_fMouseX) / 5.0f;
        m_fMouseX = x;

        m_fAccX += (y - m_fMouseY) / 5.0f;
        m_fMouseY = y;

        m_fAccX = clamp<float>(m_fAccX,-70.0f, 70.0f);

        Camera::Inst()->SetRotateXY(m_fAccX, m_fAccY);

        update();
    }
}

void COpenGLWidget::mouseReleaseEvent(QMouseEvent*)
{
    m_bMouseDown = false;
}

void COpenGLWidget::wheelEvent(QWheelEvent* event)
{
    float numDegrees = event->delta() / 50;
    Camera::Inst()->Zoom(numDegrees);
    update();
}

void COpenGLWidget::timerEvent(QTimerEvent *)
{
    const float step = 0.1f;
    if(m_nDir & KEY_FRONT)
    {
        Camera::Inst()->MoveFront(step);
    }
    if(m_nDir & KEY_BACK)
    {
        Camera::Inst()->MoveBack(step);
    }
    if(m_nDir & KEY_UP)
    {
        Camera::Inst()->MoveUp(step);
    }
    if(m_nDir & KEY_DOWN)
    {
        Camera::Inst()->MoveDown(step);
    }
    if(m_nDir & KEY_LEFT)
    {
        Camera::Inst()->MoveLeft(step);
    }
    if(m_nDir & KEY_RIGHT)
    {
        Camera::Inst()->MoveRight(step);
    }
    update();
}

void COpenGLWidget::resizeGL(int w, int h)
{
    RenderCommon::Inst()->UpdateScreenXY(w, h);
}
#include "resourceinfo.h"
void COpenGLWidget::paintGL()
{
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // auto p = CResourceInfo::Inst()->CreateTessProgram("test.tcsh","test.tesh","test.vsh","test.fsh","test");
   // p->bind();
   //
   // RenderCommon::Inst()->GetGeometryEngine()->drawSphere(p,true);
    ObjectInfo::Inst()->Render();
}
