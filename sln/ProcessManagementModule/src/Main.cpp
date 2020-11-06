/**
 * Multiple processes open a single file mapping object by name, where the file mapping object maps the process's memory
 * space to the file. The file is a system paging file which acts as shared memory.
 * 
 */

#include <conio.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <Windows.h>

#include "Heartbeats.hpp"
#include "JobManager.hpp"
#include "Modules.hpp"
#include "Process.hpp"
#include "SharedMemory.hpp"

int main(int argc, char** argv) {

	//JobManager jm;  // create the job manager

	// Create processes and create file mapping objects for each process
	std::vector<std::pair<Process, sm::FileMappingObject>> processes;
	processes.reserve(5);
	for (auto [ name, ip, port ] : mod::MODULES) {

		if (name == mod::LASER.name || name == mod::GPS.name)
			;
		else
			continue;

		Process process(name);      // create new process
		//jm.attachProcess(process);  // add process to job manager

		sm::FileMappingObject map(name, sm::SIZE);  // create file mapping object for new process
		map.createFileMapping();                    // create file mapping object handle
		map.mappedViewAddr();                       // get map view base address

		processes.emplace_back(std::move(process), std::move(map));  // move process to vector
	}

	Sleep(1000); // give time for processes to set up

	// Set up process management
	sm::FileMappingObject management(mod::MANAGE.name, sm::SIZE);
	management.createFileMapping();
	hb::Heartbeats* heartbeats = (hb::Heartbeats*)(LPWSTR)management.mappedViewAddr();

	// Enter loop
	while (!_kbhit()) {
		for (auto process : processes) {

			std::cout << heartbeats;

			// Reset heartbeat
			if (*heartbeat.check(process.first.name) == true) {
				*heartbeat = false;
				process.first.timer.time();
			}

			// Monitor processes
			if (process.first.timer.expired()) {

				if (process.first.name == mod::LASER.name ||
					process.first.name == mod::CAMERA.name) {

					std::wcout << process.first.name << " FAILED" << std::endl;
					//exit(EXIT_FAILURE);
				}
				else {
					std::wcout << process.first.name << " RESTARTING" << std::endl;
				}
			}

		}

		Sleep(100);
	}

	return EXIT_SUCCESS;
}

/*std::wcout << std::setw(24) << std::right << process.first;
			std::wcout << std::setw(6) << std::right << (process.first.isAlive() ? L"Alive" : L"Dead");
			std::wcout << std::endl << std::endl;*/