#include <iostream>
#include <client.h>
#define sq_width 10
using namespace std;

class MyArea: public Gtk::DrawingArea{
public:
    MyArea(){};
    ~MyArea(){};

    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr){  // This is where we draw on the window
        cout << "draw" << endl;
        // Gtk::Allocation allocation = get_allocation();
        // const int width = allocation.get_width();
        // const int height = allocation.get_height();

        // coordinates for the center of the window
        // int xc = width / 20;
        // int yc = height / 20;

        cr->set_line_width(1);  // outline thickness changes                              // with window size
        // client->redraw(cr, sigc::mem_fun(*this, &MyArea::draw_rect2));

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
            rgb[0] = 0.9; rgb[1] = 0.9; rgb[2] = 0.9;
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

};

class GameForm: public Gtk::Window{
public:
    GameForm(): myArea(){
        set_default_size(500, 500);
        set_border_width(0);
        signal_key_release_event().connect(sigc::mem_fun(*this, &GameForm::on_key_press));
        add(myArea);
        myArea.show();
        loop_thread = std::thread(sigc::mem_fun(*this, &GameForm::loop));
    };

    void loop(){

    }
    ~GameForm(){};

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
        }
        myArea.force_redraw();
        return true;
    }
private:
    MyArea myArea;
    std::thread loop_thread;

};


int main(int argc, char* argv[]){
    auto app =
    Gtk::Application::create(argc, argv,
      "org.gtkmm.examples.base");

    GameForm window;

    return app->run(window);
}
