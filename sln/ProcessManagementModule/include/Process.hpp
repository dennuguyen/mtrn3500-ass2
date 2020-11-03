#pragma once

#include <Windows.h>

struct Process {
   public:
	Process(LPWSTR path);
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
