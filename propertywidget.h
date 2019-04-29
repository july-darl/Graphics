#ifndef PROPERTYWIDGET_H
#define PROPERTYWIDGET_H

#include <QObject>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QColorDialog>
#include <QFileDialog>
#include <QImage>
#include <QOpenGLFunctions>
#include <QComboBox>
#include "rendercommon.h"

#define DECLARE_SLIDER(name)\
protected:                                                                                            \
    QSlider* name##slider;                                                                            \
    QLabel* name##label;                                                                              \
    float name##min,name##max;                                                                        \
    float* name##data;                                                                                \
public:                                                                                               \
    void name##Init(string na,float mi,float ma,float* da = nullptr)                                  \
    {                                                                                                 \
        name##min = mi,name##max = ma,name##data = da;                                                \
        QHBoxLayout* hlayout = new QHBoxLayout();                                                     \
        QLabel* text = new QLabel(QString::fromStdString(na));                                        \
        name##slider = new QSlider();                                                                 \
                                                                                                      \
        name##slider->setOrientation(Qt::Horizontal);                                                 \
        if(da)                                                                                        \
            name##label = new QLabel(QString::number(static_cast<double>(*da)));                      \
        else                                                                                          \
            name##label = new QLabel();                                                               \
        hlayout->addWidget(text,1);                                                                   \
        hlayout->addWidget(name##slider,2);                                                           \
        hlayout->addWidget(name##label,1);                                                            \
        vlayout->addLayout(hlayout);                                                                  \
        name##slider->setMinimum(0);                                                                  \
        name##slider->setMaximum(100);                                                                \
        connect(name##slider,&QSlider::valueChanged,this,[&](int value) {                             \
            float t = static_cast<float>(value) / 100 *(name##max - name##min) + name##min;           \
            char buffer[100];                                                                         \
            sprintf(buffer,"%.2f",static_cast<double>(t));                                            \
            name##label->setText(QString(buffer));                                                    \
            if(name##data)*name##data = t;});                                                         \
    }                                                                                                 \
    void name##SetData(float* da)                                                                     \
    {                                                                                                 \
        name##data = da;                                                                              \
        if(da)  {                                                                                     \
            char buffer[100];                                                                         \
            sprintf(buffer,"%.2f",static_cast<double>(*da));                                          \
            name##label->setText(QString(buffer));                                                    \
            name##slider->setValue(static_cast<int>((*da - name##min)/(name##max - name##min)*100));  \
        }                                                                                             \
    }                                                                                                 \

#define DECLARE_CHECKBOX(name)                                                                        \
protected:                                                                                            \
    QCheckBox* name##check;                                                                           \
    bool* name##data;                                                                                 \
public:                                                                                               \
    void name##Init(string na, bool* da = nullptr)                                                    \
    {                                                                                                 \
        name##check = new QCheckBox(QString::fromStdString(na));                                      \
        if(da) name##check->setChecked(*da);                                                          \
        vlayout->addWidget(name##check);                                                              \
        connect(name##check,&QCheckBox::stateChanged,this,[&](int value) {                            \
            *name##data = value != 0;                                                                 \
        });                                                                                           \
    }                                                                                                 \
    void name##SetData(bool* da)                                                                      \
    {                                                                                                 \
        name##data = da;                                                                              \
        if(da) {                                                                                      \
            name##check->setChecked(*da);                                                             \
        }                                                                                             \
    }

#define DECLARE_IMAGE(name)                                                                           \
protected:                                                                                            \
    GLuint* name##tex;                                                                                \
    QImage* name##image;                                                                              \
    QLabel* name##label;                                                                              \
    QPushButton* name##btn;                                                                           \
    QString name##path;                                                                               \
public:                                                                                               \
    void name##Init(string na, GLuint* tex = nullptr, QImage* img = nullptr)                          \
    {                                                                                                 \
        QLabel* label = new QLabel(QString::fromStdString(na));                                       \
        name##tex = tex;                                                                              \
        name##image = img;                                                                            \
        QHBoxLayout* hlayout = new QHBoxLayout();                                                     \
        name##label = new QLabel();                                                                   \
        name##btn = new QPushButton(".");                                                             \
        name##btn->setFixedSize(20,20);                                                               \
        hlayout->addWidget(label);                                                                    \
        hlayout->addWidget(name##label);                                                              \
        hlayout->addWidget(name##btn);                                                                \
        vlayout->addLayout(hlayout);                                                                  \
                                                                                                      \
        connect(name##btn,&QPushButton::clicked,this,[&]() {                                          \
            QString fileName = QFileDialog::getOpenFileName(this, tr("File"),                         \
                   "D:",tr("img(*png *jpg *tga);;"));                                                 \
            if(fileName!="" && name##tex && name##image)                                              \
            {                                                                                         \
                name##image->load(fileName);                                                          \
                *name##image = name##image->convertToFormat(QImage::Format_RGBA8888);                 \
                QPixmap pixmap = QPixmap::fromImage(*name##image);                                    \
                pixmap = pixmap.scaled(QSize(40,40));                                                 \
                name##label->setPixmap(pixmap);                                                       \
                QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();            \
                if(*name##tex!=0)glFuncs->glDeleteTextures(1, name##tex);                             \
                glFuncs->glGenTextures(1, name##tex);                                                 \
                glFuncs->glBindTexture(GL_TEXTURE_2D, *name##tex);                                    \
                glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, name##image->width(),                \
                   name##image->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, name##image->bits());         \
                glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);            \
                glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);            \
                glFuncs->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);                   \
                glFuncs->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);                   \
                glFuncs->glBindTexture(GL_TEXTURE_2D, 0);                                             \
            }                                                                                         \
        });                                                                                           \
    }                                                                                                 \
    void name##SetData(GLuint* tex,QImage* img)                                                       \
    {                                                                                                 \
        name##tex = tex;                                                                              \
        name##image = img;                                                                            \
        if(name##image && !name##image->isNull()) {                                                   \
            QPixmap pixmap = QPixmap::fromImage(*name##image);                                        \
            pixmap = pixmap.scaled(QSize(40,40));                                                     \
            name##label->setPixmap(pixmap);                                                           \
        }                                                                                             \
        else                                                                                          \
        {                                                                                             \
            QPixmap pixmap;                                                                           \
            name##label->setPixmap(pixmap);                                                           \
        }                                                                                             \
    }                                                                                                 \

#define DECLARE_IMAGE2(name)                                                                          \
protected:                                                                                            \
    function<void(QString)> name##callback;                                                           \
    QLabel* name##label;                                                                              \
    QPushButton* name##btn;                                                                           \
    QString name##path;                                                                               \
public:                                                                                               \
    void name##Init(string na, function<void(QString)> func)                                          \
    {                                                                                                 \
        QLabel* label = new QLabel(QString::fromStdString(na));                                       \
        name##callback = func;                                                                        \
        QHBoxLayout* hlayout = new QHBoxLayout();                                                     \
        name##label = new QLabel();                                                                   \
        name##btn = new QPushButton(".");                                                             \
        name##btn->setFixedSize(20,20);                                                               \
        hlayout->addWidget(label);                                                                    \
        hlayout->addWidget(name##label);                                                              \
        hlayout->addWidget(name##btn);                                                                \
        vlayout->addLayout(hlayout);                                                                  \
                                                                                                      \
        connect(name##btn,&QPushButton::clicked,this,[&]() {                                          \
            QString fileName = QFileDialog::getOpenFileName(this, tr("File"),                         \
                   "D:",tr("img(*png *jpg *tga);;"));                                                 \
            if(fileName!="" )                                                                         \
            {                                                                                         \
                QImage img(fileName);                                                                 \
                img = img.convertToFormat(QImage::Format_RGBA8888);                                   \
                QPixmap pixmap = QPixmap::fromImage(img);                                             \
                pixmap = pixmap.scaled(QSize(40,40));                                                 \
                name##label->setPixmap(pixmap);                                                       \
                if(name##callback)name##callback(fileName);                                           \
            }                                                                                         \
        });                                                                                           \
    }                                                                                                 \

#define DECLARE_COLOR(name)                                                                           \
protected:                                                                                            \
    QPushButton* name##colorBtn;                                                                      \
    QLabel* name##label;                                                                              \
    Vector3f* name##data;                                                                             \
public:                                                                                               \
    void name##Init(string na, Vector3f* da = nullptr)                                                \
    {                                                                                                 \
        name##colorBtn = new QPushButton();                                                           \
        name##colorBtn->setFixedSize(40,20);                                                          \
        name##label = new QLabel(QString::fromStdString(na));                                         \
        QHBoxLayout* hlayout = new QHBoxLayout();                                                     \
        if(da)                                                                                        \
        {                                                                                             \
            char buffer[500];                                                                         \
            sprintf(buffer,"background: rgb(%d,%d,%d)",int(da->x*255),int(da->y*255),                 \
                int(da->z*255));                                                                      \
            name##colorBtn->setStyleSheet(buffer);                                                    \
        }                                                                                             \
        hlayout->addWidget(name##label);                                                              \
        hlayout->addWidget(name##colorBtn);                                                           \
        vlayout->addLayout(hlayout);                                                                  \
        connect(name##colorBtn,&QPushButton::clicked,this,[&]() {                                     \
            QColor color = QColorDialog::getColor(Qt::red, this,tr("Color"),                          \
                QColorDialog::ShowAlphaChannel);                                                      \
            if(name##data)*name##data = Vector3f(1.0f*color.red()/255,1.0f*color.green()/255,         \
                1.0f*color.blue()/255);                                                               \
            char buffer[500];                                                                         \
            sprintf(buffer,"background: rgb(%d,%d,%d)",color.red(),color.green(),color.blue());       \
            name##colorBtn->setStyleSheet(buffer);                                                    \
        });                                                                                           \
    }                                                                                                 \
    void name##SetData(Vector3f* da)                                                                  \
    {                                                                                                 \
        name##data = da;                                                                              \
        if(da)                                                                                        \
        {                                                                                             \
            char buffer[500];                                                                         \
            sprintf(buffer,"background: rgb(%d,%d,%d)",int(da->x*255),int(da->y*255),                 \
                int(da->z*255));                                                                      \
            name##colorBtn->setStyleSheet(buffer);                                                    \
        }                                                                                             \
    }                                                                                                 \


class QVBoxLayout;
using namespace std;
class PropertyWidget : public QWidget
{
public:
    PropertyWidget();
protected:
    QVBoxLayout* vlayout;
    void SetLabel(QString label);
};



#endif // PROPERTYWIDGET_H
