#include <client.h>

using namespace std;
using namespace asio;

Client::Client(int height, int width): m_socket(Talker::service){
    map_rect = Point(width, height);
    tick_length = Server::tick_length;
    next_tick = TimePoint(Clock::now() + tick_length);
    m_socket.connect(Talker::ep);
    u_int command = 999999;
    m_socket.read_some(buffer(&command, sizeof(u_int)));
    while (command != Server::END){
        read_snake();
        m_socket.read_some(buffer(&command, sizeof(u_int)));
        cout << "read_snake" << endl;
    }
    mySnake = snakes.back();
    m_socket.read_some(buffer(&command, sizeof(u_int)));
    while (command != Server::END){
        read_food();
        m_socket.read_some(buffer(&command, sizeof(u_int)));
    }
    cout << "read_food" << endl;
    m_socket.read_some(buffer(&command, sizeof(u_int)));
    while (command != Server::END){
        read_wall();
        m_socket.read_some(buffer(&command, sizeof(u_int)));
    }
    read_now();
    cout << Snake::colider.size() << endl;
}

// void Client::loop(){
//     std::thread listen_thread(Functor(&Client::listen_all, this));
//     // std::thread send_thread(Functor(&Client::send_all, this));
//     next_tick = Clock::now();
//     while (true){
//         while (next_tick >= Clock::now()){
//             this_thread::sleep_until(next_tick);
//         }
//         next_tick += tick_length;
//         current_step++;
//         obj_lock.lock();
//         for (auto it = snakes.begin(); it != snakes.end(); it++){
//             (*it)->move();
//         }
//         cout << "loop" << endl;
//         auto p = mySnake->get_head_coord();
//         cout << p.x << " " << p.y << endl;
//         // redraw();
//         Snake::colider.clear_dead();
//         obj_lock.unlock();
//     }
// }

// template <class T>
// void Client::redraw(const Cairo::RefPtr<Cairo::Context>& cr, T draw_point){
//     obj_lock.lock();
//     Point mid = mySnake->get_head_coord();
//     Point left_top = mid - map_rect/2;
//     Point right_down = mid + map_rect/2;
//     printf("(%d, %d)", mid.x, mid.y);
//     for (int i = 0; i < map_rect.x; i++){
//         for (int j = 0; j < map_rect.x; j++){
//             draw_point(cr, i, j, '.');
//         }
//     }
//     for (u_int i = 0; i < Snake::colider.size(); i++){
//         Point p = Snake::colider[i]->get_coord();
//         if (left_top > p || right_down < p){
//             continue;
//         }
//         char c = Snake::colider[i]->get_char();
//         Point t = p - left_top;
//         draw_point(cr, t.x, t.y, c);
//     }
//     obj_lock.unlock();
// }

void Client::send_all(int k){
    if (k < 0 && k > 3){
        return;
    }
    out_lock.lock();
    Point new_dir = Point::dirByCode(k);
    int xy[] = {new_dir.x, new_dir.y};
    m_socket.write_some(buffer(&Server::CHANGE_DIR, sizeof(u_int)));
    m_socket.write_some(buffer(xy, 2*sizeof(int)));
    cout << "send_all" << endl;
    // if (xy[0] > 1){
    //     throw 1;
    // }
    out_lock.unlock();
}

void Client::listen_all(){
    // cout << "listen_all" << endl;
    while (m_socket.is_open()){
        u_int command;
        m_socket.read_some(buffer(&command, sizeof(u_int)));
        obj_lock.lock();
        if (command == Server::SEND_SNAKE){
            read_snake();
        }
        if (command == Server::SEND_FOOD){
            read_food();
        }
        if (command == Server::SEND_ALL_DIRS){
            read_all_dirs();
        }
        if (command == Server::SEND_NOW){
            read_now();
        }
        obj_lock.unlock();
    }
}

int Client::wait_new_dir(){
    int k = getch();
    if (k == 'a' || k == 'A'){
        return 0;
    }
    if (k == 'w' || k == 'W'){
        return 1;
    }
    if (k == 'd' || k == 'D'){
        return 2;
    }
    if (k == 's' || k == 'S'){
        return 3;
    }
    return -1;
}

void Client::read_now(){
    u_int count;
    m_socket.read_some(buffer(&count, sizeof(u_int)));
    TimePoint t(MS(count));
    // TimePoint d = TimePoint(t - Clock::now());
}

void Client::read_snake(){
    u_int length;
    cout << length << endl;
    m_socket.read_some(buffer(&length, sizeof(u_int)));
    cout << length << endl;
    int* buf = new int[length];
    m_socket.read_some(buffer(buf, length*sizeof(int)));
    vector<Point> vect;
    vect.reserve(length / 2);
    Point dir(buf[0], buf[1]);
    int* buf2 = buf + 2;
    for (u_int i = 0; i < length/2 - 1; i++){
        vect.emplace_back(buf2[2*i], buf2[2*i+1]);
    }
    PSnake s = new Snake(move(vect), dir, current_step);
    snakes.push_back(s);
    cout << snakes.size() << endl;
    delete[] buf;
}

void Client::read_all_dirs(){
    obj_lock.lock();
    u_int scount;
    m_socket.read_some(buffer(&scount, sizeof(u_int)));
    int* buf = new int[scount*2];
    m_socket.read_some(buffer(buf, 2*scount*sizeof(int)));
    vector<Point> dirs;
    dirs.reserve(scount);
    for (u_int i = 0; i < scount; i++){
        snakes[i]->change_dir(Point(buf[2*i], buf[2*i + 1]));
    }
    delete[] buf;
    cout << "read_all_dirs" << endl;
    obj_lock.unlock();
}

void Client::read_food(){
    int xy[2];
    m_socket.read_some(buffer(xy, 2*sizeof(int)));
    Snake::colider.add_object(new Food(Point(xy[0], xy[1])));
    printf("food: (%d, %d)", xy[0], xy[1]);
}

void Client::read_wall(){
    int xy[2];
    m_socket.read_some(buffer(xy, 2*sizeof(int)));
    Snake::colider.add_object(new Wall(Point(xy[0], xy[1])));
}
