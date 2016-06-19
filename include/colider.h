#pragma once
#include <objects.h>
#include <vector>
#include <iostream>///////////////////////

class State;
class Colider;

typedef std::vector<PObject> ObjVector;
typedef std::vector<Point> PointsVector;
typedef State* PState;

class State{
public:
    typedef PointsVector::iterator iterator;

    State();
    State(int num);
    State(const State& other);
    void add_point(const Point& new_point);
    u_llong get_number();
    Point operator[](int i);
    u_int size();
    iterator begin();
    iterator end();

    friend bool operator<(const State& left, const State& right);
    friend bool operator==(const State& left, const State& right);
private:
    PointsVector points;
    u_llong number;
};

class Colider{
public:
    PObject operator[](int i);
    void add_object(PObject obj);
    void add_food(PFood obj);
    void add_wall(PWall obj);
    void colide_all(PObject obj);
    void clear_dead();
    void clear_food();
    PFood get_foods(int i);
    u_int size();
    u_int foods_size();
    std::vector<PFood> foods;
    std::vector<PWall> walls;
private:
    std::vector<PObject> objects;
};
