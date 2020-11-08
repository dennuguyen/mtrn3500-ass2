#include <Winsock2.h>

#include <iostream>
#include <thread>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <Windows.h>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"
#include "Timer.hpp"
#include "turbojpeg.h"

static void glutThread();
static void display();
static void idle();

GLuint tex;
tcp::TCPClient client(mod::CAMERA.ip, mod::CAMERA.port, mod::ZID);

int main(int argc, char* argv[]) {

    // Connect TCP client
    client.tcpConnect();

    //GL Window setup
    std::cout << "Creating window" << std::endl;
    glutInit(&argc, (char**)(argv));
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 600);
    glutCreateWindow("MTRN3500 - Camera");

    // Create thread for glut
    std::thread(glutThread);

    // Create file mapping object to process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.openFileMapping();
    bool* heartbeat = (bool*)((char*)management.mappedViewAddr() + mod::CAMERA.heartbeat);

    // Create timer
    tmr::Timer timer;
    timer.time(tmr::TIMEOUT_4S);

    while (!timer.expired()) {
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

static void glutThread() {
    glutMainLoop();
}

static void display() {
    //Set camera as gl texture
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    //Map Camera to window
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

    if (glutGetWindow() == 0) {
        std::cerr << "ERROR: Camera window is closed" << std::endl;
        std::terminate();
    }
}

static void idle() {
    /*
    // Receive camera data
    std::string update = client.tcpReceive();
    long unsigned int _jpegSize = update.size();
    std::cout << "received " << _jpegSize << " bytes of data\n";
    unsigned char* _compressedImage = (unsigned char*)(update.data());
    int jpegSubsamp = 0, width = 0, height = 0;

    // JPEG Decompression
    tjhandle _jpegDecompressor = tjInitDecompress();
    tjDecompressHeader2(_jpegDecompressor, _compressedImage, _jpegSize, &width, &height, &jpegSubsamp);
    unsigned char* buffer = new unsigned char[width * height * 3];  //!< will contain the decompressed image
    printf("Dimensions:  %d   %d\n", height, width);
    tjDecompress2(_jpegDecompressor, _compressedImage, _jpegSize, buffer, width, 0, height, TJPF_RGB, TJFLAG_FASTDCT);
    tjDestroy(_jpegDecompressor);

    // Load texture
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, buffer);
    delete[] buffer;
    */
    display();
}
