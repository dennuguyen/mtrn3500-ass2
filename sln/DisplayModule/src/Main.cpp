#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <unistd.h>
#include <sys/time.h>
#elif defined(WIN32)
#include <Windows.h>
#include <tchar.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#include <GL\freeglut_ext.h>

#include "Camera.hpp"
#include "GPS.hpp"
#include "Ground.hpp"
#include "HUD.hpp"
#include "KeyManager.hpp"
#include "LIDAR.hpp"
#include "Messages.hpp"
#include "Modules.hpp"
#include "MyVehicle.hpp"
#include "Shape.hpp"
#include "Timer.hpp"
#include "Vehicle.hpp"

void display();
void reshape(int width, int height);
void idle();

void keydown(unsigned char key, int x, int y);
void keyup(unsigned char key, int x, int y);
void special_keydown(int keycode, int x, int y);
void special_keyup(int keycode, int x, int y);

void mouse(int button, int state, int x, int y);
void dragged(int x, int y);
void motion(int x, int y);

// Shared memory and heartbeats
tmr::Timer timer;
static bool* heartbeat;
const static double* speed;
const static double* steer;

using namespace scos;

// Used to store the previous mouse location so we
//   can calculate relative mouse movement.
int prev_mouse_x = -1;
int prev_mouse_y = -1;

// vehicle control related variables
Vehicle* vehicle = nullptr;

int main(int argc, char* argv[]) {
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("MTRN3500 - GL");

    Camera::get()->setWindowDimensions(WINDOW_WIDTH, WINDOW_HEIGHT);

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    glutKeyboardFunc(keydown);
    glutKeyboardUpFunc(keyup);
    glutSpecialFunc(special_keydown);
    glutSpecialUpFunc(special_keyup);

    glutMouseFunc(mouse);
    glutMotionFunc(dragged);
    glutPassiveMotionFunc(motion);

    vehicle = new MyVehicle();

    // Create file mapping object to process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.openFileMapping();
    heartbeat = (bool*)((char*)management.mappedViewAddr() + mod::DISPLAY.heartbeat);

    // Create file mapping object to teleoperations
    sm::FileMappingObject teleop(mod::TELEOP.name, sm::SIZE);
    teleop.openFileMapping();
    teleop.mappedViewAddr();
    steer = (double*)((char*)teleop.getBaseAddress());
    speed = (double*)((char*)teleop.getBaseAddress() + sizeof(double));

    // Start timer
    timer.time(tmr::TIMEOUT_4S);

    glutMainLoop();

    if (vehicle != nullptr)
        delete vehicle;

    return 0;
}

void display() {
    // -------------------------------------------------------------------------
    //  This method is the main draw routine.
    // -------------------------------------------------------------------------

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (Camera::get()->isPursuitMode() && vehicle != NULL) {
        double x = vehicle->getX(), y = vehicle->getY(), z = vehicle->getZ();
        double dx = cos(vehicle->getRotation() * 3.141592765 / 180.0);
        double dy = sin(vehicle->getRotation() * 3.141592765 / 180.0);
        Camera::get()->setDestPos(x + (-3 * dx), y + 7, z + (-3 * dy));
        Camera::get()->setDestDir(dx, -1, dy);
    }
    Camera::get()->updateLocation();
    Camera::get()->setLookAt();

    Ground::draw();

    // draw my vehicle
    if (vehicle != NULL) {
        vehicle->draw();
    }

    // draw HUD
    HUD::Draw();

    glutSwapBuffers();
};

void reshape(int width, int height) {
    Camera::get()->setWindowDimensions(width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
};

double getTime() {
#if defined(WIN32)
    LARGE_INTEGER freqli;
    LARGE_INTEGER li;
    if (QueryPerformanceCounter(&li) && QueryPerformanceFrequency(&freqli)) {
        return double(li.QuadPart) / double(freqli.QuadPart);
    } else {
        static ULONGLONG start = GetTickCount64();
        return (GetTickCount64() - start) / 1000.0;
    }
#else
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + (t.tv_usec / 1000000.0);
#endif
}

void idle() {
    if (KeyManager::get().isAsciiKeyPressed('a')) {
        Camera::get()->strafeLeft();
    }

    if (KeyManager::get().isAsciiKeyPressed('c')) {
        Camera::get()->strafeDown();
    }

    if (KeyManager::get().isAsciiKeyPressed('d')) {
        Camera::get()->strafeRight();
    }

    if (KeyManager::get().isAsciiKeyPressed('s')) {
        Camera::get()->moveBackward();
    }

    if (KeyManager::get().isAsciiKeyPressed('w')) {
        Camera::get()->moveForward();
    }

    if (KeyManager::get().isAsciiKeyPressed(' ')) {
        Camera::get()->strafeUp();
    }

    const float sleep_time_between_frames_in_seconds = 0.025;
    static double previousTime = getTime();
    const double currTime = getTime();
    const double elapsedTime = currTime - previousTime;
    previousTime = currTime;

    // do a simulation step
    if (vehicle != NULL) {
        vehicle->update(*speed, -(*steer), elapsedTime);
    }

    display();

    if (timer.expired()) {
        glutLeaveMainLoop();
    }

    if (*heartbeat == false) {
        *heartbeat = true;
        timer.time(tmr::TIMEOUT_2S);
    }

#ifdef _WIN32
    Sleep(sleep_time_between_frames_in_seconds * 1000);
#else
    usleep(sleep_time_between_frames_in_seconds * 1e6);
#endif
};

void keydown(unsigned char key, int x, int y) {
    // keys that will be held down for extended periods of time will be handled
    //   in the idle function
    KeyManager::get().asciiKeyPressed(key);

    // keys that react ocne when pressed rather than need to be held down
    //   can be handles normally, like this...
    switch (key) {
        case 27:  // ESC key
            exit(0);
            break;
        case '0':
            Camera::get()->jumpToOrigin();
            break;
        case 'p':
            Camera::get()->togglePursuitMode();
            break;
    }
};

void keyup(unsigned char key, int x, int y) {
    KeyManager::get().asciiKeyReleased(key);
};

void special_keydown(int keycode, int x, int y) {
    KeyManager::get().specialKeyPressed(keycode);
};

void special_keyup(int keycode, int x, int y) {
    KeyManager::get().specialKeyReleased(keycode);
};

void mouse(int button, int state, int x, int y){};

void dragged(int x, int y) {
    if (prev_mouse_x >= 0) {
        int dx = x - prev_mouse_x;
        int dy = y - prev_mouse_y;
        Camera::get()->mouseRotateCamera(dx, dy);
    }
    prev_mouse_x = x;
    prev_mouse_y = y;
};

void motion(int x, int y) {
    prev_mouse_x = x;
    prev_mouse_y = y;
};
