#include <server.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace asio;

////////////////////////////////////////////////////////////////////////////////
//Talker

Talker::endpoint Talker::ep(Talker::tcp::v4(), 18008);
io_service Talker::service;

Talker::Talker(SendFunction read_all, PServer server): m_socket(service),
read_thread(read_all, this, server){
    read_thread.detach();
    ready = false;
}

Talker::~Talker(){
    m_socket.close();
    mySnake->kill();
}

bool Talker::is_closed(){
    return !m_socket.is_open();
}

void Talker::read_dir(){
    int xy[2];
    m_socket.read_some(buffer(xy, sizeof(xy)));
    cout << "done: " << xy[0] << " " << xy[1] << endl;
    Point dir(xy[0], xy[1]);
    mySnake->change_dir(dir);
}

void Talker::send_all(){
    u_int length = Snake::colider.size();
    m_socket.write_some(buffer(&length, sizeof(u_int)));
    MapPoint* points = new MapPoint[length];
    for (u_int i = 0; i < Snake::colider.size(); i++){
        PObject o = Snake::colider[i];
        points[i].x = o->get_coord().x;
        points[i].y = o->get_coord().y;
        points[i].c = o->get_char();
    }
    m_socket.write_some(buffer(points, length*sizeof(MapPoint)));
}

void Talker::start(){
    ready = true;
}

////////////////////////////////////////////////////////////////////////////////
//Server

mutex Server::obj_lock;
mutex Server::talkers_lock;
mutex Server::out_lock;

MS Server::tick_length = MS(400);
TimePoint Server::next_tick = TimePoint(Clock::now() + tick_length);

void Server::read_all(PTalker t, PServer serv){
    while (!t->ready){
        this_thread::sleep_for(tick_length);
    }
    u_int comand;
    Talker::socket& sock = t->m_socket;
    try{
        while (!t->is_closed()){
            sock.read_some(buffer(&comand, sizeof(u_int)));
            t->read_dir();
        }
    }
    catch(...){
        sock.close();
        return;
    }
}

void Server::accept_all(PServer serv){
    Talker::acceptor acc(Talker::service, Talker::ep);
    while (true){
        if (serv->talkers.size() >= 5){
            this_thread::sleep_for(tick_length);
        }
        PTalker t = new Talker(read_all, serv);
        acc.accept(t->m_socket);
        try{
            obj_lock.lock();
            talkers_lock.lock();
            serv->talkers.push_back(t);
            serv->add_snake();
            talkers_lock.unlock();
            obj_lock.unlock();
        }
        catch(int){
            delete t;
        }
    }
}

void Server::write_all(){
    for (auto it = talkers.begin(); it != talkers.end(); it++){
        (*it)->send_all();
    }
}


Server::Server(int height, int width): current_step(){
    map_rect = Point(width, height);
    for (int i = -1; i <= width; i++){
        Snake::colider.add_wall(new Wall(Point(i, -1)));
        Snake::colider.add_wall(new Wall(Point(i, height)));
    }
    for (int i = 0; i < height; i++){
        Snake::colider.add_wall(new Wall(Point(-1, i)));
        Snake::colider.add_wall(new Wall(Point(width, i)));
    }
    add_food();
}

void Server::loop(){
    std::thread accepter_thread(accept_all, this);
    accepter_thread.detach();
    next_tick = Clock::now();
    cout << "Server start" << endl;
    while (true){
        while (next_tick >= Clock::now()){
            this_thread::sleep_until(next_tick);
        }
        next_tick += tick_length;
        current_step++;
        cout << current_step << " step " << talkers.size() << " " << snakes.size() << endl;
        obj_lock.lock();
        for (auto it = snakes.begin(); it != snakes.end(); it++){
            (*it)->move();
            Point p = (*it)->get_head_coord();
            cout << p.x << " " << p.y << endl;
        }
        if (!lone_food->is_alive()){
            add_food();
        }
        clear_talkers();
        clear_snakes();
        Snake::colider.clear_dead();
        write_all();
        obj_lock.unlock();
    }
}

void Server::add_snake(){
    Point pos = get_clear_point(map_rect.x, map_rect.y);
    Point dir = Point::dirByCode(rand()%4);
    PSnake s = new Snake(pos, dir, current_step);
    snakes.push_back(s);
}

void Server::add_food(){
    Point pos = get_clear_point(map_rect.x, map_rect.y);
    PFood f = new Food(pos);
    Snake::colider.add_object(f);
    lone_food = f;
}


void Server::clear_talkers(){
    vector<PTalker> temp;
    temp.reserve(talkers.capacity());
    for (auto it = talkers.begin(); it != talkers.end(); it++){
        if ((*it)->is_closed() || !(*it)->mySnake->is_alive()){
            delete *it;
        }
        else{
            temp.push_back(*it);
        }
    }
    talkers = temp;
}

void Server::clear_snakes(){
    vector<PSnake> temp;
    temp.reserve(snakes.capacity());
    for (auto it = snakes.begin(); it != snakes.end(); it++){
        if (!(*it)->is_alive()){
            delete *it;
        }
        else{
            temp.push_back(*it);
        }
    }
    snakes = temp;
}

Point Server::get_clear_point(int max_x, int max_y){
    Colider& col = Snake::colider;
    Point p = Point::randomPoint(max_x, max_y);
    int k = 0;
    while (k < 100){
        try{
            for (u_int i = 0; i < col.size(); i++){
                if (col[i]->is_alive() && col[i]->get_coord() == p){
                    throw 0;
                }
            }
            return p;
        }
        catch(int e){
            k++;
            p = Point::randomPoint(max_x, max_y);
        }
    }
    throw 0;
    return Point();
}
