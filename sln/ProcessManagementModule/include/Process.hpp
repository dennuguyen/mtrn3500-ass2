#pragma once

#include <Windows.h>

struct Process {
   public:
    Process(LPWSTR path);
	~Process();

	void restart();
	void openFileMapping(std::wstring name);
	void* mappedViewAddr(int size);
	bool isAlive();
	void kill();

	friend std::wostream& operator<<(std::wostream& output, Process& process);

   public:
	LPWSTR path;                // path to process executable
	STARTUPINFO sinfo;          // startup info
	PROCESS_INFORMATION pinfo;  // process info
	HANDLE handle;              // unique handle to file mapping object

   private:
	void* memptr;  // pointer in map view
};