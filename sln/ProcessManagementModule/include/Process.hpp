#pragma once

#include <Windows.h>

struct Process {
   public:
	Process(LPWSTR path);
	~Process();

	bool isAlive();

   public:
	LPWSTR path;
	STARTUPINFO sinfo;
	PROCESS_INFORMATION pinfo;
};