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
            glVertex3f(points[i].second / 1000, 0, points[i].first / 1000);
            glVertex3f(points[i].second / 1000, HEIGHT, points[i].first / 1000);
        }
    glEnd();
};
