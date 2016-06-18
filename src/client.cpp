#include <client.h>

using namespace std;
using namespace asio;

Client::Client(int height, int width): m_socket(Talker::service){

}

void Client::loop(){

}

void Client::redraw(){

}

void Client::send_all(){

}

void Client::listen_all(){

}

void Client::read_now(){

}

void Client::read_snake(){
    u_int length;
    m_socket.read_some(buffer(&length, sizeof(u_int)));
    int* buf = new int[length];
    m_socket.read_some(buffer(&buf, length*sizeof(int)));
    vector<Point> vect;
    vect.reserve(length / 2);
    Point dir = Point(buf[0], buf[1]);
    int* buf2 = buf + 2;
    for (u_int i = 0; i < length/2 - 1; i++){
        vect.emplace_back(buf2[2*i], buf2[2*i+1]);
    }
    PSnake s = new Snake(move(vect), dir, current_step);
    snakes.push_back(s);
    delete[] buf;
}

void Client::read_all_dirs(){

}

void Client::read_food(){
    int xy[2];
    m_socket.read_some(buffer(xy, 2*sizeof(int)));
    Snake::colider.add_object(new Food(Point(xy[0], xy[1])));
}
