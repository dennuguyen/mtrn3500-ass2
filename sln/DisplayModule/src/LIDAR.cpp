#include "LIDAR.hpp"

#include "Modules.hpp"
#include "Util.hpp"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#elif defined(WIN32)
#include <GL/gl.h>
#include <Windows.h>
#else
#include <GL/gl.h>
#endif

LIDAR::LIDAR() : laser(mod::LASER.name, sm::SIZE) {
    laser.openFileMapping();
    laser.mappedViewAddr();
    numPoints = (uint16_t*)((char*)laser.getBaseAddress());
    points = (std::pair<double, double>*)((char*)laser.getBaseAddress() + 16);
}

void LIDAR::draw() {
    glTranslatef(0.5, 0, 0);
    glBegin(GL_LINES);
    for (int i = 0; i < *numPoints; i++) {
        if (d_cmp(points[i].second, 0.0, eps) == false && d_cmp(points[i].first, 0.0, eps) == false) {
            glVertex3f(points[i].second / 1000, 0, points[i].first / 1000);
            glVertex3f(points[i].second / 1000, 1, points[i].first / 1000);
        }
    }
    glEnd();
};
