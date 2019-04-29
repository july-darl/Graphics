#include "object.h"
#include "baseobjectwidget.h"

CBaseObjectWidget::CBaseObjectWidget()
{
    bShadowInit("产生阴影");

    SetLabel("位置");

    posXInit("x",-10.0f,20.0f);
    posYInit("y",-10.0f,20.0f);
    posZInit("z",-10.0f,20.0f);

    SetLabel("旋转");

    rotXInit("x",0.0f,360.0f);
    rotYInit("y",0.0f,360.0f);
    rotZInit("z",0.0f,360.0f);

    SetLabel("缩放");

    scaleXInit("x",0.0f,10.0f);
    scaleYInit("y",0.0f,10.0f);
    scaleZInit("z",0.0f,10.0f);

    vlayout->addStretch(static_cast<int>((height() * 0.8)));
}

void CBaseObjectWidget::OnSelectedObject(Object* p)
{
    bShadowSetData(&p->bCastShadow);

    posXSetData(&p->position.x);
    posYSetData(&p->position.y);
    posZSetData(&p->position.z);

    rotXSetData(&p->rotation.x);
    rotYSetData(&p->rotation.y);
    rotZSetData(&p->rotation.z);

    scaleXSetData(&p->scale.x);
    scaleYSetData(&p->scale.y);
    scaleZSetData(&p->scale.z);
}
