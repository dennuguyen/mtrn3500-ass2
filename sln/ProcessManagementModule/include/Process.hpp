#pragma once

#include <Windows.h>

struct Process {
   public:
	Process(LPWSTR path);
	Process(const Process& proc) : path(proc.path), sinfo(proc.sinfo), pinfo(proc.pinfo) {}
	Process(Process&& proc) noexcept : path(std::move(proc.path)), sinfo(std::move(proc.sinfo)), pinfo(std::move(proc.pinfo)) {}
	~Process();

	void restart();
	bool isAlive();
	bool isIdle();
	void kill();

	friend std::wostream& operator<<(std::wostream& output, Process& process);

   public:
	LPWSTR path;                // path to process executable
	STARTUPINFO sinfo;          // startup info
	PROCESS_INFORMATION pinfo;  // process info
};
