#include <iostream>
#include <asio.hpp>
#include <vector>
#include <mutex>
#include <thread>

using namespace std;
using namespace asio;

struct Messege{
    Messege(): number(), text(){};
    Messege(const Messege& m): number(m.number), text(m.text){};
    Messege(const string& str, const long long& num): number(num), text(str){};
    long long number;
    string text;
};

mutex out_lock;

class Talker{
public:
    typedef Talker* PTalker;
    typedef ip::tcp::endpoint endpoint;
    typedef ip::tcp tcp;
    typedef ip::tcp::acceptor acceptor;
    typedef ip::tcp::socket socket;

    Talker(): m_socket(service), last_messege(), ready(false),
        send_thread(sender, this), read_thread(reader, this){
            send_thread.detach();
            read_thread.detach();
        };

    ~Talker(){
        m_socket.shutdown(socket::shutdown_receive);
        m_socket.close();
    }

    static void close_all(){
        for (std::size_t i = 0; i < talkers.size(); i++){
            delete talkers[i];
        }
    }

    static void accept_all(){
        acceptor acc(service, endpoint(tcp::v4(), 18008));
        while (true){
            PTalker t = new Talker;
            acc.accept(t->m_socket);
            talkers_lock.lock();
            out_lock.lock();
            cout << "new user connected" << endl;
            out_lock.unlock();
            talkers.push_back(t);
            talkers_lock.unlock();
            t->ready = true;
        }
    }

    static void sender(PTalker t){
        while (!t->ready){}
        while (t->m_socket.is_open()){
            messeges_lock.lock();
            for (std::vector<Messege>::iterator i = messeges.begin();
            i != messeges.end(); i++){
                if (i->number <= t->last_messege){
                    continue;
                }
                t->m_socket.write_some(buffer(i->text.c_str(), i->text.size()));
                t->last_messege = i->number;
            }
            messeges_lock.unlock();
        }
    };

    static void reader(PTalker t){
        while (!t->ready){}
        while (t->m_socket.is_open()){
            if (t->m_socket.available()<=0){
                continue;
            }
            out_lock.lock();
            cout << "new messege" << endl;
            string text;
            while (t->m_socket.available()){
                char buf[1024];
                t->m_socket.read_some(buffer(buf, 1024));
                text = string(buf);
            }
            messeges_lock.lock();
            cout << text << endl;
            // Messege b = messeges.back();
            // long long next_num = messeges.back().number + 1;
            messeges.push_back(Messege(text, max_number + 1));
            max_number++;
            cout << "done" << endl;
            messeges_lock.unlock();
            out_lock.unlock();

        }
    }

private:
    static io_service service;
    static vector<PTalker> talkers;
    static vector<Messege> messeges;
    static long long max_number;
    static mutex talkers_lock;
    static mutex messeges_lock;

    socket m_socket;
    long long last_messege;
    bool ready;
    std::thread send_thread;
    std::thread read_thread;
};


io_service Talker::service;
vector<Talker::PTalker> Talker::talkers;
vector<Messege> Talker::messeges;
mutex Talker::talkers_lock;
mutex Talker::messeges_lock;
long long Talker::max_number = 0;

int main(){
    // std::thread accepter_thread(Talker::accept_all);
    // accepter_thread.join();
    Talker::accept_all();
    while(true){

    }
    // Talker::close_all();
    return 0;
}
