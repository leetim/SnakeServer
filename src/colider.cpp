#include <colider.h>

using namespace std;

PObject Colider::operator[](int i){
    return objects[i];
}

void Colider::add_object(PObject obj){
    objects.push_back(obj);
}

void Colider::colide_all(PObject obj){
    for (u_int i = 0; i < objects.size(); i++){
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
