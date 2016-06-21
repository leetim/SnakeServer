#include <colider.h>
#include <iostream>

using namespace std;

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
        if (!objects[i]->is_alive()){
            cout << 123 << endl;
            continue;
        }
        obj->colide(objects[i]);
    }
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

u_int Colider::size(){
    return objects.size();
}
