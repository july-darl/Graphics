#include "volumecloudwidget.h"
#include "object.h"
#include "phong.h"
#include "volumecloud.h"

CVolumeCloudWidget::CVolumeCloudWidget()
{
    SetLabel("Attribute");

    thicknessInit("thickness",0.0f,1.0f);
    mixRatioInit("mixRatio",0.0f,1.0f);
    brightnessInit("brightness", 0.0f,1.0f);

    vlayout->addStretch(static_cast<int>((height() * 0.8)));
}

void CVolumeCloudWidget::OnSelectedObject(Object* obj)
{
    CBaseObjectWidget::OnSelectedObject(obj);
    VolumeCloud* p = static_cast<VolumeCloud*>(obj);

    thicknessSetData(&p->thickness);
    mixRatioSetData(&p->mixRatio);
    brightnessSetData(&p->brightness);
}
