#ifndef OUTLINEWIDGET_H
#define OUTLINEWIDGET_H
#include "mainwidget.h"
#include <QWidget>

class Object;
class QTreeWidget;
class OutlineWidget : public QWidget
{
    Q_OBJECT
private:
    enum { SIZE = 5 };
    QTreeWidget* treeWidget;
public:
    OutlineWidget(QWidget* parent = nullptr);
signals:
    void SelectLight(int);
    void SelectObject(Object*);
    void SelectEnvironment();
    void ActiveWindow(EWindowName,void*);
public slots:
    void InitParam();
    void OnClickItem(class QTreeWidgetItem* ,int);
};

#endif // OUTLINEWIDGET_H

