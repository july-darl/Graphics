#ifndef TOOLWIDGET_H
#define TOOLWIDGET_H

#include <QWidget>
#include <QHBoxLayout>

class QLabel;
class QSlider;
class Billboard;

class CLightWidget : public QWidget
{
    Q_OBJECT
private:
    size_t m_activeId = 0;

public:
    CLightWidget();
    QLabel*         curLightLabel;
    QSlider*        KqSlider;
    QLabel*         KqLabel;
    QSlider*        m_slider2[3];
    QLabel*         m_label2[3];

    QSlider*        hdrSlider;
    QLabel*         hdrLabel;
    Billboard*      icon = nullptr;
public slots:
    void            InitParam();
    void            OnKqScroll(int value);
    void            OnScroll2(int value);
    void            OnSelectLight(int index);

    void            OnHDRScroll(int value);
};

#endif // TOOLWIDGET_H
