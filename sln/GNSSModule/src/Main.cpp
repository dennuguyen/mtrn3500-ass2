#include <Winsock2.h>

#include <iostream>
#include <sstream>

#include "CircularQueue.hpp"
#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"  // #include <Winsock2.h>
#include "Timer.hpp"

#pragma pack(push, 1)
struct OEM4 {
    double northing;
    double easting;
    double height;
};
#pragma pack(pop)

static uint32_t getCRC32(const unsigned char* data, int n);
static void printGPSData(OEM4 oem4, uint32_t crc);

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

    // Create a circular buffer queue of OEM4 struct
    uint8_t* head = (uint8_t*)map.getBaseAddress();
    uint8_t* tail = (uint8_t*)((char*)map.getBaseAddress() + 8);
    OEM4 (*queue)[20] = (OEM4(*)[20])((char*)map.getBaseAddress() + 16);

    // Create timer
    tmr::Timer timer;
    timer.time(tmr::TIMEOUT_4S);

    while (!timer.expired()) {
        if (*heartbeat == false) {

            // Wait for no reason
            Sleep(500);

            // Receive GPS data
            unsigned char* buffer = (unsigned char*)(client.tcpReceive());
            unsigned char headerLength = *(buffer + 3);

            // Get CRC value
            uint32_t crc = *(uint32_t*)(buffer + headerLength + 80);

            // Validate GPS data
            if (getCRC32(buffer, 108) == crc) {

                // Get GPS data
                OEM4 oem4 = *(OEM4*)(buffer + headerLength + 16);

                // Process GPS data and store in shared memory
                if (cq::isFull(*head, *tail, 20) == true)
                    cq::dequeue(*queue, (int)*head, (int)*tail);
                
                if (cq::enqueue(*queue, oem4, (int)*head, (int)*tail) == false) {
                    std::cerr << "ERROR: Could not enqueue OEM4 data" << std::endl;
                }

                // Print GPS data
                //printGPSData(queue, crc);
            }

            // Set heartbeat
            *heartbeat = true;
            timer.time(tmr::TIMEOUT_4S);
        }

        Sleep(500);  // 500 ms refresh rate
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
static void printGPSData(OEM4 oem4, uint32_t crc) {
    std::cout << "(N: " << oem4.northing << ", E: " << oem4.easting << ", H: " << oem4.height << ")";
    std::cout << " + " << crc << std::endl;
}
