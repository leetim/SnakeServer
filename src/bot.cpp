// #define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <asio.hpp>
#include <snake.h>
#include <Point.h>
#include <string>
#include <cmath>

using namespace std;
using namespace asio;

int get_max(float* fs, int len){
    int mi = 0;
    for (int i = 1; i < len; i++){
        if (fs[mi] < fs[i]){
            mi = i;
        }
    }
    return mi;
}

class Bot{
public:
    typedef asio::ip::tcp::endpoint endpoint;
    typedef asio::ip::tcp tcp;
    typedef asio::ip::tcp::acceptor acceptor;
    typedef asio::ip::tcp::socket socket;

    Bot():service(), ep(tcp::v4(), 18008), m_socket(service), dir_code(-1){};


    void read_all(){
        try{
            char mes[4];
            m_socket.read_some(buffer(mes, 4));
            if (string(mes) == "BY"){
                throw 2;
            }
            int k = 0;
            // points_lock.lock();
            m_socket.read_some(buffer(buf, 1602*sizeof(MapPoint)));
            for (unsigned int i = 0; i < length; i++){
                if (points[i].c == '%'){
                    k++;
                }
            }
            printf("Food count: %d\n", k);
        }
        catch(...){
            cout << "done" << endl;
            throw 0;
        }
    }

    ~Bot(){
        m_socket.close();
    }

    float get_risk(const MapPoint& other){
        float len2 = Point(pos.x - other.x, pos.y - other.y).len2();
        const char& c = other.c;
        switch (c)
        {
        case '&':
            return 1600/len2;
            break;
        case 'o':
            return 0;
            break;
        case '%':
            return 1600;
            break;
        case '#':
            return -1600/len2;
            break;
        default:
            return 0.0f;
            break;
        }
    }

    int loop(){
        m_socket.connect(ep);
        try{
            read_all();
            Point cur_pos = Point(pos.x, pos.y);
            read_all();
            Point new_pos = Point(pos.x, pos.y);
            dir_code = Point::codeByDir(new_pos - cur_pos);
            while (true){
                read_all();
                MapPoint& pos = buf[1];
                PMapPoint points = &buf[2];
                unsigned int& length = *(unsigned int*)buf;

                float risk_r = 0.0;
                float risk_f = 0.0;
                float risk_l = 0.0;
                int kr = 0, kf = 0, kl = 0;

                Point dir = Point::dirByCode(dir_code);
                float dir_a = atan2(dir.y, dir.x);
                float min_a = -M_PI/2;
                float max_a = M_PI/2;
                float lf = M_PI/6;
                float rf = -M_PI/6;
                for (unsigned int i = 0; i < length; i++){
                    if (points[i].c == '%'){
                        cout << "Food" << endl;
                    }
                    if (points[i].y == pos.y && points[i].x == pos.x){
                        break;
                    }
                    float angle = atan2(points[i].y - pos.y, points[i].x - pos.x) - dir_a;
                    if (angle < -M_PI){
                        angle += 2*M_PI;
                    }
                    if (angle > M_PI){
                        angle -= 2*M_PI;
                    }
                    if (min_a <= angle && angle <= rf){ risk_r += get_risk(points[i]); kr++;}
                    if (rf <= angle && angle <= lf){ risk_f += get_risk(points[i]); kf++;}
                    if (lf <= angle && angle <= max_a){ risk_l += get_risk(points[i]); kl++;}
                }
                printf("%f %f %f %f %f\n", min_a, rf, dir_a, lf, max_a);
                printf("%d %d %d\n", kr, kf, kl);
                float mat[] = {risk_r, risk_f, risk_l};
                int new_dir_code = (dir_code + (get_max(mat, 3) - 1));
                if (new_dir_code < 0){
                    new_dir_code += 4;
                }
                if (new_dir_code > 3){
                    new_dir_code -= 4;
                }
                printf("%f %f %f\n", risk_r, risk_f, risk_l);
                printf("%d %d\n\n", get_max(mat, 3) - 1, dir_code);
                if (dir_code != new_dir_code){
                    write_dir(new_dir_code);
                    dir_code = new_dir_code;
                }
            }
        }
        catch(int e){
            return e;
        }
    }

    bool write_dir(int k){
        cout << "lol" << endl;
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

    int dir_code;
};

int main(int argc, char* argv[]){
    while (true){
        Bot bot;
        bot.loop();
    }
    // cout << atan2( 1, 0) << endl;
    // cout << atan2(-1, 0) << endl;
    return 0;
}
