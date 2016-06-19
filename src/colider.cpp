#include <colider.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//State
State::State(): number(){}

State::State(int num): number(num){}

State::State(const State& other): points(other.points), number(other.number){}

void State::add_point(const Point& new_point){
    points.push_back(new_point);
}

u_llong State::get_number(){
    return number;
}

Point State::operator[](int i){
    return points.at(i);
}

u_int State::size(){
    return points.size();
}

State::iterator State::begin(){
    return points.begin();
}

State::iterator State::end(){
    return points.end();
}

bool operator<(const State& left, const State& right){
    return left.number < right.number;
}

bool operator==(const State& left, const State& right){
    return left.number == right.number;
}

////////////////////////////////////////////////////////////////////////////////
//Colider

PObject Colider::operator[](int i){
    return objects[i];
}

void Colider::add_object(PObject obj){
    objects.push_back(obj);
}

void Colider::add_food(PFood obj){
    foods.push_back(obj);
    add_object(obj);
}

void Colider::add_wall(PWall obj){
    walls.push_back(obj);
    add_object(obj);
}

void Colider::colide_all(PObject obj){
    for (u_int i = 0; i < objects.size(); i++){
        if (!obj->is_alive()){
            continue;
        }
        obj->colide(objects[i]);
    }
}

void Colider::clear_food(){
    foods.clear();
}

void Colider::clear_dead(){
    vector<PObject> temp;
    for (u_int i = 0; i < objects.size(); i++){
        if (objects[i]->is_alive()){
            temp.push_back(objects[i]);
        }
        else{
            delete objects[i];
        }
    }
    objects = temp;
}


PFood Colider::get_foods(int i){
    return foods[i];
}

u_int Colider::foods_size(){
    return foods.size();
}

u_int Colider::size(){
    return objects.size();
}
