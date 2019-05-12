#ifndef DECALWIDGET_H
#define DECALWIDGET_H

#include "rendercommon.h"
#include "baseobjectwidget.h"

class Object;
class CDecalWidget : public CBaseObjectWidget
{
    Q_OBJECT
public:
    CDecalWidget();
private:
    DECLARE_IMAGE(decalTex)
    DECLARE_SLIDER(decalSize)
public:
    void OnSelectedObject(Object* obj);
};


#endif // DECALWIDGET_H
