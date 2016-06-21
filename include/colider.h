#pragma once
#include <objects.h>
#include <vector>
#include <iostream>///////////////////////

class Colider;

typedef std::vector<PObject> ObjVector;
typedef std::vector<Point> PointsVector;

class Colider{
public:
    PObject operator[](int i);
    void add_object(PObject obj);
    void colide_all(PObject obj);
    void clear_dead();
    u_int size();
private:
    std::vector<PObject> objects;
};
