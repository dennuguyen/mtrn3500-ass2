#include <iostream>
#include <vector>
#include <Windows.h>

#include "Process.hpp"
#include "SharedMemory.hpp"

int main(int argc, char** argv) {

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
	for (const WCHAR* path : paths)
		processes.push_back(Process((LPWSTR)path));

	
	/*
	// Create the shared memory space
	SharedMemory sm;

	// Start and attach processes to shared memory space
	for (const WCHAR* path : paths)
		sm.attachProcess(Process((LPWSTR)path));

	// Monitoring
	while (sm.monitor());
	
	// Shutdown
	sm.shutdown();
	*/
	while (TRUE) {
		std::cout << "e";
		std::cout << ":" << processes.at(0).isAlive();
		Sleep(1000);
	}

	return EXIT_SUCCESS;
}