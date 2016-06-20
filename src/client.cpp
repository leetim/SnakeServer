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

#define sq_width 10
using namespace std;
using namespace asio;

////////////////////////////////////////////////////////////////////////////////
//MyArea

class MyArea: public Gtk::DrawingArea{
public:
    MyArea(){};
    ~MyArea(){};

    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr){  // This is where we draw on the window
        // cout << "draw" << endl;
        Gtk::Allocation allocation = get_allocation();
        const int width = allocation.get_width();
        const int height = allocation.get_height();

        // coordinates for the center of the window
        int xc = width / sq_width;
        int yc = height / sq_width;
        cr->set_line_width(1);  // outline thickness changes
        printf("%d %d\n", xc, yc);
        for (int x = 0; x < xc; x++){
            cr->move_to(x*sq_width, 0);
            cr->line_to(x*sq_width, height);
        }
        for (int y = 0; y < yc; y++){
            cr->move_to(0, y*sq_width);
            cr->line_to(width, y*sq_width);
        }

        points_lock.lock();
        Point per(pos.x - xc/2, pos.y - yc/2);
        for (unsigned int i = 0; i < length; i++){
            MapPoint p = points[i];
            draw_rect2(cr, p.x - per.x, p.y - per.y, p.c);
        }
        points_lock.unlock();
        cout << "redraw " << length << endl;
        return true;
    }

    void draw_rect(const Cairo::RefPtr<Cairo::Context>& cr,
    int x, int y, float r, float g, float b){
        cr->save();
        cr->rectangle(x*sq_width, y*sq_width, sq_width, sq_width);
        cr->close_path();   // line back to start point
        cr->set_source_rgb(r, g, b);
        cr->fill_preserve();
        cr->restore();  // back to opaque black
        cr->stroke();   // outline it
    }

    void draw_rect2(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, char c){
        // cout << "rect2" << endl;
        float rgb[3];
        if (c == '.'){
            rgb[0] = 0.3; rgb[1] = 0.3; rgb[2] = 0.3;
        }
        if (c == '&'){
            rgb[0] = 1.0; rgb[1] = 1.0; rgb[2] = 0.0;
        }
        if (c == 'o'){
            rgb[0] = 1.0; rgb[1] = 0.0; rgb[2] = 0.0;
        }
        if (c == '%'){
            rgb[0] = 0.0; rgb[1] = 1.0; rgb[2] = 0.0;
        }
        if (c == '#'){
            rgb[0] = 0.1; rgb[1] = 0.1; rgb[2] = 0.1;
        }
        draw_rect(cr, x, y, rgb[0], rgb[1], rgb[2]);
    }

    void force_redraw(){
        Glib::RefPtr<Gdk::Window> win = get_window();
        if (win){
            Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
            win->invalidate_rect(r, false);
        }
    }
    std::mutex points_lock;
    MapPoint buf[1602];
    MapPoint& pos = buf[1];
    PMapPoint points = &buf[2];
    unsigned int& length = *(unsigned int*)buf;

};

////////////////////////////////////////////////////////////////////////////////
//GameForm

class GameForm: public Gtk::Window{
public:
    typedef asio::ip::tcp::endpoint endpoint;
    typedef asio::ip::tcp tcp;
    typedef asio::ip::tcp::acceptor acceptor;
    typedef asio::ip::tcp::socket socket;

    GameForm(): myArea(), service(), ep(tcp::v4(), 18008), m_socket(service){
        set_default_size(500, 500);
        set_border_width(0);
        signal_key_release_event().connect(sigc::mem_fun(*this, &GameForm::on_key_press));
        add(myArea);
        m_socket.connect(ep);
        loop_thread = std::thread(sigc::mem_fun(*this, &GameForm::read_all));
        loop_thread.detach();
        myArea.show();
    };

    void read_all(){
        try{
            while (true){
                char mes[4];
                m_socket.read_some(buffer(mes, 4));
                if (string(mes) == "BY"){
                    cout << "BY" << endl;
                    throw 2;
                }

                myArea.points_lock.lock();
                m_socket.read_some(buffer(myArea.buf, 1602*sizeof(MapPoint)));
                myArea.points_lock.unlock();

                myArea.force_redraw();
            }
        }
        catch(...){
            cout << "done" << endl;
            this->close();
        }
    }

    ~GameForm(){
        m_socket.close();
    }

    bool on_key_press(GdkEventKey* key){
        char c = (char)key->keyval;
        int k = -1;
        switch (c){
        case 'a':
            k = 0;
            break;
        case 'w':
            k = 1;
            break;
        case 'd':
            k = 2;
            break;
        case 's':
            k = 3;
            break;
        default:
            return true;
        }
        Point p = Point::dirByCode(k);
        int xy[] = {p.x, p.y};
        m_socket.write_some(buffer(xy, sizeof(xy)));
        return true;
    }
private:
    MyArea myArea;
    io_service service;
    endpoint ep;
    socket m_socket;
    std::thread loop_thread;
};


int main(int argc, char* argv[]){
    auto app =
    Gtk::Application::create(argc, argv,
      "SnakeGame");

    GameForm window;
    // Gtk::Window window;
    return app->run(window);
}
