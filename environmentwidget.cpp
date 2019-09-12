#include "environmentwidget.h"


CEnvironmentWidget::CEnvironmentWidget(QWidget* parent)
    :PropertyWidget (parent)
{
    hdrInit("开启HDR", nullptr);
    gammaInit("开启gamma矫正", nullptr);
    exposureInit("exposure",0,1,nullptr);
   // backgroundInit("HDR",[&](QString fileName)
   // {
   //     RenderCommon::Inst()->CreateHDRCubemap(fileName);
   // });
    vlayout->addStretch( static_cast<int>((height() * 0.8)));
}

void CEnvironmentWidget::OnSelected()
{
    hdrSetData(RenderCommon::Inst()->GetHDR());
    gammaSetData(RenderCommon::Inst()->GetGamma());
    exposureSetData(RenderCommon::Inst()->GetExposure());
}
