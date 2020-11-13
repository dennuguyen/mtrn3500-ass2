#include <Winsock2.h>

#include <array>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <sstream>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"  // #include <Winsock2.h>
#include "Timer.hpp"

typedef std::array<std::pair<double, double>, 400>  PointList;

static int parsePointCloud(std::string data, PointList* coords);
static void printPoints(PointList coords);

int main(int argc, char* argv[]) {

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

            // Receive laser scan data
            std::string buffer = client.tcpReceive();

            // Process and store laser scan data in shared memory
            uint16_t* numPoints = (uint16_t*)((char*)map.getBaseAddress());
            PointList* points = (PointList*)((char*)map.getBaseAddress() + 16);
            *numPoints = parsePointCloud(buffer, points);
            std::cout << (unsigned)*numPoints << " ";

            // Print points
            //printPoints(*points);
            
            // Set heartbeat
            *heartbeat = true;
            timer.time(tmr::TIMEOUT_4S);
        }

        Sleep(100); // 100 ms refresh rate
    }

    client.tcpClose();

    return EXIT_SUCCESS;
}

/**
 * Process the point cloud data from polar to cartesian coordinates which are stored in coords. Returns 
 * the length of the coords array.
 */
static int parsePointCloud(std::string data, PointList* coords) {

    // Tokenize data from hexadecimal
    std::stringstream dataStream;
    dataStream << data.substr(data.find("DIST1"));
    std::vector<std::string> dataVector = { std::istream_iterator<std::string>{dataStream}, std::istream_iterator<std::string>{} };

    // Check valid data
    if (dataVector.at(0) != "DIST1") {
        std::cerr << "ERROR: Incorrect data format" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Get info
    double scalingFactor = std::stod(dataVector.at(1));
    double scalingOffset = std::stod(dataVector.at(2));  // always zero
    double startAngle = std::stoi(dataVector.at(3), nullptr, 16) / 10000.0;
    double stepWidth = std::stoi(dataVector.at(4), nullptr, 16) / 10000.0 ; // 0.5 deg
    int numData = std::stoi(dataVector.at(5), nullptr, 16);
    
    // Parse data
    double angle = startAngle;
    for (unsigned int i = 6; i < numData + 6; i++) {
        unsigned long radius = std::stol(dataVector.at(i), nullptr, 16);
        (*coords)[i] = { radius * cos(angle * 3.1415 / 180), radius * sin(angle * 3.1415 / 180) };
        angle += 0.5;
    }

    return numData;
}

/**
 * Prints the coordinates
 */
static void printPoints(PointList coords) {
    for (const auto & [x, y] : coords)
        std::cout << "(" << x << ", " << y << ")" << std::endl;
    std::cout << std::endl << std::endl;
}
