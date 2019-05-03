#ifndef ICON_H
#define ICON_H

#include "object.h"

enum EBillboardType
{
    BILL_World,
    BILL_YAxis,
    BILL_Screen,
};

class QOpenGLTexture;
class Billboard : public Object
{
public:
    EBillboardType        billboardType;
private:
    QOpenGLTexture*       pTex = nullptr;
public:
    void                  SetImage(QImage& image);
    void                  SetBillboardType(EBillboardType type) { billboardType = type;}

    void                  Create() override;
    void                  LateRender() override;
    void                  Draw() override {} //do nothing
    void                  UpdateLocation() override;
};

#endif // ICON_H
