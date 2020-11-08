#include <Winsock2.h>

#include <iostream>

#include <GL/freeglut.h>
#include <turbojpeg.h>
#include <Windows.h>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"
#include "Timer.hpp"

static GLuint texture;
static tcp::TCPClient client(mod::CAMERA.ip, mod::CAMERA.port, mod::ZID);

static void display();
static void idle();

int main(int argc, char* argv[]) {

    // Connect TCP client
    client.tcpConnect();

    // Create file mapping object to process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.openFileMapping();
    bool* heartbeat = (bool*)((char*)management.mappedViewAddr() + mod::CAMERA.heartbeat);

    // Create timer
    tmr::Timer timer;
    timer.time(tmr::TIMEOUT_4S);

    // Initialise GL window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("UGV Camera");

    // Specify GL callbacks
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glGenTextures(1, &texture);

    while (!timer.expired()) {

        glutMainLoopEvent();

        if (*heartbeat == false) {
            *heartbeat = true;
            timer.time(tmr::TIMEOUT_4S);
        }
        Sleep(1000);  // 1000 ms refresh rate
    }

    // Close TCP client
    client.tcpClose();

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
    std::string buffer = client.tcpReceive();
    std::cout << buffer.data() << std::endl;

    unsigned long numBytes = buffer.size();
    unsigned char* compressed = (unsigned char*)buffer.data();
    int jpegSubsample = 0, width = 0, height = 0;

    tjhandle decompressor = tjInitDecompress();
    //tjDecompressHeader2(decompressor, compressed, numBytes, &width, &height, &jpegSubsample);
    //unsigned char* decompressed = new unsigned char[width * height * 3];
    //tjDecompress2(decompressor, compressed, numBytes, decompressed, width, 0, height, TJPF_RGB, TJFLAG_FASTDCT);
    //tjDestroy(decompressor);
    //
    //glBindTexture(GL_TEXTURE_2D, texture);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, decompressed);
    //delete[] decompressed;
}
