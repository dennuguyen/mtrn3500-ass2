/**
 * A file mapping object (section object) is a file where its contents are
 * associated with a virtual address space of a process.
 *
 * Useful Resources:
 * 
 */

#pragma once

#include <string>
#include <vector>
#include <Windows.h>

#include "FileMap.hpp"
#include "Process.hpp"

class SharedMemory {
   public:
	SharedMemory();
	~SharedMemory();

	void attachProcess(Process process);
	bool monitor();
	void shutdown();

   private:
	HANDLE handle; // shared memory handle
	std::vector<FileMap> fileMaps; // file mapping objects
	size_t size;
	std::wstring path;
};
