#ifndef SWITCHWIDGET_H
#define SWITCHWIDGET_H

#include <QWidget>
#include <QHBoxLayout>

class SwitchWidget : public QWidget
{
    Q_OBJECT
private:
    int m_activeId = 0;
public:
    SwitchWidget();
    class QSlider* m_slider1[3];
    class QLabel* m_label1[3];
    class QSlider* m_slider2[3];
    class QLabel* m_label2[3];
    class Icon* icon = nullptr;
public slots:
   // void InitParam();
   // void OnScroll1(int value);
   // void OnScroll2(int value);
   // void OnSelectLight(int index);
};

#endif // SWITCHWIDGET_H
