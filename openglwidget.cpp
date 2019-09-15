#include "openglwidget.h"
#include "cameramanager.h"
#include "rendercommon.h"
#include "object.h"
#include <QMouseEvent>
#include "imgui.h"
#include "resourceinfo.h"
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

void COpenGLWidget::ImGui_NewFrame()
{
    auto program = CResourceInfo::Inst()->CreateProgram("imgui.vsh", "imgui.fsh", "imgui");
}

void COpenGLWidget::ImGui_Init()
{
   // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_qt";

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab] = Qt::Key_Tab;
    io.KeyMap[ImGuiKey_LeftArrow] = Qt::Key_Left;
    io.KeyMap[ImGuiKey_RightArrow] = Qt::Key_Right;
    io.KeyMap[ImGuiKey_UpArrow] = Qt::Key_Up;
    io.KeyMap[ImGuiKey_DownArrow] = Qt::Key_Down;
    io.KeyMap[ImGuiKey_PageUp] = Qt::Key_PageUp;
    io.KeyMap[ImGuiKey_PageDown] = Qt::Key_Down;
    io.KeyMap[ImGuiKey_Home] = Qt::Key_Home;
    io.KeyMap[ImGuiKey_End] = Qt::Key_End;
    io.KeyMap[ImGuiKey_Insert] = Qt::Key_Insert;
    io.KeyMap[ImGuiKey_Delete] = Qt::Key_Delete;
    io.KeyMap[ImGuiKey_Backspace] = Qt::Key_Backspace;
    io.KeyMap[ImGuiKey_Space] = Qt::Key_Space;
    io.KeyMap[ImGuiKey_Enter] = Qt::Key_Enter;
    io.KeyMap[ImGuiKey_Escape] =  Qt::Key_Escape;
   // io.KeyMap[ImGuiKey_KeyPadEnter] =Qt::Key_KeyPad
    io.KeyMap[ImGuiKey_A] = Qt::Key_A;
    io.KeyMap[ImGuiKey_C] = Qt::Key_C;
    io.KeyMap[ImGuiKey_V] = Qt::Key_V;
    io.KeyMap[ImGuiKey_X] = Qt::Key_X;
    io.KeyMap[ImGuiKey_Y] = Qt::Key_Y;
    io.KeyMap[ImGuiKey_Z] = Qt::Key_Z;

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
    //CResourceInfo::Inst()->CreateTexture(pixels, width, height);

    // Store our identifier
   // io.Fonts->TexID = (ImTextureID)(intptr_t)m_frontTex;
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

void COpenGLWidget::paintGL()
{
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_NewFrame();
    //ImGui::NewFrame();
    //ImGui::Text("This is some useful text.");

    ObjectInfo::Inst()->Render();
   // ImGui::Render();
}
