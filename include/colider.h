#pragma once
#include <objects.h>
#include <vector>

typedef std::vector<PObject> ObjVector;
typedef std::vector<Points> PointsVector;

class State{
public:
    State(int num);
    void add_point(const Point& new_point);
    u_llong get_number();
    Point operator[](int i);
    u_int size();

    friend bool operator<(State* left, State* right);
private:
    PointsVector points;
    u_llong number;
}

class Colider{
public:
    PObject operator[](int i);
    void add_object(PObject obj);
    void colide_all(PObject obj);
    void clear_dead();
    u_int size();
private:
    std::vector<PObject> objects;
}
