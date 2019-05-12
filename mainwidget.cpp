#include "MainWidget.h"
#include "openglwidget.h"
#include "lightwidget.h"
#include "outlinewidget.h"
#include "environmentwidget.h"
#include "baseobjectwidget.h"
#include "decalwidget.h"
#include "pbrwidget.h"
#include <QHBoxLayout>
//#include <qDebug>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(600,400);
    resize(1000,700);
    hlayout             = new QHBoxLayout();
    openGLWidget        = new COpenGLWidget();
    lightWidget         = new CLightWidget();
    outlineWidget       = new OutlineWidget();
    pbrWidget           = new CPBRWidget();
    environmentWidget   = new CEnvironmentWidget();
    baseWidget          = new CBaseObjectWidget();
    decalWidget         = new CDecalWidget();

    hlayout->addWidget(outlineWidget,1);
    hlayout->addWidget(openGLWidget, 5);
    hlayout->addWidget(environmentWidget, 1);

    connect(openGLWidget,SIGNAL(OnCreate()),lightWidget,SLOT(InitParam()));
    connect(openGLWidget,SIGNAL(OnCreate()),outlineWidget,SLOT(InitParam()));

    connect(outlineWidget,SIGNAL(ActiveWindow(EWindowName,void*)),this,SLOT(SetActiveWindow(EWindowName,void*)));

    activeWidget = environmentWidget;
    setLayout(hlayout);
}

QWidget* MainWidget::GetWidget(EWindowName widget)
{
    switch(widget)
    {
    case LIGHT_WIDGET:
        return lightWidget;
    case PBR_WIDGET:
       return pbrWidget;
    case ENVIRONMENT_WIDGET:
        return environmentWidget;
    case BASE_WIDGET:
        return baseWidget;
    case DECAL_WIDGET:
        return decalWidget;
    }
    return nullptr;
}

void MainWidget::SetActiveWindow(EWindowName widget, void* param/* = nullptr*/)
{
    QWidget* selectedWidget = GetWidget(widget);
    if(activeWidget != selectedWidget)
    {
        activeWidget->setParent(nullptr);
        hlayout->removeWidget(activeWidget);
        hlayout->addWidget(selectedWidget,1);
        activeWidget = selectedWidget;
    }

    switch(widget)
    {
    case LIGHT_WIDGET:
    {
        CLightWidget* pWidget = static_cast<CLightWidget*>(selectedWidget);
        int* index = static_cast<int*>(param);
        pWidget->OnSelectLight(*index);
        break;
    }
    case PBR_WIDGET:
    {
        CPBRWidget* pWidget = static_cast<CPBRWidget*>(selectedWidget);
        Object* object = static_cast<Object*>(param);
        pWidget->OnSelectedObject(object);
        break;
    }
    case ENVIRONMENT_WIDGET:
    {
        CEnvironmentWidget* pWidget = static_cast<CEnvironmentWidget*>(selectedWidget);
        pWidget->OnSelected();
        break;
    }
    case BASE_WIDGET:
    {
        CBaseObjectWidget* pWidget = static_cast<CBaseObjectWidget*>(selectedWidget);
        Object* object = static_cast<Object*>(param);
        pWidget->OnSelectedObject(object);
        break;
    }
    case DECAL_WIDGET:
    {
        CDecalWidget* pWidget = static_cast<CDecalWidget*>(selectedWidget);
        Object* object = static_cast<Object*>(param);
        pWidget->OnSelectedObject(object);
        break;
    }
    }
}
