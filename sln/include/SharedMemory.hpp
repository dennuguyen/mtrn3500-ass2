#ifndef SM_HPP_
#define SM_HPP_

#include <string>
#include <vector>
#include <Windows.h>

namespace sm {
// Shared memory size
size_t SIZE = 1024;

// Constant module strings
const WCHAR CAMERA[] = L"CameraModule.exe";
const WCHAR DISPLAY[] = L"DisplayModule.exe";
const WCHAR GPS[] = L"GNSSModule.exe";
const WCHAR LASER[] = L"LaserModule.exe";
const WCHAR MANAGE[] = L"ProcessManagementModule.exe";
const WCHAR TELEOP[] = L"VehicleControlModule.exe";
const WCHAR* modules[] = {
    DISPLAY,
    LASER,
    GPS,
    CAMERA,
    TELEOP,
};

/**
 * A file mapping object (section object) is a file where its contents are
 * associated with a virtual address space of a process.
 */
class FileMappingObject {
   public:
	FileMappingObject(std::wstring name, size_t size)
		: name(name)
		, size(size)
		, createHandle()
		, accessHandle()
		, baseAddress()
	{}

	~FileMappingObject() {
		if (createHandle != NULL)
			if (CloseHandle(createHandle) == false)
				std::wcerr << L"WARNING: Could not close handle" << createHandle << std::endl;

		if (accessHandle != NULL)
			if (CloseHandle(accessHandle) == false)
				std::wcerr << L"WARNING: Could not close handle" << accessHandle << std::endl;

		if (UnmapViewOfFile(baseAddress) == 0)
			std::wcerr << L"WARNING: Could not unmap " << name << std::endl;
	}

	HANDLE createFileMapping() {
		createHandle = CreateFileMapping(INVALID_HANDLE_VALUE,
			                             NULL,
			                             PAGE_READWRITE,
			                             0,
			                             size,
			                             name.c_str());

		if (createHandle == INVALID_HANDLE_VALUE) {
			std::wcerr << L"ERROR: " << INVALID_HANDLE_VALUE << std::endl;
			exit(EXIT_FAILURE);
		}

		return createHandle;
	}

	HANDLE openFileMapping() {
		accessHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS,
			                           FALSE,
			                           name.c_str());

		if (accessHandle == INVALID_HANDLE_VALUE) {
			std::wcerr << L"ERROR: " << INVALID_HANDLE_VALUE << std::endl;
			exit(EXIT_FAILURE);
		}

		return accessHandle;
	}

	LPVOID mappedViewAddr() {
		baseAddress = MapViewOfFile(accessHandle,
			                        FILE_MAP_ALL_ACCESS,
			                        0,
			                        0,
			                        size);
		return baseAddress;
	}

	const std::wstring getName() { return name; }
	const size_t getSize() { return size; }
	const HANDLE getCreateHandle() { return createHandle; }
	const HANDLE getAccessHandle() { return accessHandle; }
	const void* getBaseAddress() { return baseAddress; }

   private:
	std::wstring name;    // name of file mapping object
	size_t size;          // size of file mapping object
	HANDLE createHandle;  // unique handle to file mapping object
	HANDLE accessHandle;  // unique handle to access file mapping object
	LPVOID baseAddress;   // base address of map view
};

}  // namespace sm

#endif  // SM_HPP_