#include <snake.h>

using namespace std;

Colider Snake::colider;

Snake::Snake(const Point& coord, const Point& dir, u_llong first_step){
    head = PHead(new Head(coord, dir));
    colider.add_object(head);
    fragments.push_back(head);
    state_number = first_step;
    alive = true;
    for (int i = 0; i < 3; i++){
        PObject t = PObject(new Body(fragments.back()->get_coord(), fragments.back()));
        fragments.push_back(t);
        colider.add_object(t);
    }
}

Snake::Snake(std::vector<Point>&& other, const Point& dir, u_llong first_step){
    get_fragments(other.begin(), other.end(), dir);
    state_number = first_step;
    alive = true;
}

Point Snake::get_head_coord(){
    if (!alive){
        return Point();
    }
    return head->get_coord();
}

Point Snake::get_dir(){
    if (!alive){
        return Point();
    }
    return head->get_dir();
}
void Snake::change_dir(const Point& new_dir){
    if (!alive){
        return;
    }
    Point dir = head->get_dir();
    if (dir == new_dir || new_dir == -dir || new_dir.len2() != 1){
        return;
    }
    head->change_dir(new_dir);
}

PObject Snake::operator[](int ind){
    return fragments[ind];
}

u_int Snake::size(){
    return fragments.size();
}

void Snake::move(){
    if (!alive){
        return;
    }
    try{
        colider.colide_all(head);
    }
    catch(KillFood){
        PObject t = PObject(new Body(fragments.back()->get_coord(), fragments.back()));
        fragments.push_back(t);
        colider.add_object(t);
    }
    catch(KillSnake){
        if (!head->is_alive()){
            kill();
            return;
        }
    }
    for (auto i = fragments.rbegin(); i != fragments.rend(); i++){
        (*i)->move();
    }
}

void Snake::kill(){
    if (!alive){
        return;
    }
    alive = false;
    for (u_int i = 0; i < fragments.size(); i++){
        fragments[i]->kill();
        if (i%3 == 1){
            colider.add_object(PFood(new Food(fragments[i]->get_coord())));
        }
    }
}

bool Snake::is_alive(){
    return alive;
}

void Snake::get_fragments(PIterator begin, PIterator end, const Point& dir){
    head = PHead(new Head(*begin, dir));
    fragments.reserve((end - begin)*3/2);
    colider.add_object(head);
    fragments.push_back(head);
    for (auto i = begin+1; i != end; i++){
        PObject t = PObject(new Body(*i, fragments.back()));
        colider.add_object(t);
        fragments.push_back(t);
    }
}
