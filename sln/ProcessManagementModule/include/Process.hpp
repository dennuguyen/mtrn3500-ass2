#ifndef PROCESS_HPP_
#define PROCESS_HPP_

#include <Windows.h>

#include "Modules.hpp"
#include "Timer.hpp"

struct Process {
   public:
	Process(mod::ModuleInfo minfo) : minfo(minfo), sinfo({}), pinfo({}), timer(tmr::Timer()) { start(); }
	Process(const Process& proc) : minfo(proc.minfo), sinfo(proc.sinfo), pinfo(proc.pinfo), timer(proc.timer) {}
	Process(Process&& proc) noexcept : minfo(std::move(proc.minfo)), sinfo(std::move(proc.sinfo)), pinfo(std::move(proc.pinfo)), timer(std::move(timer)) {}
	~Process() {}

	void start() {
		if (!isAlive())
			if (CreateProcess(NULL,
				(LPWSTR)minfo.name.c_str(),
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

	bool isAlive() {
		HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pinfo.dwProcessId);
		DWORD status = WaitForSingleObject(process, 0);
		CloseHandle(process);
		return status == WAIT_TIMEOUT;
	}

	void kill() {
		TerminateProcess(pinfo.hProcess, EXIT_SUCCESS);
	}

	friend std::wostream& operator<<(std::wostream& output, Process& process) {
		output << process.minfo.name;
		return output;
	}

   public:
	mod::ModuleInfo minfo;      // module info
	STARTUPINFO sinfo;          // startup info
	PROCESS_INFORMATION pinfo;  // process info
	tmr::Timer timer;           // timer
};

#endif  // PROCESS_HPP_
