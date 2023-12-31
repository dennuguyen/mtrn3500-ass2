/**
 * Multiple processes open a single file mapping object by name, where the file mapping object maps the process's memory
 * space to the file. The file is a system paging file which acts as shared memory.
 * 
 */

#include <Windows.h>
#include <conio.h>

#include <array>
#include <iomanip>
#include <iostream>
#include <vector>

#include "JobManager.hpp"
#include "Modules.hpp"
#include "Process.hpp"
#include "SharedMemory.hpp"

constexpr int numModules = 6;

static void printHeartbeats(bool* heartbeats[]);

int main(int argc, char* argv[]) {
    // Create the job manager
    //JobManager jm;

    // Set up process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.createFileMapping();

    // Setup heartbeat map
    bool* heartbeats[numModules];
    *heartbeats = (bool*)(char*)management.mappedViewAddr();
    for (int i = 0; i < numModules; i++)
        (*heartbeats)[i] = {true};

    // Create processes and create file mapping objects for each process
    std::vector<std::pair<Process, sm::FileMappingObject>> processes;
    processes.reserve(numModules);
    for (mod::ModuleInfo minfo : mod::STARTUP) {
        Process process(minfo);  // create new process

        // Only add display module to job manager
        //if (process.minfo.name == mod::DISPLAY.name)
        //    jm.attachProcess(process);

        sm::FileMappingObject map(minfo.name, sm::SIZE);  // create file mapping object for new process
        map.createFileMapping();                          // create file mapping object handle
        map.mappedViewAddr();                             // get map view base address

        processes.emplace_back(std::move(process), std::move(map));  // move process to vector
    }

    Sleep(600);  // give time for processes to startup

    // Enter loop
    while (!_kbhit()) {
        for (auto& process : processes) {
            // Printing heartbeats
            printHeartbeats(heartbeats);

            // Reset heartbeat
            if ((*heartbeats)[process.first.minfo.heartbeat] == true) {
                (*heartbeats)[process.first.minfo.heartbeat] = false;
                process.first.timer.time(tmr::TIMEOUT_4S);
            }

            // Catch failed processes
            else if (process.first.timer.expired()) {
                if (process.first.minfo.name == mod::LASER.name ||
                    process.first.minfo.name == mod::CAMERA.name) {
                    std::wcout << process.first.minfo.name << " FAILED" << std::endl;
                    exit(EXIT_FAILURE);
                } else {
                    std::wcout << process.first.minfo.name << " RESTARTING" << std::endl;
                    process.first.kill();
                    process.first.start();
                    process.first.timer.time(tmr::TIMEOUT_4S);
                }
            }
        }

        Sleep(200);  // 200ms refresh rate
    }

    return EXIT_SUCCESS;
}

static void printHeartbeats(bool* heartbeats[]) {
    std::cout << "C D G L M T" << std::endl;
    for (int i = 0; i < numModules; i++)
        std::cout << (*heartbeats)[i] << " ";
    std::cout << std::endl
              << std::endl;
}
