#include <Winsock2.h>

#include <iostream>
#include <sstream>

#include "Modules.hpp"
#include "OEM4.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"  // #include <Winsock2.h>
#include "Timer.hpp"


static uint32_t getCRC32(const unsigned char* data, int n);
static void printGPSData(OEM4 oem4);
static void printCRC32Value(uint32_t oem4, uint32_t crc);

int main(int argc, char* argv[]) {
    // Create file mapping object for this process
    sm::FileMappingObject map(mod::GPS.name, sm::SIZE);
    map.openFileMapping();
    map.mappedViewAddr();
    OEM4* oem4 = (OEM4*)map.getBaseAddress();

    // Create file mapping object to process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.openFileMapping();
    bool* heartbeat = (bool*)((char*)management.mappedViewAddr() + mod::GPS.heartbeat);

    // Create TCP client
    tcp::TCPClient client(mod::GPS.ip, mod::GPS.port, mod::ZID);
    client.tcpConnect();

    // Create timer
    tmr::Timer timer;
    timer.time(tmr::TIMEOUT_4S);

    while (!timer.expired()) {
        if (*heartbeat == false) {

            // Receive GPS data
            unsigned char* buffer = (unsigned char*)(client.tcpReceive());
            unsigned char headerLength = *(buffer + 3);

            // Get expected and actual CRC32 values
            uint32_t expected = *(uint32_t*)(buffer + headerLength + 80);
            uint32_t actual = getCRC32(buffer, 108);

            // Print CRC32 values
            printCRC32Value(expected, actual);

            // Validate GPS data
            if (expected == actual) {

                // Process GPS data and store in shared memory
                *oem4 = *(OEM4*)(buffer + headerLength + 16);

                // Print GPS data
                printGPSData(*oem4);
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
static void printGPSData(OEM4 oem4) {
    std::cout << "(N: " << oem4.northing << ", E: " << oem4.easting << ", H: " << oem4.height << ")" << std::endl;
}

/**
 * Print expected and actual CRC value
 */
static void printCRC32Value(uint32_t expected, uint32_t actual) {
    std::cout << "CRC32: " << expected << (expected == actual ? " == " : " != ") << actual << "\t";
}
