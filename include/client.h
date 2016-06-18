#pragma once

#include <server.h>

class Client;

typedef Client* PCLient;

class Client{
public:
    Client(int height, int width);
    void loop();

private:
    void redraw();
    void send_all();
    void listen_all();
    void read_now();
    void read_snake();
    void read_all_dirs();
    void read_food();
    Point map_rect;
    Point left_top;
    Talker::socket m_socket;
    PSnake mySnake;
    std::vector<PSnake> snakes;
    u_llong current_step;
};
