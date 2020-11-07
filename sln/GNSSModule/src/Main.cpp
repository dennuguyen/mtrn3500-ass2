#include <Winsock2.h>

#include <iostream>
#include <sstream>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"  // #include <Winsock2.h>
#include "Timer.hpp"


#define CRC32_POLYNOMIAL			0xEDB88320L

unsigned long CRC32Value(int i)
{
    int j;
    unsigned long ulCRC;
    ulCRC = i;
    for (j = 8; j > 0; j--)
    {
        if (ulCRC & 1)
            ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
        else
            ulCRC >>= 1;
    }
    return ulCRC;
}
/* --------------------------------------------------------------------------
Calculates the CRC-32 of a block of data all at once
-------------------------------------------------------------------------- */
unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */
    unsigned char* ucBuffer) /* Data block */
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
};

static bool checkCRC32(char* data);
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
            char* buffer = (char*)(client.tcpReceive());
            
            std::cout << CalculateBlockCRC32(108, (unsigned char*)buffer) << std::endl;

            // Consider non-corrupt GPS data
            if (checkCRC32(buffer) == true) {

                // Process GPS data and store in shared memory
                OEM4* oem4 = ((OEM4*)map.getBaseAddress());
                *oem4 = *(OEM4*)((char*)buffer + 44);
                unsigned int crc = (unsigned int)(buffer + 108);

                // Print GPS data
                printGPSData(*oem4, crc);
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

static bool checkCRC32(char* data) {

    unsigned int crc32 = 0xFFFFFFFF;

    for (char* byte = data; *byte != '\0'; *(byte++)) {
        crc32 ^= *byte;

        for (int i = 7; i >= 0; i--)
            crc32 = (crc32 >> 1) ^ (0xEDB88320L & (crc32 & 1));
    }

    std::cout << ~crc32 << " " << (unsigned int)(data + 108) << std::endl;

    return ~crc32 == (unsigned int)(data + 109);
}

static void printGPSData(OEM4 oem4, unsigned int crc) {
    std::cout << "(N: " << oem4.northing << ", E: " << oem4.easting << ", H: " << oem4.height << ")";
    std::cout << "\tCRC: " << crc << std::endl;
}
