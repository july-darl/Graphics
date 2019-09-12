#include "propertywidget.h"
#include <QVBoxLayout>

PropertyWidget::PropertyWidget(QWidget* parent)
    : QWidget(parent)
{
    vlayout = new QVBoxLayout;
    setLayout(vlayout);
}

void PropertyWidget::SetLabel(QString label)
{
    QLabel* l = new QLabel(label);
    vlayout->addWidget(l);
}
