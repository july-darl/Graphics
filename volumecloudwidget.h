#ifndef VOLUMECLOUDWIDGET_H
#define VOLUMECLOUDWIDGET_H

#include "baseobjectwidget.h"
class Object;
class CVolumeCloudWidget : public CBaseObjectWidget
{
public:
    CVolumeCloudWidget();
private:

    DECLARE_SLIDER(thickness)
    DECLARE_SLIDER(mixRatio)
    DECLARE_SLIDER(brightness)
public:
    void OnSelectedObject(Object* obj);
};

#endif // VOLUMECLOUDWIDGET_H
