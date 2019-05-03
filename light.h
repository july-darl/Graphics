#ifndef LIGHT_H
#define LIGHT_H

#include <QVector3D>
#include <QColor>
#include <vector>
#include <QOpenGLFunctions>
using namespace std;

struct SSpotLight
{

    QVector3D  lightPos;
    QVector3D  lightColor;

    float Kq;
    int index;

    float GetPos(int i)
    {
        switch(i)
        {
        case 0:return lightPos.x();
        case 1:return lightPos.y();
        case 2:return lightPos.z();
        }
        return -1.0f;
    }


    void SetPos(int i, float data)
    {
        switch(i)
        {
        case 0:lightPos.setX(data);break;
        case 1:lightPos.setY(data);break;
        case 2:lightPos.setZ(data);break;
        }
    }
};

Q_DECLARE_METATYPE(SSpotLight)
class CLightInfo
{
private:
    enum { LightNum = 10 };

public:
    vector<SSpotLight> vecLight;
    CLightInfo();

};

#endif // LIGHT_H
