#pragma once
#include <objects.h>
#include <colider.h>
#include <set>

class Snake;

typedef PointsVector::iterator PIterator;
typedef Snake* PSnake;

class Snake{
public:
    static Colider colider;

    Snake(const Point& coord, const Point& dir, u_llong first_step);
    Snake(std::vector<Point>&& other, const Point& dir, u_llong first_step);
    Point get_head_coord();
    Point get_dir();
    void change_dir(const Point& new_dir);
    PObject operator[](int ind);
    u_int size();
    void move();
    void kill();
    bool is_alive();

private:
    u_llong get_state_number();
    void get_fragments(PIterator begin, PIterator end, const Point& dir);
    ObjVector fragments;
    PHead head;
    bool alive;
    u_llong state_number;
};
