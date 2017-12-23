#include <iostream>
#include "TimeMeasure.h"
#include <chrono>
#include <thread>

int main() {
    std::cout << "Hello, World!" << std::endl;
    TimeMeasure tm(true);
    std::string s;
    unsigned n = 0;
    do {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1000000000));
        std::cout << tm.Duration().count() / 1'000'000'000.0 << " ("
                  << tm.TotalDuration().count() / 1'000'000'000.0 << ")"
                  << std::endl;
    } while(++n < 10);
    return 0;
}