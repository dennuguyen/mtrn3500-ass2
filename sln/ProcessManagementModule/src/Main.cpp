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

#include "JobManager.hpp"
#include "Process.hpp"
#include "FileMappingObject.hpp"

int main(int argc, char** argv) {

	JobManager jm;                 // create the job manager
	FileMappingObject sm(L"sm", 1000);  // create the shared memory space

	// Our processes to manage
	const WCHAR* paths[] = {
		L"DisplayModule.exe",
		L"LaserModule.exe",
		L"CameraModule.exe",
		L"GNSSModule.exe",
		L"VehicleControlModule.exe",
	};

	// Start processes
	std::vector<Process> processes;
	processes.reserve(5);
	for (const WCHAR* path : paths) {
		Process process((LPWSTR)path);          // create new process
		jm.attachProcess(process);              // add process to job manager
		process.openFileMapping(sm.getName());  // access shared memory
		processes.emplace_back(process);        // move process to vector
	}

	std::cout << "BASE ADDR: " << processes[0].mappedViewAddr(100) << std::endl;
	std::cout << "BASE ADDR: " << processes[1].mappedViewAddr(100) << std::endl;
	std::cout << "BASE ADDR: " << processes[2].mappedViewAddr(100) << std::endl;
	std::cout << "BASE ADDR: " << processes[3].mappedViewAddr(100) << std::endl;
	std::cout << "BASE ADDR: " << processes[4].mappedViewAddr(100) << std::endl;

	// Monitoring
	while (!_kbhit()) {
		for (Process process : processes) {
			
			std::wcout << std::setw(24) << std::right << process;
			std::wcout << std::setw(6) << std::right << (process.isAlive() ? L"Alive" : L"Dead");
			std::wcout << std::endl << std::endl;
			
			
			// Restart dead process
			//if (!process.isAlive())
				//process.restart();
		}
	}

	// Shutdown all processes if not shutdown
	for (Process process : processes)
		process.kill();

	return EXIT_SUCCESS;
}