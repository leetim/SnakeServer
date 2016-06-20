#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <asio.hpp>
#include <gtkmm.h>
#include <snake.h>
#include <Point.h>
#include <string>

using namespace std;
using namespace asio;

class Bot{
public:
    typedef asio::ip::tcp::endpoint endpoint;
    typedef asio::ip::tcp tcp;
    typedef asio::ip::tcp::acceptor acceptor;
    typedef asio::ip::tcp::socket socket;

    Bot():service(), ep(tcp::v4(), 18008), m_socket(service){
        m_socket.connect(ep);
    };

    void read_all(){
        try{
            char mes[4];
            m_socket.read_some(buffer(mes, 4));
            if (string(mes) == "BY"){
                throw 2;
            }
            points_lock.lock();
            m_socket.read_some(buffer(buf, 1602*sizeof(MapPoint)));
            points_lock.unlock();
        }
        catch(...){
            cout << "done" << endl;
            throw 0;
        }
    }

    ~Bot(){
        m_socket.close();
    }

    int loop(){
        try{
            while (true){

            }
        }
        catch(int e){
            return e;
        }
    }

    bool write_dir(int k){
        Point p = Point::dirByCode(k);
        int xy[] = {p.x, p.y};
        // m_socket.write_some(buffer(xy, sizeof(int)));
        m_socket.write_some(buffer(xy, sizeof(xy)));
        return true;
    }

private:
    io_service service;
    endpoint ep;
    socket m_socket;

    std::mutex points_lock;
    MapPoint buf[1602];
    MapPoint& pos = buf[1];
    PMapPoint points = &buf[2];
    unsigned int& length = *(unsigned int*)buf;
};

int main(int argc, char* argv[]){

    return 0;
}
