#include <stdlib.h>
#include "particlesystem.h"
#include "rendercommon.h"
#include "resourceinfo.h"

REGISTER(ParticleEmitter)

void Particle::Update(QOpenGLShaderProgram* program)
{
    program->setUniformValue("color",color);

    place.setY(place.y() + speed.y());
    place.setX(place.x() + speed.x());
    place.setZ(place.z() + speed.z());

    speed.setY(speed.y() + acc.y());
    speed.setX(speed.x() + acc.x());
    speed.setZ(speed.z() + acc.z());

    life -= 0.2f;
}

Particle::Particle() {}

Particle::Particle(QVector3D _size, QVector3D _speed, QVector3D _acc, QVector3D _angle,
    float _life, unsigned int _texture)
{
    size = _size;
    speed = _speed;
    acc = _acc;
    life = _life;
    texture = _texture;
    angle = _angle;
    has_tex = true;
}

Particle::Particle(QVector3D _size, QVector3D _speed, QVector3D _acc, QVector3D _angle,
    float _life, QVector3D _color)
{
    size = _size;
    speed = _speed;
    acc = _acc;
    life = _life;
    color = _color;
    angle = _angle;
    has_tex = false;
}

void ParticleEmitter::Emit(function<Particle*()> init, function<bool(Particle*)> judge)
{
    bForever = true;
    for (int i = 0; i < speed; i++)
    {
        Init = init;
        IsDead = judge;
        p[i] = Init();
        p[i]->id = static_cast<unsigned int>(i);

        int place_x = rand() % speed;
        int place_y = rand() % speed;
        int place_z = rand() % speed;

        p[i]->place.setX(1.0f * place_x / speed*(x2 - x1) + x1);
        p[i]->place.setY(1.0f * place_y / speed*(y2 - y1) + y1);
        p[i]->place.setZ(1.0f * place_z / speed*(z2 - z1) + z1);
    }
}

void ParticleEmitter::Emit(function<Particle*()> init)
{
    bForever = false;
    for (int i = 0; i < speed; i++)
    {
        Init = init;
        p[i] = init();
        p[i]->id = static_cast<unsigned int>(i);

        int place_x = rand() % speed;
        int place_y = rand() % speed;
        int place_z = rand() % speed;

        p[i]->place.setX(1.0f * place_x / speed*(x2 - x1) + x1);
        p[i]->place.setY(1.0f * place_y / speed*(y2 - y1) + y1);
        p[i]->place.setZ(1.0f * place_z / speed*(z2 - z1) + z1);
    }
}


void ParticleEmitter::Reset()
{
    for (int i = 0; i < speed; i++)
    {
        p[i] = Init();
        p[i]->bDead = false;
        int place_x = rand() % speed;
        int place_y = rand() % speed;
        int place_z = rand() % speed;
        p[i]->place.setX(1.0f * place_x / speed*(x2 - x1) + x1);
        p[i]->place.setY(1.0f * place_y / speed*(y2 - y1) + y1);
        p[i]->place.setZ(1.0f * place_z / speed*(z2 - z1) + z1);
    }
    num = speed;
}

void ParticleEmitter::Create()
{
    program = CResourceInfo::Inst()->CreateProgram("colorvshader.vsh","colorfshader.fsh","ColorVShader");
}

void ParticleEmitter::Render()
{
    program->bind();
    program->setUniformValue("ModelMatrix",modelMatrix);
    program->setUniformValue("IT_ModelMatrix",IT_modelMatrix);


    for (int i = 0; i < speed; i++)
    {
        p[i]->Update(program);
        Draw();

        if (p[i]->life < 0 || (bForever && IsDead(p[i])))
        {
            delete p[i];
            p[i] = Init();
            int place_x = rand() % speed;
            int place_y = rand() % speed;
            int place_z = rand() % speed;
            p[i]->place.setX(1.0f * place_x / speed*(x2 - x1) + x1);
            p[i]->place.setY(1.0f * place_y / speed*(y2 - y1) + y1);
            p[i]->place.setZ(1.0f * place_z / speed*(z2 - z1) + z1);
        }
    }
}

void ParticleEmitter::SetParam(int _speed, float _x1,
    float _x2, float _y1, float _y2,float _z1,float _z2) {

    speed = _speed;
    x1 = _x1;
    x2 = _x2;
    y1 = _y1;
    y2 = _y2;
    z1 = _z1;
    z2 = _z2;

    unsigned long long arrSize = static_cast<unsigned long long>(speed);
    p = new Particle*[arrSize];

    num = speed;
}
