#ifndef ENVIRONMENTWIDGET_H
#define ENVIRONMENTWIDGET_H

#include <QWidget>
#include <QVector3D>
#include "rendercommon.h"
#include "propertywidget.h"

class CEnvironmentWidget : public PropertyWidget
{
    Q_OBJECT
public:
    CEnvironmentWidget(QWidget* parent = nullptr);
private:
    DECLARE_CHECKBOX(hdr)
    DECLARE_CHECKBOX(gamma)
    DECLARE_SLIDER(exposure)
    DECLARE_IMAGE2(background)
public:
    void OnSelected();
};
#endif // ENVIRONMENTWIDGET_H
