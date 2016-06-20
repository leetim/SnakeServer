#include <iostream>
#include <server.h>

using namespace std;

int main(int argc, char* argv[]){
    srand(time(0));
    Server server(30, 30);
    server.loop();
	return 0;
}
