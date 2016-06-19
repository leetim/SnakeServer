#include <iostream>
#include <client.h>
#include <server.h>
#include <ncurses.h>

using namespace std;

int main(int argc, char* argv[]){
    srand(time(0));
    Server c(40, 40);
    cout << "lol2" << endl;
    c.loop();
	return 0;
}
