#include <Winsock2.h>

#include <iostream>
#include <sstream>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"
#include "Timer.hpp"

static std::tuple<double, double> teleopInput();
static double limit(double value, double min, double max);
static void printCommand(std::string command);

int main(int argc, char* argv[]) {
    // Create file mapping object for teleoperation
    sm::FileMappingObject teleop(mod::TELEOP.name, sm::SIZE);
    teleop.createFileMapping();
    teleop.mappedViewAddr();
    double* steerAddr = (double*)((char*)teleop.getBaseAddress());
    double* speedAddr = (double*)((char*)teleop.getBaseAddress() + sizeof(double));

    // Create file mapping object to process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.openFileMapping();
    bool* heartbeat = (bool*)((char*)management.mappedViewAddr() + mod::TELEOP.heartbeat);

    // Create TCP client
    tcp::TCPClient client(mod::TELEOP.ip, mod::TELEOP.port, mod::ZID);
    client.tcpConnect();

    // Create timer
    tmr::Timer timer;
    timer.time(tmr::TIMEOUT_4S);

    // Teleop flag to UGV server
    bool flag = 0;

    while (!timer.expired()) {
        if (*heartbeat == false) {
            // Get teleop command
            std::stringstream command;
            auto [steer, speed] = teleopInput();
            command << "# " << steer << " " << speed << " " << (flag = !flag) << " #";

            // Store steer and speed in shared memory
            *steerAddr = steer;
            *speedAddr = speed;

            // Send teleop command
            std::cout << client.tcpSend(command.str()) << std::endl;

            // Print teleop command
            printCommand(command.str());

            // Set heartbeat
            *heartbeat = true;
            timer.time(tmr::TIMEOUT_2S);
        }

        Sleep(100);  // 100 ms refresh rate
    }

    client.tcpClose();

    return EXIT_SUCCESS;
}

/**
 * Gets teleop input from key press and packages it as a tuple
 */
static std::tuple<double, double> teleopInput() {
    double steer = 0.0, speed = 0.0;

    if (GetConsoleWindow() == GetForegroundWindow()) {
        if (GetAsyncKeyState('W'))
            speed = 1.0;

        if (GetAsyncKeyState('S'))
            speed = -1.0;

        if (GetAsyncKeyState('A'))
            steer = 40.0;

        if (GetAsyncKeyState('D'))
            steer = -40.0;
    }

    steer = limit(steer, -40, 40);
    speed = limit(speed, -1, 1);

    return {steer, speed};
}

/**
 * Limit some value by min and max
 */
static double limit(double value, double min, double max) {
    if (value > max)
        return max;
    if (value < min)
        return min;
    return value;
}

/**
 * Limit some value by minand max
 */
static void printCommand(std::string command) {
    std::cout << command.c_str() << std::endl;
}
