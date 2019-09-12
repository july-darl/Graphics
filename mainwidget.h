#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>


enum EWindowName
{
    LIGHT_WIDGET,
    PBR_WIDGET,
    ENVIRONMENT_WIDGET,
    BASE_WIDGET,
    DECAL_WIDGET,
    WIDGET_NUM
};

class COpenGLWidget;
class CLightWidget;
class OutlineWidget;
class QHBoxLayout;
class CEnvironmentWidget;
class CBaseObjectWidget;
class CPBRWidget;
class CDecalWidget;
class QScrollArea;
class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);

    QWidget*            GetWidget(EWindowName widget);
private:
    QHBoxLayout*        hlayout = nullptr;
    QScrollArea*        scrollArea[WIDGET_NUM];
    CDecalWidget*       decalWidget = nullptr;
    COpenGLWidget*      openGLWidget = nullptr;
    CLightWidget*       lightWidget = nullptr;
    OutlineWidget*      outlineWidget = nullptr;
    CPBRWidget*         pbrWidget = nullptr;
    CEnvironmentWidget* environmentWidget = nullptr;
    CBaseObjectWidget*  baseWidget = nullptr;
    EWindowName         activeWindow;
public slots:
    void                SetActiveWindow(EWindowName widget, void* param = nullptr);
};

#endif // MAINWIDGET_H
