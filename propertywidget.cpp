#include "propertywidget.h"
#include <QVBoxLayout>

PropertyWidget::PropertyWidget()
{
    vlayout = new QVBoxLayout;
    setLayout(vlayout);
}

void PropertyWidget::SetLabel(QString label)
{
    QLabel* l = new QLabel(label);
    vlayout->addWidget(l);
}
