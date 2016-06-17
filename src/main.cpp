#include <iostream>
#include <colider.h>
using namespace std;

int main(int argc, char* argv[]){
    auto t1 = State(4);
    auto t2 = State(5);
    cout << (t1 < t2) << endl;
    return 0;
}
