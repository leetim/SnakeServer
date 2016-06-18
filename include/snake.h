#pragma once
#include <objects.h>
#include <colider.h>
#include <set>

class Snake;

typedef std::set<State> StateSet;
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
    void move_to(u_llong steps);

private:
    u_llong get_state_number();
    void get_fragments(PIterator begin, PIterator end, const Point& dir);
    StateSet states;
    ObjVector fragments;
    PHead head;
    u_llong state_number;
};
