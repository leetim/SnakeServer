#pragma once

#include <gtkmm.h>
#include <server.h>

class Client;

typedef Client* PClient;
typedef void (Client::*Func)();

struct Functor{
    Functor(Func _f, PClient some): client(some), f(_f){};
    void operator()(){
        (client->*f)();
    };
    PClient client;
    Func f;
};

class Client{
public:
    Client(int height, int width);

    template<class T>
    void loop(T redrawer){
        std::thread listen_thread(Functor(&Client::listen_all, this));
        // std::thread send_thread(Functor(&Client::send_all, this));
        next_tick = Clock::now();
        while (true){
            while (next_tick >= Clock::now()){
                std::this_thread::sleep_until(next_tick);
            }
            next_tick += tick_length;
            current_step++;
            obj_lock.lock();
            for (auto it = snakes.begin(); it != snakes.end(); it++){
                (*it)->move();
            }
            std::cout << "loop" << std::endl;
            auto p = mySnake->get_head_coord();
            std::cout << p.x << " " << p.y << std::endl;
            Snake::colider.clear_dead();
            obj_lock.unlock();
            redrawer();
        }
    }

    template <class T>
    void redraw(const Cairo::RefPtr<Cairo::Context>& cr, T draw_point){
        // obj_lock.lock();
        Point mid = mySnake->get_head_coord();
        Point left_top = mid - map_rect/2;
        Point right_down = mid + map_rect/2;
        printf("(%d, %d)\n", mid.x, mid.y);
        for (int i = 0; i < map_rect.x; i++){
            for (int j = 0; j < map_rect.x; j++){
                draw_point(cr, i, j, '.');
            }
        }
        for (u_int i = 0; i < Snake::colider.size(); i++){
            Point p = Snake::colider[i]->get_coord();
            if (left_top > p || right_down < p){
                continue;
            }
            char c = Snake::colider[i]->get_char();
            Point t = p - left_top;
            draw_point(cr, t.x, t.y, c);
        }
        // obj_lock.unlock();
    }
    void send_all(int k);

private:
    void listen_all();
    int wait_new_dir();
    void read_now();
    void read_snake();
    void read_all_dirs();
    void read_food();
    void read_wall();

    u_llong current_step;
    Point map_rect;
    Talker::socket m_socket;
    PSnake mySnake;
    std::vector<PSnake> snakes;

    TimePoint next_tick;
    MS delta;
    MS tick_length;

    std::recursive_mutex obj_lock;
    std::recursive_mutex out_lock;
};
