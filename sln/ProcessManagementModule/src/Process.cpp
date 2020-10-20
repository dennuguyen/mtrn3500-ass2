#include <iostream>
#include <vector>

#include "Process.hpp"

Process::Process(LPWSTR path) : path(path), sinfo({}), pinfo({}) {
    if (CreateProcess(NULL,
		              (LPWSTR)std::wstring(path).c_str(),
		              NULL,
		              NULL,
		              FALSE,
		              CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP,
		              NULL,
		              NULL,
		              &sinfo,
		              &pinfo) == FALSE) {
		std::cerr << "Error: Process could not be created" << std::endl;
		exit(EXIT_FAILURE);
	}
}

Process::~Process() {
	CloseHandle(pinfo.hProcess);
}

bool Process::isAlive() {
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pinfo.dwProcessId);
	DWORD status = WaitForSingleObject(process, 0);
	CloseHandle(process);
	return status == WAIT_TIMEOUT;
}
