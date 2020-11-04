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
#include "SharedMemory.hpp"

int main(int argc, char** argv) {

	JobManager jm;  // create the job manager

	// Start processes and create file mapping objects for each process
	std::vector<std::pair<Process, sm::FileMappingObject>> processes;
	processes.reserve(5);
	for (const WCHAR* path : sm::modules) {

		if (path != sm::LASER)
			continue;

		Process process((LPWSTR)path);  // create new process
		jm.attachProcess(process);      // add process to job manager

		sm::FileMappingObject map(path, sm::SIZE);  // create file mapping object for new process
		map.createFileMapping();                    // create file mapping object handle
		map.mappedViewAddr();                       // get map view base address

		processes.emplace_back(std::move(process), std::move(map));  // move process to vector
	}

	Sleep(1000); // give time for processes to set up

	int* addr = (int*)((LPWSTR)processes[0].second.getBaseAddress() + 10);
	std::wcout << *addr << std::endl;

	// Monitoring
	while (!_kbhit()) {
		for (auto process : processes) {
			std::wcout << std::setw(24) << std::right << process.first;
			std::wcout << std::setw(6) << std::right << (process.first.isAlive() ? L"Alive" : L"Dead");
			std::wcout << std::endl << std::endl;

			int* addr = (int*)((LPWSTR)process.second.getBaseAddress() + 10);
			std::wcout << *addr << std::endl;

			// Restart dead process
			if (!process.first.isAlive()) {
				process.first.kill();
			}
		}
	}

	// Shutdown all processes if not shutdown
	for (auto process : processes) {
		process.second.close();
		process.first.kill();
	}

	return EXIT_SUCCESS;
}
