#include "noise.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
void Noise::InitPerlinNoise()
{
    int i, j, k;

    for (i = 0 ; i < B ; i++)
    {
        p[i] = i;
        g3[i].setX((float)((rand() % (B + B)) - B) / B);
        g3[i].setY((float)((rand() % (B + B)) - B) / B);
        g3[i].setZ((float)((rand() % (B + B)) - B) / B);

        g3[i].normalize();
    }

    while (--i)
    {
        k = p[i];
        p[i] = p[j = rand() % B];
        p[j] = k;
    }

    for (i = 0 ; i < B + 2 ; i++)
    {
        p[B + i] = p[i];
        g3[B + i].setX(g3[i].x());
        g3[B + i].setY(g3[i].y());
        g3[B + i].setZ(g3[i].z());
    }

    for(int i = 0;i<B+ B+2;i++)
    {
      // std::cout << "(" <<g3[i].x() <<","<<g3[i].y()<<","<<g3[i].z()<<"),\n";
      // std::cout << p[i] << ",";
    }
}

void Noise::SetUp(float data, int b0, int b1, float r0, float r1)
{
    int t = static_cast<int>(data + N);
    b0 = t & BM;
    b1 = (b0 + 1) & BM;
    r0 = t - (int)t;
    r1 = r0 - 1.f;
}

float Noise::GenPerlinNoise(float x, float y, float z)
{
    if(!bInitPerlinNoise)
    {
        InitPerlinNoise();
    }

    int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
    float rx0, rx1, ry0, ry1, rz0, rz1, sy, sz, a, b, c, d, t, u, v;

    SetUp(x, bx0,bx1, rx0,rx1);
    SetUp(y, by0,by1, ry0,ry1);
    SetUp(z, bz0,bz1, rz0,rz1);

    int i = p[ bx0 ];
    int j = p[ bx1 ];

    b00 = p[ i + by0 ];
    b10 = p[ j + by0 ];
    b01 = p[ i + by1 ];
    b11 = p[ j + by1 ];

    t  = Curve(rx0);
    sy = Curve(ry0);
    sz = Curve(rz0);

    u = at3(rx0,ry0,rz0, g3[ b00 + bz0 ]);
    v = at3(rx1,ry0,rz0, g3[ b10 + bz0 ]);
    a = lerp(t, u, v);

    u = at3(rx0,ry1,rz0,g3[ b01 + bz0 ] );
    v = at3(rx1,ry1,rz0,g3[ b11 + bz0 ]);
    b = lerp(t, u, v);

    c = lerp(sy, a, b);

    u = at3(rx0,ry0,rz1,g3[ b00 + bz1 ]);
    v = at3(rx1,ry0,rz1,g3[ b10 + bz1 ]);
    a = lerp(t, u, v);

    u = at3(rx0,ry1,rz1,g3[ b01 + bz1 ]);
    v = at3(rx1,ry1,rz1,g3[ b11 + bz1 ]);
    b = lerp(t, u, v);

    d = lerp(sy, a, b);

    return lerp(sz, c, d);
}

float Noise::hash(int i, int j ,int k)
{
    unsigned int n = 1 << 32;
    return (541 * i + 79 * j + 31 * k) % n;
}

int Noise::Random(int seed)
{
    unsigned int n = 1 << 31;
    return (1103515245 * seed + 12345) % n;
}

float Noise::distance2(QVector3D& a, QVector3D& b)
{
    return sqrt((a.x() - b.x()) * (a.x() - b.x()) +
                (a.y() - b.y()) * (a.y() - b.y()) +
                (a.z() - b.z()) * (a.z() - b.z()));
}

float Noise::GenWorleyNoise(float x, float y, float z)
{
    QVector3D pos(x,y,z);
    int cubeX, cubeY,cubeZ;

    float closestDistance = 1000000.0f;
    float tempDistance = 0.0f;
    int lastRandom;

    int evalCubeX = static_cast<int>(floor(x));
    int evalCubeY = static_cast<int>(floor(y));
    int evalCubeZ = static_cast<int>(floor(z));


    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            for (int k = -1; k < 2; ++k)
            {
                int cubeX = evalCubeX + i;
                int cubeY = evalCubeY + j;
                int cubeZ = evalCubeZ + k;

                lastRandom = Random(hash(cubeX, cubeY,cubeZ));
                int numberFeaturePoints = lastRandom % 9 + 1;
                for (int l = 0; l < numberFeaturePoints; ++l)
                {
                    QVector3D randomDiff;
                    lastRandom = Random(lastRandom);
                    randomDiff.setX((double)lastRandom / 0x100000000);

                    lastRandom = Random(lastRandom);
                    randomDiff.setY((double)lastRandom / 0x100000000);

                    lastRandom = Random(lastRandom);
                    randomDiff.setZ((double)lastRandom / 0x100000000);

                    QVector3D featurePoint(randomDiff.x() + (double)cubeX, randomDiff.y() + (double)cubeY,randomDiff.z() + (double)cubeZ) ;

                    tempDistance = distance2(featurePoint,pos);
                    if (tempDistance < closestDistance)
                        closestDistance = tempDistance;
                }
            }
        }
    }
    return closestDistance;
}
