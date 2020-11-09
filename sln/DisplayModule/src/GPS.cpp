#include "GPS.hpp"
#include "Modules.hpp"

#ifdef __APPLE__
    #include <OpenGL/gl.h>
#elif defined(WIN32)
    #include <Windows.h>
    #include <GL/gl.h>
#else
    #include <GL/gl.h>
#endif

GPS::GPS() : gps(mod::GPS.name, sm::SIZE) {
    gps.openFileMapping();
    gps.mappedViewAddr();
    update();
}

void GPS::update() {
    numPoints = (uint8_t*)gps.getBaseAddress();
    head = (int*)((char*)gps.getBaseAddress() + 8);
    tail = (int*)((char*)gps.getBaseAddress() + 16);
    data = (double*)((char*)gps.getBaseAddress() + 24);
}

void GPS::draw() {

    int northing = 0;
    int easting = 0;
    int height = ;

    update();
    glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(*(data + sizeof(double) * easting), *(data + sizeof(double) * height), *(data + sizeof(double) * northing));
        for (int i = 1; i < (*numPoints - 1); i++) {
            glVertex3f(points[i].second / 1000, HEIGHT, points[i].first / 1000);
            glVertex3f(points[i + 1].second / 1000, HEIGHT, points[i + 1].first / 1000);
        }
    glEnd();
};
