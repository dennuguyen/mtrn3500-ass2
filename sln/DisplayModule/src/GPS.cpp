#include "GPS.hpp"
#include "Modules.hpp"
#include "Util.hpp"

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
    head = (int8_t*)((char*)gps.getBaseAddress() + 8);
    tail = (int8_t*)((char*)gps.getBaseAddress() + 16);
    data = (OEM4*)((char*)gps.getBaseAddress() + 24);
}

void GPS::draw(double x, double y) {
    update();
    glBegin(GL_LINES);
        glColor3f(0.0, 1.0, 0.0);
        for (int i = *tail, j = 0; j < *numPoints; i++, j++) {
            if (d_cmp(data[i % *numPoints].easting, x, eps) == false && d_cmp(data[i % *numPoints].northing, y, eps) == false) {
                glVertex3f(data[i % *numPoints].easting / 10 + x,
                    data[i % *numPoints].height / 1000,
                    data[i % *numPoints].northing / 10 + y);
            }
        }
    glEnd();
};
