#include "Timer.h"
#include <iostream>
#include <thread>

void a() {
    std::cout << "A\n";
}

void b() {
    std::cout << "B\n";
}

int main() {
    Timer t1(1, a);
    Timer t2(2, b);

    t1.start();
    t2.start();

    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}
