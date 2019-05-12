#include "decalwidget.h"
#include "object.h"
#include "decal.h"

CDecalWidget::CDecalWidget()
{
    SetLabel("Attribute");
    decalTexInit("贴花");
    decalSizeInit("大小",0.0f,4.0f);

    vlayout->addStretch(static_cast<int>((height() * 0.8)));
}

void CDecalWidget::OnSelectedObject(Object* obj)
{
    qDebug() << "OnSelectedObject";
    CBaseObjectWidget::OnSelectedObject(obj);
    Decal* p = static_cast<Decal*>(obj);

    decalTexSetData(&p->decalTex, &p->decalImg);
    decalSizeSetData(&p->decalSize);
}
