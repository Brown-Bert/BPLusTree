#include <iostream>
#include <vector>
#include "ThreadPool.h"
#include <mutex>
#include <unistd.h>
#include <random>
#include <string>

using namespace std;

mutex m;
template<class T>
void speak(T i){
    {
        // unique_lock<mutex> lock(m);
        cout << i << " = 123" << endl;
        // sleep(10);
    }
    this_thread::sleep_for(chrono::seconds(3));
}

int main(int argc, char** argv){
    int a,b;
    a = 1;
    b = 0;
    try
    {
        // a / b;
        cout << 3 << endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        b = 1;
    }
    
    
    return 0;
}