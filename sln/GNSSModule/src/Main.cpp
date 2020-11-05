#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"

int main(int argc, char** argv) {

    // Create file mapping object for this process
    sm::FileMappingObject map(mod::GPS.name, sm::SIZE);
    map.openFileMapping();
    map.mappedViewAddr();

    // Create TCP client
    tcp::TCPClient client(mod::GPS.ip, mod::GPS.port, mod::ZID);
    client.tcpConnect();

    while (1) {

    }

	return EXIT_SUCCESS;
}