#include "LightWidget.h"
#include "rendercommon.h"
#include "mainwidget.h"
#include "object.h"
#include "icon.h"
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSignalMapper>
#include <QComboBox>
#include <QCheckBox>
CLightWidget::CLightWidget()
{
    QVBoxLayout* vlayout = new QVBoxLayout();
    setWindowTitle("灯光");

    QLabel* label1 = new QLabel("光源衰减参数");
    curLightLabel = new QLabel("当前光源:");

    vlayout->addWidget(curLightLabel);
    vlayout->addWidget(label1);

    QString name2[] =
    {
        "x","y","z",
    };

    {
        QHBoxLayout* hlayout = new QHBoxLayout();
        QLabel* label = new QLabel("Kq");
        KqSlider = new QSlider();
        KqSlider->setOrientation(Qt::Horizontal);
        KqLabel = new QLabel();
        hlayout->addWidget(label,1);
        hlayout->addWidget(KqSlider,2);
        hlayout->addWidget(KqLabel,1);
        vlayout->addLayout(hlayout);

        connect(KqSlider,SIGNAL(valueChanged(int)),this,SLOT(OnKqScroll(int)));
    }

    QLabel* label2 = new QLabel("光源位置");
    vlayout->addWidget(label2);

    for(int i = 0;i < 3;i++)
    {
        QHBoxLayout* hlayout1 = new QHBoxLayout();
        QLabel* label = new QLabel(name2[i]);
        m_slider2[i] = new QSlider();
        m_slider2[i]->setOrientation(Qt::Horizontal);
        m_label2[i] = new QLabel();
        hlayout1->addWidget(label,1);
        hlayout1->addWidget(m_slider2[i],2);
        hlayout1->addWidget(m_label2[i],1);
        vlayout->addLayout(hlayout1);

        connect(m_slider2[i],SIGNAL(valueChanged(int)),this,SLOT(OnScroll2(int)));
    }

    {
        QHBoxLayout* hlayout1 = new QHBoxLayout();
        QLabel* label = new QLabel("曝光");
        hdrSlider = new QSlider();
        hdrSlider->setOrientation(Qt::Horizontal);
        hdrLabel = new QLabel();
        hlayout1->addWidget(label,1);
        hlayout1->addWidget(hdrSlider,2);
        hlayout1->addWidget(hdrLabel,1);
        vlayout->addLayout(hlayout1);

        connect(hdrSlider,SIGNAL(valueChanged(int)),this,SLOT(OnHDRScroll(int)));
    }

    vlayout->addSpacing( (int)(height() * 0.8));

    setLayout(vlayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);

}

void CLightWidget::OnKqScroll(int value)
{
    float data = (float)value / 1000;
    if(fIsZero(data,0.001f))
    {
        data = 0.001f;
    }
    KqLabel->setText(QString::number((double)data));
    RenderCommon::Inst()->GetLightInfo()->vecLight[m_activeId].Kq = data;
}

void CLightWidget::OnScroll2(int value)
{
    QSlider* slider = qobject_cast <QSlider*>(sender());
    for(int i = 0;i < 3;i++)
    {
        if(slider == m_slider2[i])
        {
            float data;
            if(i == 1)
            {
                data = (float)value / 10;
            }
            else
            {
                data = (float)value / 5 - 10;
            }

            m_label2[i]->setText(QString::number((double)data));
            RenderCommon::Inst()->GetLightInfo()->vecLight[m_activeId].SetPos(i,data);
            break;
        }
    }

    if(icon)
    {
        SSpotLight& spotLight = RenderCommon::Inst()->GetLightInfo()->vecLight[m_activeId];
        icon->position = Vector3f(spotLight.lightPos.x(),spotLight.lightPos.y(),spotLight.lightPos.z());
    }
}

void CLightWidget::InitParam()
{
    SSpotLight& spotLight = RenderCommon::Inst()->GetLightInfo()->vecLight[m_activeId];

    float num = spotLight.Kq;
    KqSlider->setValue(num * 1000);
    KqLabel->setText(QString::number((double)num));

    for(int i = 0;i < 3;i++)
    {
        float num =spotLight.GetPos(i);
        if(i == 1)
        {
            m_slider2[i]->setValue(num * 10);
        }
        else
        {
            m_slider2[i]->setValue((num + 10) * 5);
        }
        m_label2[i]->setText(QString::number((double)num));
    }
    if(!icon)
    {
       // icon = static_cast<Icon*>(ObjectInfo::Inst()->CreateObject("Icon"));
        if(icon)
        {

            icon->position = Vector3f(spotLight.lightPos.x(),spotLight.lightPos.y(),spotLight.lightPos.z());
        }
    }

    curLightLabel->setText("当前光源:灯光" + QString::number(spotLight.index));

    hdrSlider->setValue(RenderCommon::Inst()->GetHDRParam() * 20);
    hdrLabel->setText(QString::number(RenderCommon::Inst()->GetHDRParam()));
}

void CLightWidget::OnHDRScroll(int value)
{
    float param = 1.0f * value / 20.0f;
    RenderCommon::Inst()->SetHDRParam(param);
    hdrLabel->setText(QString::number(RenderCommon::Inst()->GetHDRParam()));
}

void CLightWidget::OnSelectLight(int index)
{
    m_activeId = index;
    InitParam();
}
