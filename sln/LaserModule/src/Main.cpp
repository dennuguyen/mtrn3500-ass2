#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "SharedMemory.hpp"
#include "Modules.hpp"

#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_LEN 1024

static SOCKET initConnection() {

    // Determine Winsock version
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::wcerr << "ERROR " << WSAGetLastError() << ": Could not retrieve Winsock details" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Prepare socket address info
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    InetPton(AF_INET, mod::LASER.ip, &addr.sin_addr.s_addr);
    addr.sin_port = htons(mod::LASER.port);

    // Create client socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
        std::wcerr << "ERROR: Invalid client socket" << std::endl;

    // Connect to the server
    std::wcout << "Connecting to " << mod::LASER.ip << ":" << mod::LASER.port << std::endl;
    if (connect(clientSocket, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
        std::wcerr << "ERROR " << WSAGetLastError() << ": Could not connect to host" << mod::LASER.ip << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Authorise
    std::wcout << "Authorising " << mod::ZID << std::endl;
    if (send(clientSocket, mod::ZID, 9, 0) == SOCKET_ERROR) {
        std::wcerr << "WARNING " << WSAGetLastError() << ": Could not send data" << std::endl;
    }

    // Indicate success
    std::wcout << "Connection success" << std::endl;

    return clientSocket;
}

int main(int argc, char** argv) {

    // Create file mapping object for this process
    sm::FileMappingObject map(mod::LASER.name, sm::SIZE);
    map.openFileMapping();
    map.mappedViewAddr();

    // Create connection to server
    SOCKET clientSocket = initConnection();

    // Prepare buffers
    char receiveBuffer[BUFFER_LEN];
    //char sendBuffer[BUFFER_LEN];

    while (1) {

        // Receive data from server
        int numBytes = recv(clientSocket, receiveBuffer, BUFFER_LEN, 0);
        std::wcout << "numBytes: " << numBytes << std::endl;

        // Valid reception
        if (numBytes > 0) {
            char* addr = (char*)((LPWSTR)map.getBaseAddress() + 10);
            *addr = *receiveBuffer;
            std::wcout << receiveBuffer << std::endl;
        }

        // Connection closed
        else if (numBytes == 0) {
            std::wcout << "Connection has been closed. Restarting connection." << std::endl;
            clientSocket = initConnection();
        }

        // Invalid reception
        else {
            std::wcerr << "ERROR " << WSAGetLastError() << ": Failed to receive data " << std::endl;
        }
    }

    closesocket(clientSocket);
    WSACleanup();

    return EXIT_SUCCESS;
}