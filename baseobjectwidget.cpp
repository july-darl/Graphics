#include "object.h"
#include "baseobjectwidget.h"

CBaseObjectWidget::CBaseObjectWidget(QWidget* parent)
    : PropertyWidget(parent)
{
    bShadowInit("产生阴影");
    bShowInit("show");

    SetLabel("位置");

    posXInit("x",-10.0f,10.0f);
    posYInit("y",-10.0f,10.0f);
    posZInit("z",-10.0f,10.0f);

    SetLabel("旋转");

    rotXInit("x",0.0f,360.0f);
    rotYInit("y",0.0f,360.0f);
    rotZInit("z",0.0f,360.0f);

    SetLabel("缩放");

    scaleXInit("x",0.0f,10.0f);
    scaleYInit("y",0.0f,10.0f);
    scaleZInit("z",0.0f,10.0f);

    vector<QString> name { "背景","默认","透明测试","透明混合"};
    renderQueueInit("渲染队列", name, [&](int index){
        OnChangeRenderQueue(index);
    });

    vector<QString> blendName { "Cs + Cd","Cs * (1 - Cd) + Cd","Cs * As + Cd * (1 - As)","Cs * Cs + Cd * (1 - Cs)"};
    blendModeInit("透明混合公式", blendName, [&](int index){
        OnChangeRenderQueue(index);
    });
    blendModeSetVisible(false);

    vlayout->addStretch(static_cast<int>((height() * 0.8)));
}

void CBaseObjectWidget::OnSelectedObject(Object* p)
{
    ObjectInfo::Inst()->SetActiveObject(p);
    bShadowSetData(&p->bCastShadow);
    bShowSetData(&p->bRender);

    posXSetData(&p->position.x);
    posYSetData(&p->position.y);
    posZSetData(&p->position.z);

    rotXSetData(&p->rotation.x);
    rotYSetData(&p->rotation.y);
    rotZSetData(&p->rotation.z);

    scaleXSetData(&p->scale.x);
    scaleYSetData(&p->scale.y);
    scaleZSetData(&p->scale.z);

    renderQueueSetData(&p->renderPriority);
}

void CBaseObjectWidget::OnChangeRenderQueue(int index)
{
    Object* obj = ObjectInfo::Inst()->GetActiveObject();
    ObjectInfo::Inst()->SetRenderQueue(obj, index);

    blendModeSetVisible(index == RQ_Transparent);
}
