#include <Winsock2.h>

#include <iostream>
#include <sstream>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"
#include "Timer.hpp"

int main(int argc, char** argv) {
    // Create file mapping object for this process
    sm::FileMappingObject map(mod::TELEOP.name, sm::SIZE);
    map.openFileMapping();
    map.mappedViewAddr();

    // Create file mapping object to process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.openFileMapping();
    bool* heartbeat = (bool*)((char*)management.mappedViewAddr() + mod::TELEOP.heartbeat);

    // Create TCP client
    tcp::TCPClient client(mod::TELEOP.ip, mod::TELEOP.port, mod::ZID);
    client.tcpConnect();

    // Create timer
    tmr::Timer timer;
    timer.time(tmr::TIMEOUT_4S);

    while (!timer.expired()) {
        if (*heartbeat == false) {
            
            // Send teleop
            std::stringstream command;
            command << "# 20 0.5 1 #";
            std::cout << client.tcpSend(command.str()) << std::endl;

            // Give server time to prepare data
            Sleep(500);
            /*
            // Confirm command
            std::string buffer = client.tcpReceive();
            std::cout << buffer << std::endl;
            */
            // Set heartbeat
            *heartbeat = true;
            timer.time(tmr::TIMEOUT_4S);
        }

        Sleep(100);  // 100 ms refresh rate
    }

    client.tcpClose();

    return EXIT_SUCCESS;
}

