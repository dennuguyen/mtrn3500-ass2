#include <Winsock2.h>

#include <iostream>
#include <sstream>

#include "CircularQueue.hpp"
#include "Modules.hpp"
#include "OEM4.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"  // #include <Winsock2.h>
#include "Timer.hpp"

static uint32_t getCRC32(const unsigned char* data, int n);
static void printGPSData(std::vector<OEM4> data);

int main(int argc, char* argv[]) {

    // Create file mapping object for this process
    sm::FileMappingObject map(mod::GPS.name, sm::SIZE);
    map.openFileMapping();
    map.mappedViewAddr();

    // Create file mapping object to process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.openFileMapping();
    bool* heartbeat = (bool*)((char*)management.mappedViewAddr() + mod::GPS.heartbeat);

    // Create TCP client
    tcp::TCPClient client(mod::GPS.ip, mod::GPS.port, mod::ZID);
    client.tcpConnect();

    // Create a circular buffer queue for OEM4 values
    const uint8_t N = 30;
    uint8_t* maxQueue = (uint8_t*)map.getBaseAddress();
    *maxQueue = N;
    int8_t* head = (int8_t*)((char*)map.getBaseAddress() + 8);
    int8_t* tail = (int8_t*)((char*)map.getBaseAddress() + 16);
    OEM4 (*oem4Queue)[N] = (OEM4(*)[N])((char*)map.getBaseAddress() + 24);

    // Initialise values
    *head = *tail = -1;
    **oem4Queue = {};

    // Create timer
    tmr::Timer timer;
    timer.time(tmr::TIMEOUT_4S);

    while (!timer.expired()) {
        if (*heartbeat == false) {
        
            // Receive GPS data
            unsigned char* buffer = (unsigned char*)(client.tcpReceive());
            unsigned char headerLength = *(buffer + 3);

            // Get CRC value
            uint32_t crc = *(uint32_t*)(buffer + headerLength + 80);

            // Validate GPS data
            if (getCRC32(buffer, 108) == crc) {

                // Get GPS data
                OEM4 oem4 = *(OEM4*)(buffer + headerLength + 16);
                oem4.crc = crc;  // overwrite crc value

                // Process GPS data and store in shared memory
                if (cq::isFull(*head, *tail, N) == true) {
                    cq::dequeue(*oem4Queue, (int&)*head, (int&)*tail);
                }

                if (cq::enqueue(*oem4Queue, oem4, (int&)*head, (int&)*tail) == false) {
                    std::cerr << "ERROR: Could not enqueue OEM4 data" << std::endl;
                }

                // Print GPS data
                printGPSData(cq::tokenise(*oem4Queue, *head, *tail));
            }

            // Set heartbeat
            *heartbeat = true;
            timer.time(tmr::TIMEOUT_2S);
        }

        Sleep(1000);  // 1000 ms refresh rate
    }

    client.tcpClose();
    
    return EXIT_SUCCESS;
}

/**
 * CRC-32 algorithm
 */
static uint32_t getCRC32(const unsigned char* data, int n) {
    uint32_t crc32 = 0;
    while (n-- != 0) {
        uint32_t temp = (crc32 ^ *(data++)) & 0xFF;

        for (int j = 8; j > 0; j--) {
            if (temp & 1)
                temp = (temp >> 1) ^ 0xEDB88320L;
            else
                temp >>= 1;
        }
        crc32 = ((uint32_t)(crc32 >> 8) & 0x00FFFFFFL) ^ temp;
    }
    return crc32;
}

/**
 * Print GPS data which includes Northing, Easting, Height and CRC
 */
static void printGPSData(std::vector<OEM4> data) {
    for (OEM4 d : data) {
        std::cout << "(N: " << d.northing << ", E: " << d.easting << ", H: " << d.height << ")";
        std::cout << " + " << d.crc << std::endl;
    }
    std::cout << std::endl;
}
