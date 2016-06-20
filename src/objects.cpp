#include <objects.h>
#include <stdlib.h>
#include <acts.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
//Head

Head::Head(const Point& _coord, const Point& _dir){
    coord = _coord;
    dir = _dir;
    alive = true;
}

void Head::move(){
    coord = coord + dir;
}

void Head::colide(PObject other){
    if (other->coord != coord + dir){
        return;
    }
    other->colide(PHead(this));
}

void Head::colide(PHead other){
    if (rand()%2){
        other->alive = false;
    }
    else{
        this->alive = false;
    }
    throw KillSnake();
}

////////////////////////////////////////////////////////////////////////////////
//Body

Body::Body(const Point& _coord, PObject _next){
    coord = _coord;
    next = _next;
    alive = true;
}

void Body::move(){
    coord = next->coord;
}

void Body::colide(PHead other){
    other->alive = false;
    throw KillSnake();
}

////////////////////////////////////////////////////////////////////////////////
//Food

Food::Food(const Point& _coord){
    coord = _coord;
}

void Food::colide(PHead other){
    this->alive = false;
    throw KillFood();
}

////////////////////////////////////////////////////////////////////////////////
//Wall

Wall::Wall(const Point& _coord){
    coord = _coord;
}

void Wall::colide(PHead other){
    other->alive = false;
    throw KillSnake();
}
