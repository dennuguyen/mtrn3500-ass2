#include <iostream>
#include <vector>

#include "Process.hpp"

Process::Process(LPWSTR path) : path(path), sinfo({}), pinfo({}) {
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

bool Process::isAlive() {
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pinfo.dwProcessId);
	DWORD status = WaitForSingleObject(process, 0);
	CloseHandle(process);
	return status == WAIT_TIMEOUT;
}

bool isProcessIdle() {
	return false;
}

void Process::kill() {
	TerminateProcess(pinfo.hProcess, EXIT_SUCCESS);
}

std::wostream& operator<<(std::wostream& output, Process& process) {
	output << process.path;
	return output;
}
