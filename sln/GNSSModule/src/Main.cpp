#include <iostream>
#include <sstream>
#include <Winsock2.h>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"  // #include <Winsock2.h>
#include "Timer.hpp"


static char* parseGPSData(std::string data) {
    return &data[0];
}

int main(int argc, char** argv) {

    // Create file mapping object for this process
    sm::FileMappingObject map(mod::GPS.name, sm::SIZE);
    map.openFileMapping();
    map.mappedViewAddr();

    // Create file mapping object to process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.openFileMapping();
    bool* heartbeat = (bool*)((char*)management.mappedViewAddr() + mod::GPS.heartbeat);

    // Create TCP client
   //tcp::TCPClient client(mod::GPS.ip, mod::GPS.port, mod::ZID);
   //client.tcpConnect();

    // Create timer
    tmr::Timer timer;
    timer.time(tmr::TIMEOUT_4S);

    while (!timer.expired()) {
        if (*heartbeat == false) {

            //std::string buffer = client.tcpReceive();
            //char** addr = (char**)((LPWSTR)map.getBaseAddress() + 10);
            //*addr = parseGPSData(buffer.substr(26));
            //std::cout << addr << " : " << *addr << std::endl;

            Sleep(500);

            // Set heartbeat
            *heartbeat = true;
            timer.time(tmr::TIMEOUT_4S);
        }
    }

    //client.tcpClose();

	return EXIT_SUCCESS;
}
