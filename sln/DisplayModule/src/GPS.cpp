#include "GPS.hpp"

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

GPS::GPS() : gps(mod::GPS.name, sm::SIZE), numPoints(30), head(-1), tail(-1), data() {
    gps.openFileMapping();
    gps.mappedViewAddr();
    update();
    timer.time(tmr::TIMEOUT_1S);  // update draw every 1 second
}

/**
 * Update the data vector with GPS data on a timely basis
 */
void GPS::update() {
    if (timer.expired()) {
        data.push_back(*(OEM4*)((char*)gps.getBaseAddress()));
        timer.time(tmr::TIMEOUT_1S);
    }
}

void GPS::draw(double x, double y) {
    update();
    glBegin(GL_LINE_STRIP);
    glColor3f(0.0, 1.0, 0.0);
    for (const auto& pt : data) {
        if (d_cmp(pt.height, 0.0, eps) == false) {
            glVertex3f(pt.easting / 1 - std::abs(x),
                       pt.height / 1000,
                       pt.northing / 1 - std::abs(y));
        }
    }
    glEnd();
};
