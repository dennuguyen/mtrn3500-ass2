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
    numPoints = (uint16_t*)((char*)laser.getBaseAddress());
    points = (std::pair<double, double>*)((char*)laser.getBaseAddress() + 16);
}

void LIDAR::draw() {
    
    glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(1, 0, 0);
    glEnd();
};
