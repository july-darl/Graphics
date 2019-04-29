#include "light.h"

CLightInfo::CLightInfo()
{

    vecLight.resize(LightNum);
    for(size_t i = 0;i < LightNum; i++)
    {
        float y = rand() % 10;
        float x = rand() % 20 - 10;
        float z = rand() % 20 - 10;

        if(y < 3) y = 3;
        vecLight[i].lightPos.setY(y);
        vecLight[i].lightPos.setX(x);
        vecLight[i].lightPos.setZ(z);

        float r = 1;//(float)(rand() % 100) / 100;
        float g = 1;//(float)(rand() % 100) / 100;
        float b = 1;//(float)(rand() % 100) / 100;

        vecLight[i].lightColor.setX(r);
        vecLight[i].lightColor.setY(g);
        vecLight[i].lightColor.setZ(b);

        vecLight[i].Kq = 0.01;

        vecLight[i].index = i;
    }
}
