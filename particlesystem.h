#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#pragma once
#define GLUT_DISABLE_ATEXIT_HACK
#include <QVector3D>
#include <functional>
#include <QOpenGLShaderProgram>
#include "object.h"

using namespace std;
class Particle
{
    bool                has_tex;//纹理或颜色
    bool                bDead = false;

    QVector3D           place;//位置
    QVector3D           size;//大小
    QVector3D           speed;// 速度
    QVector3D           acc;//加速度
    QVector3D           angle;//角度
    QVector3D           color;//颜色

    unsigned int        texture;//纹理
    unsigned int        id;
    float               life;//生命

    friend class        ParticleEmitter;

    void                Update(QOpenGLShaderProgram* program);
public:
    Particle();
    const QVector3D&    GetPlace() { return place; }
    const QVector3D&    GetSize() { return size; }
    const QVector3D&    GetAngle(){ return angle; }

    Particle(QVector3D size,QVector3D speed,QVector3D acc,QVector3D angle,
        float _life, unsigned int _texture);

    Particle(QVector3D size, QVector3D speed, QVector3D acc, QVector3D angle,
        float _life, QVector3D _color);

};

class ParticleEmitter : public Object
{
    bool                bEnableCollision = false;
    float               x1, y1, x2, y2, z1, z2; //发射器位置
    int                 speed;//发射速率
    bool                bForever;
    int                 num;
    Particle**          p;//发射粒子
    function<Particle*()> Init;
    //初始化粒子的函数指针
    function<bool(Particle*)> IsDead;
public:
    void Emit(function<Particle*()> init, function<bool(Particle*)> judge);
    void Emit(function<Particle*()> init);

    void Reset();

    void Create() override;
    void Render() override;

    void SetParam(int _speed, float _x1,
        float _x2, float _y1, float _y2, float _z1, float _z2);
};
#endif // PARTICLESYSTEM_H
