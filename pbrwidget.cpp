#include "pbrwidget.h"
#include "object.h"
#include "phong.h"

CPBRWidget::CPBRWidget(QWidget* parent)
    : CBaseObjectWidget(parent)
{
    SetLabel("Attribute");

    roughInit("rough",0.0f,1.0f);
    aoInit("ao",0.0f,1.0f);
    metalInit("metal",0.0f,1.0f);
    albedoInit("color");

    albedoTexInit("albedo");
    normalTexInit("法线");
    pbrTexInit("遮罩");
    bBloomInit("开启bloom");
    bSSRInit("开启SSR");
    bFireInit("fire");
    bXRayInit("X射线");
    bOutlineInit("描边");

    vector<QString> name { "背景","默认","透明测试","透明混合","叠加效果"};
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
// Cs + Cd
// Cs * (1 - Cd) + Cd
// Cs * As + Cd * (1 - As)
// Cs * Cs + Cd * (1 - Cs)
void CPBRWidget::OnSelectedObject(Object* obj)
{
    qDebug() << "OnSelectedObject";
    CBaseObjectWidget::OnSelectedObject(obj);
    Phong* p = static_cast<Phong*>(obj);

    roughSetData(&p->rough);
    aoSetData(&p->ao);
    metalSetData(&p->metal);
    albedoSetData(&p->color);

    albedoTexSetData(&p->albedo, &p->albedoImg);
    normalTexSetData(&p->normal, &p->normalImg);
    pbrTexSetData(&p->maskTex, &p->maskImg);

    bBloomSetData(&p->bBloom);
    bSSRSetData(&p->bSSR);
    bFireSetData(&p->bFire);
    bXRaySetData(&p->bXRay);
    bOutlineSetData(&p->bOutline);
    renderQueueSetData(&p->renderPriority);
}

void CPBRWidget::OnChangeRenderQueue(int index)
{
    Object* obj = ObjectInfo::Inst()->GetActiveObject();
    ObjectInfo::Inst()->SetRenderQueue(obj, index * 1000);

    blendModeSetVisible(index == RQ_Transparent);

}
