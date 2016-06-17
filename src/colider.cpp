#include <colider.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//State
State::State(int num): number(num){
    points = new PointsVector;
}

State::~State(){
    delete points;
}

void State::add_point(const Point& new_point){
    points->push_back(new_point);
}

u_llong State::get_number(){
    return number;
}

Point State::operator[](int i){
    return points->at(i);
}

u_int State::size(){
    return points->size();
}

State::iterator State::begin(){
    return points->begin();
}

State::iterator State::end(){
    return points->end();
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

void Colider::colide_all(PObject obj){
    for (u_int i = 0; i < objects.size(); i++){
        if (!obj->is_alive()){
            continue;
        }
        obj->colide(objects[i]);
    }
}

void Colider::clear_dead(){
    vector<PObject> temp;
    vector<PObject> dead;
    for (u_int i = 0; i < objects.size(); i++){
        if (objects[i]->is_alive()){
            temp.push_back(objects[i]);
        }
        else{
            dead.push_back(objects[i]);
        }
    }
    for (u_int i = 0; i < dead.size(); i++){
        delete dead[i];
    }
}

u_int Colider::size(){
    return objects.size();
}
