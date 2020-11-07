#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <cassert>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

namespace tmr {
constexpr auto TIMEOUT_1S = 1s;
constexpr auto TIMEOUT_2S = 2s;
constexpr auto TIMEOUT_3S = 3s;
constexpr auto TIMEOUT_4S = 4s;
constexpr auto TIMEOUT_5S = 5s;
constexpr auto TIMEOUT_10S = 10s;
constexpr auto TIMEOUT_30S = 10s;

class Timer {
   public:
    Timer() : ref(), exp() {}
    ~Timer() {}

    void time(std::chrono::milliseconds expiry) {
        ref = std::chrono::system_clock::now();
        exp = expiry;
    }

    bool expired() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now() - ref) > exp;
    }

    void print() {
        std::cout << "Time: " << std::chrono::system_clock::to_time_t(ref) << std::endl;
        std::cout << "Expiry: " << exp.count() << "s" << std::endl;
    }

   private:
    std::chrono::system_clock::time_point ref;  // Time of reference
    std::chrono::milliseconds exp;              // Expiry duration
};
}  // namespace tmr

#endif  // TIMER_HPP_
