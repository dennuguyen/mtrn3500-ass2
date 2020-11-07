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

constexpr int numModules = 5;

static void printHeartbeats(bool* heartbeats[]);

int main(int argc, char** argv) {
    // Create the job manager
    //JobManager jm;

    // Set up process management
    sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
    management.createFileMapping();

    // Setup heartbeat map
    bool* heartbeats[numModules];
    *heartbeats = (bool*)(LPWSTR)management.mappedViewAddr();
    for (int i = 0; i < numModules; i++)
        (*heartbeats)[i] = {true};

    // Create processes and create file mapping objects for each process
    std::vector<std::pair<Process, sm::FileMappingObject>> processes;
    processes.reserve(numModules);
    for (mod::ModuleInfo minfo : mod::STARTUP) {
        if (minfo.name == mod::LASER.name || minfo.name == mod::GPS.name || minfo.name == mod::CAMERA.name)
            ;
        else
            continue;

        Process process(minfo);  // create new process
        //jm.attachProcess(process);  // add process to job manager

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
            //printHeartbeats(heartbeats);

            if (process.first.minfo.name == mod::LASER.name) {
                uint16_t* length = (uint16_t*)((char*)process.second.getBaseAddress());
                std::pair<double, double>* points = (std::pair<double, double>*)((char*)process.second.getBaseAddress() + 16);
            }

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
                    //exit(EXIT_FAILURE);
                } else {
                    std::wcout << process.first.minfo.name << " RESTARTING" << std::endl;
                    process.first.kill();
                    process.first.start();
                    process.first.timer.time(tmr::TIMEOUT_2S);
                }
            }
        }

        Sleep(200);  // 200ms refresh rate
    }

    return EXIT_SUCCESS;
}

/*std::wcout << std::setw(24) << std::right << process.first;
            std::wcout << std::setw(6) << std::right << (process.first.isAlive() ? L"Alive" : L"Dead");
            std::wcout << std::endl << std::endl;*/

static void printHeartbeats(bool* heartbeats[]) {
    std::cout << "C D G L T" << std::endl;
    for (int i = 0; i < numModules; i++)
        std::cout << (*heartbeats)[i] << " ";
    std::cout << std::endl
              << std::endl;
}
