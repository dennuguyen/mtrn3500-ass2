#include <Winsock2.h>

#include <iostream>
#include <sstream>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"  // #include <Winsock2.h>
#include "Timer.hpp"
unsigned long CRC32Value(int i)
{
    int j;
    unsigned long ulCRC;
    ulCRC = i;
    for (j = 8; j > 0; j--)
    {
        if (ulCRC & 1)
            ulCRC = (ulCRC >> 1) ^ 0xEDB88320L;
        else
            ulCRC >>= 1;
    }
    return ulCRC;
}

unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */
    const unsigned char* ucBuffer) /* Data block */
{
    unsigned long ulTemp1;
    unsigned long ulTemp2;
    unsigned long ulCRC = 0;
    while (ulCount-- != 0)
    {
        ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
        ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff);
        ulCRC = ulTemp1 ^ ulTemp2;
    }
    return(ulCRC);
}
struct OEM4 {
    double northing;
    double easting;
    double height;
    uint32_t _garbage1;
    uint8_t _garbage2;
    uint32_t crc;
};

static bool checkCRC32(const unsigned char* data, int n);
static void printGPSData(OEM4 oem4, unsigned int crc);

int main(int argc, char** argv) {
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

            // Consider non-corrupt GPS data
            if (checkCRC32(buffer, 108) == false) {

                // Process GPS data and store in shared memory
                OEM4* oem4 = ((OEM4*)map.getBaseAddress());
                *oem4 = *(OEM4*)((char*)buffer + headerLength + 16);
                std::cout << oem4->crc << std::endl;
                uint32_t crc = (uint32_t)(buffer + headerLength + 80);

                // Print GPS data
                // printGPSData(*oem4, crc);
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
static bool checkCRC32(const unsigned char* data, int n) {

    uint32_t crc32 = 0xFFFFFFFF;

    for (int i = 0; i < n; i++) {
        char ch = data[i];

        for (int j = 0; j < 8; j++) {
            crc32 >>= 1;
            if ((ch ^ crc32) & 1)
                crc32 ^= 0xEDB88320;
            ch >>= 1;
        }
    }
    std::cout << ~crc32 << " " << (uint32_t)(data + 108) << " " << CalculateBlockCRC32(n, data) << std::endl;

    return ~crc32 == (uint32_t)(data + 108);
}

/**
 * Print GPS data which includes Northing, Easting, Height and CRC
 */
static void printGPSData(OEM4 oem4, unsigned int crc) {
    std::cout << "(N: " << oem4.northing << ", E: " << oem4.easting << ", H: " << oem4.height << ")";
    std::cout << " + CRC: " << crc << std::endl;
}
