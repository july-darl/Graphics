#ifndef REFLRECT_H
#define REFLRECT_H
#include <QString>
#include <QObject>
#include <string>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>

class Object;

#include <map>
using namespace std;
#define REGISTER(class_name)                                                    \
     class class_name##Generator : public Generator{                            \
     public:class_name##Generator() {                                           \
        Helper::Inst()->Push(#class_name, this);}                               \
        Object* Create() {return new class_name();}};                           \
      static class_name##Generator* class_name##Inst = new class_name##Generator();

class Generator
{
public:
    virtual Object* Create() = 0;
    virtual ~Generator() { }
};

class Helper
{
private:
    map<string, Generator*> mapStr2Generator;
    static Helper* helper;
    Helper() {}
public:
    static Helper* Inst()
    {
        if (!helper)
        {
            helper = new Helper();
        }
        return helper;
    }
    Object* CreateObject(string name)
    {
        if (mapStr2Generator.find(name) != mapStr2Generator.end())
            return mapStr2Generator[name]->Create();
        return nullptr;
    }
    void Push(string name, Generator* generator)
    {
        mapStr2Generator[name] = generator;
    }
};

#endif // REFLRECT_H
