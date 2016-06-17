#include <snake.h>

Colider Snake::colider;

Snake::Snake(const Point& coord, const Point& dir){
    head = new Head(coord, dir);
    colider.add_object(head);
    fragments.push_back(head);
}

Snake::Snake(std::vector<Point>&& other, const Point& dir){
    head = new Head(other[0], dir);
    fragments.reserve(other.capacity());
    fragments.push_back(head);
    for (u_int i = 1; i < other.size(); i++){
        PObject t = new Body(other[i], fragments.back());
        colider.add_object(t);
        fragments.push_back(t);
    }
}

Point Snake::get_head_coord(){
    return head->get_coord();
}

PObject Snake::operator[](int ind){
    return fragments[ind];
}

u_int Snake::size(){
    return fragments.size();
}

void Snake::move(){
    for (auto i = fragments.rbegin(); i != fragments.rend(); i++){
        (*i)->move();
    }
}

void Snake::move_back(){

}
