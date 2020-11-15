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

GPS::GPS() : gps(mod::GPS.name, sm::SIZE), numPoints(30), head(-1), tail(-1), data() {
    gps.openFileMapping();
    gps.mappedViewAddr();
    update();
    timer.time(tmr::TIMEOUT_1S); // update draw every 1 second
}

void GPS::update() {
    if (timer.expired()) {
        data.push_back(*(OEM4*)((char*)gps.getBaseAddress()));
        timer.time(tmr::TIMEOUT_1S);
    }
}

void GPS::draw(double x, double y) {
    update();
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    for (const auto & pt : data) {
        glVertex3f(pt.easting / 100 - std::abs(x),
                   pt.height / 1000,
                   pt.northing / 100 - std::abs(y));
    }
    glEnd();
};
