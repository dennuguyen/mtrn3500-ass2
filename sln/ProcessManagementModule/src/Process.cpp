#include <iostream>
#include <vector>

#include "Process.hpp"

Process::Process(LPWSTR path) : path(path), sinfo({}), pinfo({}), handle() {
    if (CreateProcess(NULL,
		              (LPWSTR)std::wstring(path).c_str(),
		              NULL,
		              NULL,
		              FALSE,
		              CREATE_NEW_CONSOLE,
		              NULL,
		              NULL,
		              &sinfo,
		              &pinfo) == FALSE) {
		std::cerr << "Error: Process could not be created" << std::endl;
		exit(EXIT_FAILURE);
	}
}

Process::~Process() {}

void Process::restart() {
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pinfo.dwProcessId);
}

void Process::openFileMapping(std::wstring name) {
	handle = OpenFileMapping(FILE_MAP_ALL_ACCESS,
		                     FALSE,
		                     name.c_str());

	if (handle == INVALID_HANDLE_VALUE) {
		std::cerr << "ERROR " << INVALID_HANDLE_VALUE << std::endl;
		exit(EXIT_FAILURE);
	}
}

void* Process::mappedViewAddr(int size) {
	memptr = MapViewOfFile(handle,
		                   FILE_MAP_ALL_ACCESS,
		                   0,
		                   0,
		                   size);
	return memptr;
}

bool Process::isAlive() {
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pinfo.dwProcessId);
	DWORD status = WaitForSingleObject(process, 0);
	CloseHandle(process);
	return status == WAIT_TIMEOUT;
}

void Process::kill() {
	CloseHandle(handle);
	TerminateProcess(pinfo.hProcess, EXIT_SUCCESS);
}

std::wostream& operator<<(std::wostream& output, Process& process) {
	output << process.path;
	return output;
}