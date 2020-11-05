#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <chrono>

namespace util {
static std::chrono::milliseconds timeDiff(std::chrono::steady_clock::time_point later, std::chrono::steady_clock::time_point before) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(later - before);
}
}  // namespace util

#endif  // UTIL_HPP_
