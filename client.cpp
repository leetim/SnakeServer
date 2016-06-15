#include <iostream>
#include <asio.hpp>
#include <thread>
#include <mutex>
#include <queue>
#include <string>

using namespace std;
using namespace asio;

queue<string> outq;
mutex out_lock;
io_service service;
ip::tcp::socket sock(service);
ip::tcp::endpoint ep(ip::tcp::v4(), 18008);

void read_t(){
    while (true){
        string text;
        while (sock.available() > 0){
            // cout << sock.available() << endl;
            char buf[1024];
            sock.read_some(buffer(buf, 1024));
            text += buf;
            cout << text << endl;
        }
    }
}

void write_t(){
    while (true){
        out_lock.lock();
        while (!outq.empty()){
            sock.write_some(buffer(outq.front().c_str(), outq.front().size()+1));
            outq.pop();
        }
        out_lock.unlock();
    }
}

int main(){
    sock.connect(ep);
    std::thread read_thread(read_t);
    std::thread write_thread(write_t);
    string buf;
    while (true){
        getline(cin, buf);
        out_lock.lock();
        outq.push(buf);
        out_lock.unlock();
    }
    return 0;
}
