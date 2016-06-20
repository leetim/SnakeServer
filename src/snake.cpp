#include <snake.h>

using namespace std;

Colider Snake::colider;

Snake::Snake(const Point& coord, const Point& dir, u_llong first_step){
    head = new Head(coord, dir);
    colider.add_object(head);
    fragments.push_back(head);
    state_number = first_step;
    alive = true;
}

Snake::Snake(std::vector<Point>&& other, const Point& dir, u_llong first_step){
    get_fragments(other.begin(), other.end(), dir);
    state_number = first_step;
    alive = true;
}

Point Snake::get_head_coord(){
    return head->get_coord();
}

Point Snake::get_dir(){
    return head->get_dir();
}
void Snake::change_dir(const Point& new_dir){
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
    State s(get_state_number());
    s.add_point(head->get_dir());
    for (auto i = fragments.begin(); i != fragments.end(); i++){
        s.add_point((*i)->get_coord());
    }
    states.insert(s);
    if (states.size() > 20){
        states.erase(states.begin());
    }
    try{
        colider.colide_all(head);
    }
    catch(KillFood){
        PObject t = new Body(fragments.back()->get_coord(), fragments.back());
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

void Snake::move_to(u_llong step){
    if (step > state_number){
        while (step != state_number){
            move();
        }
    }
    else{
        StateSet::iterator iter = states.find(State(step));
        if (iter == states.end()){
            for (u_int i = 0; i < fragments.size(); i++){
                fragments[i]->kill();
            }
            return;
        }
        State it = *iter;
        states.erase(iter, states.end());
        for (u_int i = 0; i < fragments.size(); i++){
            fragments[i]->kill();
        }
        fragments.clear();
        get_fragments(it.begin()+1, it.end(), it[0]);
    }
}

void Snake::kill(){
    if (!is_alive()){
        return;
    }
    alive = false;
    for (u_int i = 0; i < fragments.size(); i++){
        fragments[i]->kill();
        if (i%3 == 1){
            colider.add_food(new Food(fragments[i]->get_coord()));
        }
    }
}

bool Snake::is_alive(){
    return alive;
}


u_llong Snake::get_state_number(){
    state_number++;
    auto t = state_number;
    return t;
}

void Snake::get_fragments(PIterator begin, PIterator end, const Point& dir){
    head = new Head(*begin, dir);
    fragments.reserve((end - begin)*3/2);
    colider.add_object(head);
    fragments.push_back(head);
    for (auto i = begin+1; i != end; i++){
        PObject t = new Body(*i, fragments.back());
        colider.add_object(t);
        fragments.push_back(t);
    }
}
