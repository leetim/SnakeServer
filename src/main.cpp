#include <iostream>
#include <client.h>
#include <server.h>
#include <ncurses.h>

using namespace std;

int main(int argc, char* argv[]){
    srand(time(0));
    Server server(40, 40);
    server.loop();
	return 0;
}
