#pragma once

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <thread>
#include <asio.hpp>
#include <mutex>
#include <chrono>
#include <queue>
#include <snake.h>
#include <ncurses.h>

class Talker;
class Server;

typedef Talker* PTalker;
typedef Server* PServer;
typedef void (*SendFunction)(PTalker, PServer);
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<Clock> TimePoint;
typedef std::chrono::milliseconds MS;

class Talker{
public:
    friend class Server;

    typedef asio::ip::tcp::endpoint endpoint;
    typedef asio::ip::tcp tcp;
    typedef asio::ip::tcp::acceptor acceptor;
    typedef asio::ip::tcp::socket socket;

    static asio::io_service service;
    static endpoint ep;

    Talker(SendFunction read_all, PServer server);
    ~Talker();
    bool is_closed();
    void send_all();
    void read_dir();
    void start();

private:
    bool ready;
    PSnake mySnake;
    socket m_socket;
    std::thread read_thread;
};

class Server{
public:
    friend class Talker;

    static void read_all(PTalker t, PServer serv);
    static void accept_all(PServer serv);

    Server(int height, int weidth);
    void loop();
    void write_all();
    void add_snake();
    void add_food();

    static std::mutex obj_lock;
    static std::mutex talkers_lock;
    static std::mutex out_lock;

    static MS tick_length;
    static TimePoint next_tick;

private:
    void clear_talkers();
    void clear_snakes();
    Point get_clear_point(int max_x, int max_y);

    std::vector<PTalker> talkers;
    std::vector<PSnake> snakes;
    PFood lone_food;
    u_llong current_step;
    Point map_rect;


};
