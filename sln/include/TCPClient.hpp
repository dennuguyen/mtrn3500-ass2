#ifndef TCP_HPP_
#define TCP_HPP_

#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>

#include <iostream>
#include <string>

namespace tcp {

const int BUFFER_LEN = 2046;

class TCPClient {
   public:
    TCPClient(const std::wstring ip, const int port, const std::string auth)
        : clientSocket(), ip(ip), port(port), buffer(), auth(auth) {}

    ~TCPClient() {}

    SOCKET tcpConnect() {
        // Determine Winsock version
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::wcerr << "ERROR " << WSAGetLastError() << ": Could not retrieve Winsock details" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Prepare socket address info
        SOCKADDR_IN addr;
        addr.sin_family = AF_INET;
        InetPton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);
        addr.sin_port = htons(port);

        // Create client socket
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET)
            std::wcerr << "ERROR: Invalid client socket" << std::endl;

        // Connect to the server
        std::wcout << "Connecting to " << ip << ":" << port << std::endl;
        if (connect(clientSocket, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
            std::wcerr << "ERROR " << WSAGetLastError() << ": Could not connect to host" << ip << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        // Authenticate
        std::wcout << "Authenticating " << auth.c_str() << std::endl;
        tcpSend(auth);

        // Indicate success
        std::wcout << "Connection success" << std::endl;

        return clientSocket;
    }

    void tcpClose() {
        if (clientSocket != NULL && clientSocket != INVALID_SOCKET)
            closesocket(clientSocket);
        WSACleanup();
    }

    char* tcpSend(std::string data) {
        if (send(clientSocket, data.c_str(), data.size(), 0) == SOCKET_ERROR) {
            std::wcerr << "WARNING " << WSAGetLastError() << ": Could not send data" << std::endl;
        }
        return buffer;
    }

    char* tcpReceive() {
        int numBytes = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (numBytes == 0) {
            std::wcout << "Connection has been closed. Restarting connection." << std::endl;
            tcpConnect();
        }

        if (numBytes < 0) {
            std::wcerr << "ERROR " << WSAGetLastError() << ": Failed to receive data " << std::endl;
        }

        return buffer;
    }

    std::string getBuffer() { return buffer; }

   private:
    SOCKET clientSocket;      // TCP connection client socket
    const std::wstring ip;    // IP address of TCP connection
    const int port;           // Port of TCP connection
    char buffer[BUFFER_LEN];  // Buffer to store data over TCP
    std::string auth;         // Authentication key
};
}  // namespace tcp

#pragma comment(lib, "Ws2_32.lib")  // #include <Winsock2.h>

#endif  // TCP_HPP_