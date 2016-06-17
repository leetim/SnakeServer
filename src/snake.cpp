#include <snake.h>

Colider Snake::colider;

Snake::Snake(const Point& coord, const Point& dir, u_llong first_step){
    head = new Head(coord, dir);
    colider.add_object(head);
    fragments.push_back(head);
    state_number = first_step;
}

Snake::Snake(std::vector<Point>&& other, const Point& dir, u_llong first_step){
    get_fragments(other.begin(), other.end(), dir);
    state_number = first_step;
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
    State s(get_state_number());
    s.add_point(head->get_dir());
    for (auto i = fragments.begin(); i != fragments.end(); i++){
        s.add_point((*i)->get_coord());
    }
    states.insert(s);
    if (states.size() > 20){
        states.erase(states.begin());
    }
    for (auto i = fragments.rbegin(); i != fragments.rend(); i++){
        (*i)->move();
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
            for (u_int i = 0; i < fragments.size(); i++){
                fragments[i]->kill();
            }
        }
    }
}

void Snake::move_to(u_llong step){
    if (step > state_number){
        while (step != state_number){
            move();
        }
    }
    else{
        State it = *states.find(step);
        states.erase(states.find(step), states.end());
        for (u_int i = 0; i < fragments.size(); i++){
            fragments[i]->kill();
        }
        fragments.clear();
        get_fragments(it.begin()+1, it.end(), it[0]);
    }
}


u_llong Snake::get_state_number(){
    state_number++;
    auto t = state_number;
    return t;
}

void Snake::get_fragments(PIterator begin, PIterator end, const Point& dir){
    head = new Head(*begin, dir);
    fragments.reserve((end - begin)*3/2);
    fragments.push_back(head);
    for (auto i = begin+1; i != end; i++){
        PObject t = new Body(*i, fragments.back());
        colider.add_object(t);
        fragments.push_back(t);
    }
}
