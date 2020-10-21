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

#include "Process.hpp"

class FileMappingObject {
   public:
	FileMappingObject(std::wstring name, size_t size);
	~FileMappingObject();

	std::wstring getName();
	size_t getSize();

   private:
	std::wstring name;  // name of file mapping object
	size_t size;        // size of file mapping object
	HANDLE handle;      // handle of file mapping object
};
