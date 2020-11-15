#include <Winsock2.h>

#include <conio.h>
#include <future>
#include <iostream>
#include <sstream>
#include <thread>

#include "Modules.hpp"
#include "SharedMemory.hpp"
#include "TCPClient.hpp"
#include "Timer.hpp"

static std::tuple<double, double, bool> teleopInput();
static double limit(double value, double min, double max);
static void printCommand(std::string command);

int main(int argc, char* argv[]) {
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

    // Create thread for nonblocking teleop
    std::future<std::tuple<double, double, bool>> teleop = std::async(&teleopInput);

    while (!timer.expired()) {
        if (*heartbeat == false) {

            double steer = 20.0;
            double speed = 0.3;
            bool flag = 1;
            
            // Get teleop values
            // auto [steer, speed, flag] = teleop.get();

            // Send teleop
            std::cout << "Sending command" << std::endl;
            std::stringstream command;
            command << "# " << steer << " " << speed << " " << flag << " #";
            std::cout << client.tcpSend(command.str()) << std::endl;

            // Print sent message
            printCommand(command.str());

            // Give server time to prepare data
            Sleep(500);
            
            // Set heartbeat
            *heartbeat = true;
            timer.time(tmr::TIMEOUT_2S);
        }

        Sleep(100);  // 100 ms refresh rate
    }

    client.tcpClose();

    return EXIT_SUCCESS;
}

static std::tuple<double, double, bool> teleopInput() {

    double steer, speed;
    bool flag;

    // Read pressed keys
    if (KeyManager::get()->isAsciiKeyPressed('w')) {
        speed = 1.0;
    } else {
        speed = 0.0;
    }

    if (KeyManager::get()->isAsciiKeyPressed('a')) {
        speed = -1.0;
    } else {
        speed = 0.0;
    }

    if (KeyManager::get()->isAsciiKeyPressed('s')) {
        steer = 40.0;
    } else {
        steer = 0.0;
    }

    if (KeyManager::get()->isAsciiKeyPressed('d')) {
        steer = -40.0;
    } else {
        steer = 0.0;
    }

    // Get teleop values
    
    std::cout << "<steer> <speed> <flag> ";
    std::cin >> steer >> speed >> flag;

    // Limit teleop values
    steer = limit(steer, -40, 40);
    speed = limit(speed, -1, 1);

    return {steer, speed, flag};
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
