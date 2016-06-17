#pragma once
#include <Point.h>
#include <acts.h>

class Object;
class Head;
class Body;
class Food;
class Wall;

typedef Object* PObject;
typedef Head* PHead;
typedef Body* PBody;
typedef Food* PFood;
typedef Wall* PWall;

class Object{
public:
    friend class Head;
    friend class Body;
    friend class Food;
    friend class Wall;

    Object(): alive(true){};
    virtual ~Object(){};
    virtual void move()=0;
    virtual void move_to(const Point& new_coord){
        coord = new_coord;
    };
    virtual unsigned char get_char()=0;

    virtual void colide(PObject other)=0;
    virtual void colide(PHead other)=0;
    virtual void colide(PBody other)=0;
    virtual void colide(PFood other)=0;
    virtual void colide(PWall other)=0;

    Point get_coord(){
        return coord;
    };

    bool is_alive(){
        return alive;
    };

protected:
    Point coord;
    bool alive;
};

class Head: public Object{
public:
    friend class Body;
    friend class Food;
    friend class Wall;

    Head(const Point& coord, const Point& dir);
    void move();
    virtual unsigned char get_char(){
        return '&';
    }

    void colide(PObject other);
    void colide(PHead other);
    void colide(PBody other){};
    void colide(PFood other){};
    void colide(PWall other){};
protected:
    Point dir;
};

class Body: public Object{
public:
    friend class Head;
    friend class Food;
    friend class Wall;

    Body(const Point& coord, PObject next);
    void move();
    virtual unsigned char get_char(){
        return 'o';
    }

    void colide(PObject other){};
    void colide(PHead other);
    void colide(PBody other){};
    void colide(PFood other){};
    void colide(PWall other){};
protected:
    PObject next;
};

class Food: public Object{
public:
    friend class Head;
    friend class Body;
    friend class Wall;

    Food(const Point& coord);
    void move(){};
    virtual unsigned char get_char(){
        return '%';
    }

    void colide(PObject other){};
    void colide(PHead other);
    void colide(PBody other){};
    void colide(PFood other){};
    void colide(PWall other){};
};

class Wall: public Object{
public:
    friend class Head;
    friend class Body;
    friend class Food;

    Wall(const Point& coord);
    void move(){};
    virtual unsigned char get_char(){
        return '#';
    }

    void colide(PObject other){};
    void colide(PHead other);
    void colide(PBody other){};
    void colide(PFood other){};
    void colide(PWall other){};
};
