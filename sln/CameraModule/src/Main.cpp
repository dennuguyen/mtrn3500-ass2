#include <Winsock2.h>

#include <iostream>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"
#include "Timer.hpp"

static void openImage() {

}

int main(int argc, char** argv) {

    // Create file mapping object to process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.openFileMapping();
    bool* heartbeat = (bool*)((char*)management.mappedViewAddr() + mod::CAMERA.heartbeat);

    // Create TCP client
    tcp::TCPClient client(mod::CAMERA.ip, mod::CAMERA.port, mod::ZID);
    client.tcpConnect();

    // Create timer
    tmr::Timer timer;
    timer.time(tmr::TIMEOUT_4S);

    while (!timer.expired()) {
        if (*heartbeat == false) {

            // Receive and process camera data
            //char* buffer = client.tcpReceive();


            // View camera image
            

            // Set heartbeat
            *heartbeat = true;
            timer.time(tmr::TIMEOUT_4S);
        }

        Sleep(1000);  // 1000 ms refresh rate
    }

    client.tcpClose();

    return EXIT_SUCCESS;
}
