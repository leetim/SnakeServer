#include <server.h>
#include <stdlib.h>

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
}

bool Talker::is_closed(){
    return !m_socket.is_open();
}

void Talker::send_snake(PSnake snake){
    m_socket.write_some(buffer(&Server::SEND_SNAKE, sizeof(u_int)));
    Server::obj_lock.lock();
    u_int length = (snake->size()+1)*2;
    int* buf = new int[length];
    buf[0] = snake->get_dir().x;
    buf[1] = snake->get_dir().y;
    for (u_int i = 0; i < snake->size(); i++){
        buf[2*(i + 1)] = (*snake)[i]->get_coord().x;
        buf[2*(i + 1) + 1] = (*snake)[i]->get_coord().y;
    }
    m_socket.write_some(buffer(&length, sizeof(u_int)));
    m_socket.write_some(buffer(buf, length*sizeof(int)));
    delete[] buf;
    Server::obj_lock.unlock();
}

void Talker::send_food(PFood food){
    m_socket.write_some(buffer(&Server::SEND_FOOD, sizeof(u_int)));
    Server::obj_lock.lock();
    int xy[] = {food->get_coord().x, food->get_coord().y};
    m_socket.write_some(buffer(xy, sizeof(xy)));
    Server::obj_lock.unlock();
}

void Talker::read_dir(){
    int xy[2];
    m_socket.write_some(buffer(xy, sizeof(xy)));
    Point dir(xy[0], xy[1]);
    Server::obj_lock.lock();
    mySnake->change_dir(dir);
    Server::obj_lock.unlock();
}

void Talker::send_now(){
    // m_socket.write_some(buffer(&Server::SEND_NOW, sizeof(u_int)));
    auto since_epoch = Clock::now().time_since_epoch();
    int time_now = std::chrono::duration_cast<MS>(since_epoch).count();
    m_socket.write_some(buffer(&time_now, sizeof(int)));
}

void Talker::send_all_dirs(PServer serv){
    m_socket.write_some(buffer(&Server::SEND_ALL_DIRS, sizeof(u_int)));
    u_int snakes_count = serv->snakes.size();
    int* buf = new int[snakes_count*2];
    m_socket.write_some(buffer(&snakes_count, sizeof(int)));
    for (u_int i = 0; i < snakes_count; i++){
        buf[2*i] = serv->snakes[i]->get_dir().x;
        buf[2*i + 1] = serv->snakes[i]->get_dir().y;
    }
    m_socket.write_some(buffer(buf, 2*snakes_count*sizeof(int)));
    delete[] buf;
}

void Talker::start(){
    ready = true;
}

////////////////////////////////////////////////////////////////////////////////
//Server

u_int Server::SNAKE_GAME = 10002;
u_int Server::SEND_SNAKE = 7827;
u_int Server::SEND_FOOD = 92029;
u_int Server::CHANGE_DIR = 111122;
u_int Server::HEAR = 8282;
u_int Server::SEND_NOW = 22112;
u_int Server::SEND_ALL_DIRS = 2122;
u_int Server::END = 2212;

mutex Server::obj_lock;
mutex Server::talkers_lock;
mutex Server::out_lock;

MS Server::tick_length = MS(1000);
TimePoint Server::next_tick = TimePoint(Clock::now() + tick_length);

void Server::read_all(PTalker t, PServer serv){
    while (!t->ready){
        this_thread::sleep_for(MS(1000));
    }
    u_int buf[1024];
    Talker::socket& sock = t->m_socket;
    while (!t->is_closed()){
        sock.read_some(buffer(buf, 1024*sizeof(int)));
        if (*buf == CHANGE_DIR){
            t->read_dir();
            serv->write_all_dirs();
        }
        // if ()

    }
}

void Server::accept_all(PServer serv){
    Talker::acceptor acc(Talker::service, Talker::ep);
    while (true){
        if (serv->talkers.size() >= 5){
            this_thread::sleep_for(MS(1000));
        }
        PTalker t = new Talker(read_all, serv);
        Talker::socket& sock = t->m_socket;
        acc.accept(t->m_socket);
        try{
            obj_lock.lock();
            serv->add_snake();
            t->mySnake = serv->snakes.back();
            for (auto i = serv->snakes.begin(); i != serv->snakes.end(); i++){
                t->send_snake(*i);
            }
            sock.write_some(buffer(&END, sizeof(u_int)));
            t->send_food(serv->lone_food);
            for (auto i = Snake::colider.foods.begin(); i != Snake::colider.foods.end(); i++){
                t->send_food(*i);
            }
            sock.write_some(buffer(&END, sizeof(u_int)));
            t->send_now();
            obj_lock.unlock();
            talkers_lock.lock();
            serv->write_all(t->mySnake);
            serv->talkers.push_back(t);
            t->start();
            talkers_lock.unlock();
        }
        catch(int){
            delete t;
        }
    }
}

void Server::write_all(PSnake new_snake){
    for (auto it = talkers.begin(); it != talkers.end(); it++){
        (*it)->send_snake(new_snake);
    }
}

void Server::write_all(PFood new_food){
    for (auto it = talkers.begin(); it != talkers.end(); it++){
        (*it)->send_food(new_food);
    }
}

void Server::write_all_dirs(){
    for (auto it = talkers.begin(); it != talkers.end(); it++){
        (*it)->send_all_dirs(this);
    }
}

Server::Server(int height, int width){
    map_rect = Point(width, height);
    for (int i = -1; i <= width; i++){
        Snake::colider.add_object(new Wall(Point(i, -1)));
        Snake::colider.add_object(new Wall(Point(i, height)));
    }
    for (int i = 0; i < height; i++){
        Snake::colider.add_object(new Wall(Point(-1, i)));
        Snake::colider.add_object(new Wall(Point(width, i)));
    }
    add_food();
}

void Server::loop(){
    std::thread accepter_thread(accept_all, this);
    while (true){
        while (next_tick > Clock::now()){
            this_thread::sleep_until(next_tick);
        }
        next_tick += tick_length;
        current_step++;
        obj_lock.lock();
        for (auto it = snakes.begin(); it != snakes.end(); it++){
            (*it)->move();
        }
        if (Snake::colider.foods_size() > 0){
            for (u_int i = 0 ; i < Snake::colider.foods_size(); i++){
                write_all(Snake::colider.foods[i]);
            }
            Snake::colider.clear_food();
        }
        if (!lone_food->is_alive()){
            add_food();
            write_all(lone_food);
        }
        clear_talkers();
        clear_snakes();
        Snake::colider.clear_dead();
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
