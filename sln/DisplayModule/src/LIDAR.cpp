#include "LIDAR.hpp"
#include "Modules.hpp"

#ifdef __APPLE__
    #include <OpenGL/gl.h>
#elif defined(WIN32)
    #include <Windows.h>
    #include <GL/gl.h>
#else
    #include <GL/gl.h>
#endif

constexpr double eps = 1e12 * std::numeric_limits<double>::epsilon();

static bool d_cmp(double a, double b, double eps) {
    return std::abs(a - b) <= eps ||
        std::abs(a - b) < (std::fmax(std::abs(a), std::abs(b)) * eps);
}

LIDAR::LIDAR() : laser(mod::LASER.name, sm::SIZE) {
    laser.openFileMapping();
    laser.mappedViewAddr();
    update();
}

void LIDAR::update() {
    numPoints = (uint16_t*)((char*)laser.getBaseAddress());
    points = (std::pair<double, double>*)((char*)laser.getBaseAddress() + 16);
}

void LIDAR::draw() {
    update();
    glBegin(GL_LINES);
        for (int i = 0; i < *numPoints; i++) {
            if (d_cmp(points[i].second, 0.0, eps) == false && d_cmp(points[i].first, 0.0, eps) == false) {
                glVertex3f(points[i].second / 1000, 0, points[i].first / 1000);
                glVertex3f(points[i].second / 1000, 1, points[i].first / 1000);
            }
        }
    glEnd();
};
