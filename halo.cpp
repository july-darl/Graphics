#include "halo.h"
#include <QOpenGLTexture>
#include "icon.h"
#include "object.h"
#include "cameramanager.h"
void Halo::Create()
{

    for(int i = 0;i < 10;i++)
    {
        halo[i] = static_cast<Icon*>(ObjectInfo::Inst()->CreateObject("Icon", QVector3D(0,4,-20)));
        halo[i]->AddShader("icon.vsh", "flare.fsh");
        halo[i]->SetIcon("./halo.png");
        int r = rand() % 5;
        halo[i]->SetScale(0.2f * r);
        halo[i]->bRender = false;
    }
    bCreate = true;
}

void Halo::UpdateLocation()
{
    if(!bCreate)return;
    Icon* sun = ObjectInfo::Inst()->sun;
    if(!sun) return;
    if(sun->IsOnScreen())
    {

        QVector3D centerPos = Camera::Inst()->GetCenterPos();
        QVector3D sunPos = sun->position;
        for(int i = 0;i < 10;i++)
        {
            float ratio = (float)(i + 1) / ( 10 + 1);
            halo[i]->bRender = true;
            halo[i]->position = centerPos * ratio + sunPos * (1 - ratio);
        }
    }
    else
    {
        for(int i = 0;i < 10;i++)
        {
            halo[i]->bRender = false;
        }
    }
}
