#include <iostream>
#include <iterator>
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "SharedMemory.hpp"
#include "Modules.hpp"
#include "TCPClient.hpp"

#pragma comment(lib, "Ws2_32.lib")

static void parsePointCloud(std::string data) {

    // Tokenize data
    std::stringstream dataStream;
    dataStream << data;
    std::vector<std::string> dataVector = { std::istream_iterator<std::string>{dataStream}, std::istream_iterator<std::string>{} };

    // Check valid data
    if (dataVector.at(0) != "DIST1") {
        std::cerr << "ERROR: Incorrect data format" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Get info
    double scalingFactor = std::stod(dataVector.at(1));
    double scalingOffset = std::stod(dataVector.at(2)); // always zero
    double angle = std::stoi(dataVector.at(3)) / 10000.0;
    double stepWidth = std::stoi(dataVector.at(4)) / 10000.0;
    int numData = std::stoi(dataVector.at(5));
    
    // Parse data
    std::vector<std::pair<double, double>> coords;
    for (int i = 6; i < numData + 6; i++) {
        ULONG radius = std::stol(dataVector.at(i), nullptr, 16);
        //coords.push_back({std::stod(dataVector.at(i)) * cos(angle), std::stod(dataVector.at(i)) * sin(angle) });
        angle += stepWidth;
    }
    
    //return coords;
}

int main(int argc, char** argv) {

    // Create file mapping object for this process
    sm::FileMappingObject map(mod::LASER.name, sm::SIZE);
    map.openFileMapping();
    map.mappedViewAddr();

    // Create TCP client
    tcp::TCPClient client(mod::LASER.ip, mod::LASER.port, mod::ZID);
    SOCKET clientSocket = client.tcpConnect();

    while (1) {
        std::stringstream request;
        request << (char)0x02 << "sRN LMDscandata" << (char)0x03;
        client.tcpSend(request.str());

        Sleep(500);

        std::string buffer = client.tcpReceive();
        parsePointCloud(buffer.substr(buffer.find("DIST1")));

        //char* addr = (char*)((LPWSTR)map.getBaseAddress() + 10);
        //*addr = *client.getBuffer().c_str();
    }

    closesocket(clientSocket);
    WSACleanup();

    return EXIT_SUCCESS;
}