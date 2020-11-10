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
    data = (OEM4*)((char*)gps.getBaseAddress() + 24);
}

void GPS::draw(double x, double y) {

    update();

    glTranslated(0, 0, 0);
    glBegin(GL_LINES);
        glColor3f(0.0, 1.0, 0.0);
        //glVertex3f(0, 0, 0);
        for (int i = *tail, j = 0; j < *numPoints; i++, j++) {
            glVertex3f(data[i % *numPoints].easting / 10 - y,
                       data[i % *numPoints].height / 1000,
                       data[i % *numPoints].northing / 10 - x);
            
            std::cout << "(N: " << (data + (i % *numPoints))->northing << ", E: " << (data + (i % *numPoints))->easting << ", H: " << (data + (i % *numPoints))->height << ")";
            std::cout << " + " << (data + (i % *numPoints))->crc << std::endl;
        }
    glEnd();
};
