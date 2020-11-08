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
    northing = (double*)((char*)gps.getBaseAddress());
    easting = (double*)((char*)gps.getBaseAddress() + 8);
    height = (double*)((char*)gps.getBaseAddress() + 16);
}

void GPS::draw() {
    
    glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(1, 0, 0);
    glEnd();
};
