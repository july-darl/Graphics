#ifndef NOISE_H
#define NOISE_H
#include <QVector3D>

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

class Noise
{
private:
    bool            bInitPerlinNoise = false;
    float           p[B + B + 2];
    QVector3D       g3[B + B + 2];

    void            SetUp(float data, int b0, int b1, float r0, float r1);
    float           Curve(float t) { return t * t * t * (t * (t * 6.f - 15.f) + 10.f); }
    float           at3(float rx,float ry,float rz, QVector3D& q) { return rx * q.x() + ry * q.y() + rz * q.z();}
    float           lerp(float t, float a, float b) { return a + t * (b - a); }
    float           hash(int i, int j ,int k);
    float           distance2(QVector3D& a, QVector3D& b);
    int             Random(int seed);
public:
    void            InitPerlinNoise();
    float           GenPerlinNoise(float x, float y, float z);
    float           GenWorleyNoise(float x, float y, float z);
};

#endif // NOISE_H
