#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <chrono>

using namespace std::chrono_literals;

namespace tm {
constexpr auto TIMEOUT_1S = 1s;
constexpr auto TIMEOUT_2S = 2s;
constexpr auto TIMEOUT_4S = 4s;

class Timer {
   public:
    Timer(std::chrono::steady_clock::time_point ref, std::chrono::milliseconds exp) : ref(ref), exp(exp) {}
    ~Timer() {}

    void time() { ref = std::chrono::steady_clock::now(); }

    bool expired() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::steady_clock::now() - ref) > exp;
    }

   private:
    std::chrono::steady_clock::time_point ref;  // Time of reference
    std::chrono::milliseconds exp;              // Expiry duration
};
}  // namespace tm

#endif  // TIMER_HPP_
