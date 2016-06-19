#include <Point.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>

using namespace std;

const Point Point::dir_top = Point(0, -1);
const Point Point::dir_left = Point(-1, 0);
const Point Point::dir_bottom = Point(0, 1);
const Point Point::dir_right = Point(1, 0);

Point::Point(const Point& other){
	x = other.x;
	y = other.y;
}

Point Point::randomPoint(int max_x, int max_y){
	return Point(rand()%max_x, rand()%max_y);
}

Point Point::dirByCode(int code){
	switch(code){
	case 0:
		return dir_left;
	case 1:
		return dir_top;
	case 2:
		return dir_right;
	case 3:
		return dir_bottom;
	default:
		return Point();
	}
}

Point& Point::operator=(const Point& other){
	x = other.x;
	y = other.y;
	return *this;
}

int Point::len2() const{
	return x*x + y*y;
}

Point Point::operator-(){
	return Point(-x, -y);
}

Point operator+(const Point& l, const Point& r){
	return Point(l.x + r.x, l.y + r.y);
}


Point operator-(const Point& l, const Point& r){
	return Point(l.x - r.x, l.y - r.y);
}

Point operator/(const Point& l, const int& right){
	return Point(l.x/right, l.y/right);
}

bool operator==(const Point& l, const Point& r){
	return (l.x == r.x)&&(l.y == r.y);
}

bool operator!=(const Point& l, const Point& r){
	return !(l == r);
}

bool operator<(const Point& l, const Point& r){
	return (l.x < r.x && l.y < r.y) && l != r;
}
bool operator>(const Point& l, const Point& r){
	return (l.x > r.x && l.y > r.y) && l != r;
}
