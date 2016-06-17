#pragma once
#include <objects.h>
#include <colider.h>

class Snake{
public:
    static Colider colider;

    Snake(const Point& coord, const Point& dir);
    Snake(std::vector<Point>&& other, const Point& dir);
    Point get_head_coord();
    PObject operator[](int ind);
    u_int size();
    void move();
    void move_back();

private:
    ObjVector fragments;
    PHead head;
}
