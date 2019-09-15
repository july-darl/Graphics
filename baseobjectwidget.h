#ifndef BASEOBJECTWIDGET_H
#define BASEOBJECTWIDGET_H

#include "propertywidget.h"
class Object;

class CBaseObjectWidget : public PropertyWidget
{
    Q_OBJECT
public:
    CBaseObjectWidget(QWidget* parent = nullptr);
private:

    DECLARE_CHECKBOX(bShadow)
    DECLARE_CHECKBOX(bShow)
    DECLARE_SLIDER(posX)
    DECLARE_SLIDER(posY)
    DECLARE_SLIDER(posZ)
    DECLARE_SLIDER(rotX)
    DECLARE_SLIDER(rotY)
    DECLARE_SLIDER(rotZ)
    DECLARE_SLIDER(scaleX)
    DECLARE_SLIDER(scaleY)
    DECLARE_SLIDER(scaleZ)
    DECLARE_COMBO(renderQueue)
    DECLARE_COMBO(blendMode)

    void OnChangeRenderQueue(int index);
public slots:
    void OnSelectedObject(Object* obj);

};

#endif // BASEOBJECTWIDGET_H
