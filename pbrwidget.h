#ifndef PBRWIDGET_H
#define PBRWIDGET_H

#include "rendercommon.h"
#include "baseobjectwidget.h"

class CPBRWidget : public CBaseObjectWidget
{
    Q_OBJECT
public:
    CPBRWidget(QWidget* parent = nullptr);
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
    DECLARE_CHECKBOX(bXRay)
    DECLARE_COMBO(renderQueue)
public:
    void OnSelectedObject(class Object* obj);
    void OnChangeRenderQueue(int index);
};

// Cs + Cd
// Cs * (1 - Cd) + Cd
// Cs * As + Cd * (1 - As)
// Cs * Cs + Cd * (1 - Cs)
//

#endif // PBRWIDGET_H
