#ifndef PBRWIDGET_H
#define PBRWIDGET_H

#include "rendercommon.h"
#include "baseobjectwidget.h"

class CPBRWidget : public CBaseObjectWidget
{
    Q_OBJECT
public:
    CPBRWidget();
private:

    DECLARE_SLIDER(rough)
    DECLARE_SLIDER(ao)
    DECLARE_SLIDER(metal)
    DECLARE_COLOR(albedo)
    DECLARE_IMAGE(albedoTex)
    DECLARE_IMAGE(normalTex)
    DECLARE_IMAGE(pbrTex)
    DECLARE_CHECKBOX(bBloom)
    DECLARE_CHECKBOX(bSSR)
    DECLARE_CHECKBOX(bFire)
    DECLARE_CHECKBOX(bSSS)
public:
    void OnSelectedObject(class Object* obj);
};


#endif // PBRWIDGET_H
