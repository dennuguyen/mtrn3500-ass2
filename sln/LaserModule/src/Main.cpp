#include <iostream>
#include <iterator>
#include <sstream>
#include <Winsock2.h>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"  // #include <Winsock2.h>
#include "Timer.hpp"

typedef std::vector<std::pair<double, double>>  PointList;

static PointList parsePointCloud(std::string data);

int main(int argc, char** argv) {

    // Create file mapping object for this process
    sm::FileMappingObject map(mod::LASER.name, sm::SIZE);
    map.openFileMapping();
    map.mappedViewAddr();

    // Create file mapping object to process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.openFileMapping();
    bool* heartbeat = (bool*)((char*)management.mappedViewAddr() + mod::LASER.heartbeat);

    // Create TCP client
    tcp::TCPClient client(mod::LASER.ip, mod::LASER.port, mod::ZID);
    client.tcpConnect();

    // Create timer
    tmr::Timer timer;
    timer.time(tmr::TIMEOUT_4S);

    while (!timer.expired()) {
        if (*heartbeat == false) {
            
            // Request laser scan data
            std::stringstream request;
            request << (char)0x02 << "sRN LMDscandata" << (char)0x03;
            client.tcpSend(request.str());

            // Give server time to prepare data
             Sleep(500);

             // Receive and parse data
             std::string buffer = client.tcpReceive();
             PointList* addr = (PointList*)((LPWSTR)map.getBaseAddress());
             *addr = parsePointCloud(buffer.substr(buffer.find("DIST1")));

             // Set heartbeat
             *heartbeat = true;
             timer.time(tmr::TIMEOUT_4S);
        }

        Sleep(100); // 100 ms refresh rate
    }

    client.tcpClose();

    return EXIT_SUCCESS;
}

static PointList parsePointCloud(std::string data) {

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
    double scalingOffset = std::stod(dataVector.at(2));         // always zero
    double angle = std::stoi(dataVector.at(3)) / 10000.0 - 90;  // subtract 90 to correct reference frame orientation
    double stepWidth = std::stoi(dataVector.at(4)) / 10000.0;
    int numData = std::stoi(dataVector.at(5));

    // Parse data
    std::vector<std::pair<double, double>> coords;
    for (int i = 6; i < numData + 6; i++) {
        ULONG radius = std::stol(dataVector.at(i), nullptr, 16);
        coords.push_back({ radius * cos(angle), radius * sin(angle) });
        angle += stepWidth;
    }

    return coords;
}
