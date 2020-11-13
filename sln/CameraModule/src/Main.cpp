#include <Winsock2.h>

#include <iostream>

#include <GL/freeglut.h>
#include <turbojpeg.h>
#include <Windows.h>
#include <zmq.hpp>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"
#include "Timer.hpp"

#pragma comment(lib, "turbojpeg.lib")
#pragma comment(lib, "libzmq-v120-mt-gd-4_0_4.lib")

static GLuint texture;
static zmq::context_t context(1);
static zmq::socket_t subscriber(context, ZMQ_SUB);
tmr::Timer timer;
static bool* heartbeat;

static void display();
static void idle();

int main(int argc, char* argv[]) {

    // Create file mapping object to process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.openFileMapping();
    heartbeat = (bool*)((char*)management.mappedViewAddr() + mod::CAMERA.heartbeat);

    // Start timer
    timer.time(tmr::TIMEOUT_4S);

    // Socket to talk to server
    subscriber.connect("tcp://192.168.1.200:26000");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    // Initialise GL window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(200, 200);
    glutCreateWindow("UGV Camera");

    // Specify GL callbacks
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glGenTextures(1, &texture);

    // Run main
    glutMainLoop();

    return EXIT_SUCCESS;
}

static void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2f(-1, -1);
    glTexCoord2f(1, 1);
    glVertex2f(1, -1);
    glTexCoord2f(1, 0);
    glVertex2f(1, 1);
    glTexCoord2f(0, 0);
    glVertex2f(-1, 1);
    glEnd();

    glutSwapBuffers();
}

static void idle() {
    zmq::message_t buffer;
    if (subscriber.recv(&buffer, ZMQ_NOBLOCK)) {
        unsigned long numBytes = buffer.size();
        unsigned char* compressed = (unsigned char*)buffer.data();
        int jpegSubsample = 0, width = 0, height = 0;

        tjhandle decompressor = tjInitDecompress();
        if (tjDecompressHeader2(decompressor, compressed, numBytes, &width, &height, &jpegSubsample) == -1) {
            std::cerr << "ERROR: Could not decompress header" << std::endl;
            glutLeaveMainLoop();
        }

        unsigned char* decompressed = new unsigned char[width * height * 3];

        if (tjDecompress2(decompressor, compressed, numBytes, decompressed, width, 0, height, TJPF_RGB, TJFLAG_FASTDCT) == -1) {
            std::cerr << "ERROR: Could not decompress image" << std::endl;
            glutLeaveMainLoop();
        }

        if (tjDestroy(decompressor) == -1) {
            std::cerr << "ERROR: Could not destroy turbojpeg handle" << std::endl;
            glutLeaveMainLoop();
        }

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, decompressed);
        delete[] decompressed;
    }

    display();

    if (timer.expired()) {
        glutLeaveMainLoop();
    }

    if (*heartbeat == false) {
        *heartbeat = true;
        timer.time(tmr::TIMEOUT_2S);
    }
}
