#include <iostream>
#include <Winsock2.h>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"
#include "Timer.hpp"

int main(int argc, char** argv) {

    // Create file mapping object for this process
    sm::FileMappingObject map(mod::CAMERA.name, sm::SIZE);
    map.openFileMapping();
    map.mappedViewAddr();

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
            // Set heartbeat
            *heartbeat = true;
            timer.time(tmr::TIMEOUT_4S);
        }

        Sleep(1000); // 1000 ms refresh rate
    }

    client.tcpClose();

	return EXIT_SUCCESS;
}
