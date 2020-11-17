#ifndef UTIL_HPP_
#define UTIL_HPP_

constexpr double eps = 1e12 * std::numeric_limits<double>::epsilon();

static bool d_cmp(double a, double b, double eps) {
    return std::abs(a - b) <= eps ||
           std::abs(a - b) < (std::fmax(std::abs(a), std::abs(b)) * eps);
}

#endif  // UTIL_HPP_