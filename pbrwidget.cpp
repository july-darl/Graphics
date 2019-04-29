#include "pbrwidget.h"
#include "object.h"
#include "phong.h"

CPBRWidget::CPBRWidget()
{
    SetLabel("Attribute");

    roughInit("rough",0.0f,1.0f);
    aoInit("ao",0.0f,1.0f);
    metalInit("metal",0.0f,1.0f);
    albedoInit("color");

    albedoTexInit("albedo");
    normalTexInit("法线");
    pbrTexInit("PBR");

    vlayout->addStretch(static_cast<int>((height() * 0.8)));
}

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
    pbrTexSetData(&p->pbrTex, &p->pbrImg);
}
