#ifndef HALO_H
#define HALO_H


class Halo
{
private:
    bool bCreate = false;
    class Icon* halo[10];
public:
    void Create();
    void UpdateLocation();
};

#endif // HALO_H
