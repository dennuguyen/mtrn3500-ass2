#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "SharedMemory.hpp"

#pragma comment(lib, "Ws2_32.lib")

constexpr PCWSTR ip = L"192.168.1.200";
constexpr int port = 23000;
constexpr int BUFFER_LEN = 1024;

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
    InetPton(AF_INET, ip, &addr.sin_addr.s_addr);
    addr.sin_port = htons(port);

    // Create client socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
        std::wcerr << "ERROR: Invalid client socket" << std::endl;

    // Connect to the server
    if (connect(clientSocket, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
        std::wcerr << "ERROR " << WSAGetLastError() << ": Could not connect to host" << ip << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    return clientSocket;
}

int main(int argc, char** argv) {

    // Create file mapping object for this process
    sm::FileMappingObject map(sm::LASER, sm::SIZE);
    map.openFileMapping();
    map.mappedViewAddr();

    // Create connection to server
    SOCKET clientSocket = initConnection();

    // Prepare buffers
    char receiveBuffer[BUFFER_LEN];
    //char sendBuffer[BUFFER_LEN];

    while (1) {
        // send(clientSocket, reinterpret_cast<SOCKADDR *>(&addr), sizeof(addr));

        // Receive data from server
        int numBytes = recv(clientSocket, receiveBuffer, BUFFER_LEN, 0);
        
        // Valid reception
        if (numBytes > 0) {
            int* addr = (int*)((LPWSTR)map.getBaseAddress() + 10);
            *addr = numBytes;
            std::wcout << map.getBaseAddress() << std::endl;
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