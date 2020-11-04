#include <iostream>
#include <iomanip>
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "SharedMemory.hpp"
#include "Modules.hpp"
#include "TCPClient.hpp"

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char** argv) {

    // Create file mapping object for this process
    sm::FileMappingObject map(mod::LASER.name, sm::SIZE);
    map.openFileMapping();
    map.mappedViewAddr();

    // Create TCP client
    tcp::TCPClient client(mod::LASER.ip, mod::LASER.port, mod::ZID);
    SOCKET clientSocket = client.tcpConnect();

    while (1) {
        client.tcpReceive();
        char* addr = (char*)((LPWSTR)map.getBaseAddress() + 10);
        *addr = *client.getBuffer().c_str();

        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (int i = 0; i < 1024; i++)
            ss << std::setw(2) << static_cast<unsigned>(addr[i]);
        std::cout << ss.str();
    }

    closesocket(clientSocket);
    WSACleanup();

    return EXIT_SUCCESS;
}