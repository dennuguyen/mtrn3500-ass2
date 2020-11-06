#ifndef PROCESS_HPP_
#define PROCESS_HPP_

#include <Windows.h>

#include "Timer.hpp"

struct Process {
   public:
	Process(std::wstring name) : name(name), sinfo({}), pinfo({}), timer(tmr::Timer(tmr::TIMEOUT_4S)) {
		start();
	}

	Process(const Process& proc) : name(proc.name), sinfo(proc.sinfo), pinfo(proc.pinfo), timer(proc.timer) {}
	Process(Process&& proc) noexcept : name(std::move(proc.name)), sinfo(std::move(proc.sinfo)), pinfo(std::move(proc.pinfo)), timer(std::move(timer)) {}
	~Process() {}

	void start() {
		if (CreateProcess(NULL,
			(LPWSTR)std::wstring(name).c_str(),
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
		output << process.name;
		return output;
	}

   public:
	std::wstring name;          // name of process
	STARTUPINFO sinfo;          // startup info
	PROCESS_INFORMATION pinfo;  // process info
	tmr::Timer timer;            // timer

};

#endif  // PROCESS_HPP_
