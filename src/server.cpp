#include <server.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace asio;

////////////////////////////////////////////////////////////////////////////////
//Talker

Talker::endpoint Talker::ep(Talker::tcp::v4(), 18008);
io_service Talker::service;

Talker::Talker():  ready(false), m_socket(service){
    closed_flag = false;
}

Talker::~Talker(){
    try{
        m_socket.write_some(buffer("BY"));
    }
    catch(...){

    }
    m_socket.close();
    mySnake->kill();
    delete mySnake;
}

bool Talker::is_closed(){
    return closed_flag;
}

void Talker::close(){
    closed_flag = true;
}


void Talker::read_dir(){
    try{
        if (!m_socket.available()){
            return;
        }
        int xy[2];
        m_socket.read_some(buffer(xy, sizeof(xy)));;
        cout << "done: " << xy[0] << " " << xy[1] << endl;
        Point dir(xy[0], xy[1]);
        mySnake->change_dir(dir);
    }
    catch(...){
        cout << "read error" << endl;
        close();
        mySnake->kill();
    }
}

void Talker::send_all(MapPoint* buf, u_int length){
    if (!ready){
        return;
    }
    try{
        char mes[4] = "OK";
        m_socket.write_some(buffer(mes, 4));
        buf[1].x = mySnake->get_head_coord().x;
        buf[1].y = mySnake->get_head_coord().y;
        m_socket.write_some(buffer(buf, length*sizeof(MapPoint)));
    }
    catch(...){
        cout << "sended error" << endl;
        close();
        mySnake->kill();
    }
}

void Talker::start(){
    ready = true;
}

////////////////////////////////////////////////////////////////////////////////
//Server

mutex Server::obj_lock;
mutex Server::talkers_lock;
mutex Server::out_lock;

MS Server::tick_length = MS(100);
TimePoint Server::next_tick = TimePoint(Clock::now() + tick_length);

void Server::read_all(PServer serv){
    while (true){
        obj_lock.lock();
        talkers_lock.lock();
        for (u_int i = 0; i < serv->talkers.size(); i++){
            PTalker t = serv->talkers[i];
            if (!t->is_closed()){
                t->read_dir();
            }
        }
        // cout << "readed done" << endl;
        talkers_lock.unlock();
        obj_lock.unlock();
        this_thread::sleep_for(tick_length);
    }
}

void Server::accept_all(PServer serv){
    Talker::acceptor acc(Talker::service, Talker::ep);
    while (true){
        try{
            if (serv->talkers.size() >= 5){
                this_thread::sleep_for(tick_length);
                continue;
            }
            PTalker t = new Talker();
            acc.accept(t->m_socket);
            obj_lock.lock();
            talkers_lock.lock();

            cout << "Client connect..." << endl;
            t->m_socket.set_option(asio::ip::tcp::socket::reuse_address(true));

            serv->add_snake();
            t->mySnake = serv->snakes.back();
            t->start();
            serv->talkers.push_back(t);
            cout << "Client connected" << endl;

            talkers_lock.unlock();
            obj_lock.unlock();
        }
        catch(...){

        }
    }
}

void Server::write_all(){
    MapPoint buf[1602];
    u_int* length = (u_int*)buf;
    MapPoint* points = &buf[2];

    *length = Snake::colider.size();
    for (u_int i = 0; i < Snake::colider.size(); i++){
        PObject o = Snake::colider[i];
        points[i].x = o->get_coord().x;
        points[i].y = o->get_coord().y;
        points[i].c = o->get_char();
    }
    for (auto it = talkers.begin(); it != talkers.end(); it++){
        (*it)->send_all(buf, *length+2);
    }
}


Server::Server(int height, int width): current_step(){
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
    std::thread reader_thread(read_all, this);
    accepter_thread.detach();
    reader_thread.detach();
    next_tick = Clock::now();
    cout << "Server start" << endl;
    while (true){
        while (next_tick >= Clock::now()){
            this_thread::sleep_until(next_tick);
        }
        next_tick += tick_length;
        current_step++;

        obj_lock.lock();
        talkers_lock.lock();

        for (auto it = snakes.begin(); it != snakes.end(); it++){
            (*it)->move();
            cout << 1;
            Point p = (*it)->get_head_coord();
            cout << 1 << endl;
            cout << p.x << " " << p.y << endl;
        }
        for (auto it = snakes.begin(); it != snakes.end(); it++){
            if (!(**it)[0]->is_alive() && !(*it)->is_alive()){
                cout << 1 << endl;
                (*it)->kill();
            }
        }
        if (!lone_food->is_alive()){
            add_food();
        }
        cout << current_step << " step " << talkers.size() << " " << snakes.size() << endl;
        clear_snakes();
        clear_talkers();
        Snake::colider.clear_dead();
        write_all();

        talkers_lock.unlock();
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
            // delete *it;
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
