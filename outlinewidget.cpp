#include "outlinewidget.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHBoxLayout>
#include "rendercommon.h"
#include "cameramanager.h"
#include "object.h"

#include <qDebug>
OutlineWidget::OutlineWidget(QWidget* parent)
    :QWidget(parent)
{
    QHBoxLayout* hlayout = new QHBoxLayout();

    treeWidget = new QTreeWidget();
    treeWidget->setHeaderLabel("大纲视图");
    QString name[] =
    {
        "灯光","场景对象","贴花","环境","噪声"
    };

    for(int i = 0;i < SIZE;i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(treeWidget);
        item->setText(0,name[i]);
    }
    hlayout->addWidget(treeWidget);
    setLayout(hlayout);

    connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)), this,SLOT(OnClickItem(QTreeWidgetItem* ,int)));
}

void OutlineWidget::OnClickItem(QTreeWidgetItem* item ,int)
{
    QVariant var = item->data(0,1);

    if(item->text(0) == "环境")
    {
        emit(SelectEnvironment());
        emit(ActiveWindow(ENVIRONMENT_WIDGET,nullptr));
    }
    else if(var.canConvert<SSpotLight>())
    {
        const SSpotLight& light = var.value<SSpotLight>();
        Camera::Inst()->SetDest(light.lightPos);
        emit(SelectLight(light.index));
        emit(ActiveWindow(LIGHT_WIDGET,(void*)&light.index));
    }
    else if(var.canConvert<void*>())
    {
        void* data = var.value<void*>();
        Object* obj = static_cast<Object*>(data);

        if(!obj)
        {
            return;
        }

        if(obj->type == "Phong")
        {
            emit(ActiveWindow(PBR_WIDGET,data));
        }
        else if(obj->type == "VolumeCloud")
        {
            emit(ActiveWindow(VOLUMECLOUD_WIDGET,data));
        }
        else
        {
            emit(ActiveWindow(BASE_WIDGET,data));
        }
    }
}

void OutlineWidget::InitParam()
{
    QTreeWidgetItemIterator it(treeWidget);

    while (*it)
    {
        auto item = *it;
        if(item->text(0) == "灯光")
        {
            vector<SSpotLight>& spotLight = RenderCommon::Inst()->GetLightInfo()->vecLight;
            for(size_t i = 0;i < spotLight.size();i++)
            {
                QTreeWidgetItem* childItem = new QTreeWidgetItem(item);
                QVariant var;
                var.setValue(spotLight[i]);
                childItem->setData(0,1,var);
                childItem->setText(0,"灯光" + QString::number(i));
            }
        }
        else if(item->text(0) == "场景对象")
        {
            size_t count = static_cast<size_t>(ObjectInfo::Inst()->GetObjectCount());
            for(size_t i = 0;i < count;i++)
            {
                Object* obj = ObjectInfo::Inst()->GetObject(i);
                QTreeWidgetItem* childItem = new QTreeWidgetItem(item);
                QVariant var;
                var.setValue(static_cast<void*>(obj));
                childItem->setData(0,1,var);
                childItem->setText(0,QString::fromStdString(obj->GetName()));
            }
        }
        ++it;
    }
}
