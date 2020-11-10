#ifndef OEM4_HPP_
#define OEM4_HPP_

#include <cstdint>

struct OEM4 {
    double northing;
    double easting;
    double height;
    uint32_t crc;
};

#endif // OEM4_HPP_
